import jieba
import opencc
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency
import wikipedia

from database import database, objects

from collections import defaultdict, namedtuple
import copy
import csv
from itertools import chain
import logging
import re
import sqlite3
import sys
import time
import traceback

yue_converter = opencc.OpenCC("hk2s.json")
zh_converter = opencc.OpenCC("tw2s.json")
wikipedia.set_rate_limiting(True)


def insert_words(c, words):
    # Because the sentence id is presumed to be unique by Tatoeba, we will give it
    # a namespace of 999999999 potential sentences. Thus, words.hk sentences will start
    # at rowid 1000000000.
    example_id = 1000000000

    for key in words:
        for entry in words[key]:
            trad = entry.traditional
            simp = entry.simplified
            jyut = entry.jyutping
            pin = entry.pinyin
            freq = entry.freq

            entry_id = database.get_entry_id(c, trad, simp, pin, jyut, freq)

            if entry_id == -1:
                entry_id = database.insert_entry(c, trad, simp, pin, jyut, freq, None)
                if entry_id == -1:
                    logging.warning(f"Could not insert word {trad}, uh oh!")
                    continue

            # Insert each meaning for the entry
            for definition in entry.definitions:
                definition_id = database.insert_definition(
                    c, definition.definition, definition.label, entry_id, 1, None
                )
                if definition_id == -1:
                    # Try to find definition if we got an error
                    definition_id = database.get_definition_id(
                        c, definition.definition, definition.label, entry_id, 1
                    )
                    if definition_id == -1:
                        logging.warning(
                            f"Could not insert definition {definition} for word {trad}, uh oh!"
                        )
                        continue


def write(db_name, source, words):
    print("Writing to database file")

    db = sqlite3.connect(db_name)
    c = db.cursor()

    database.write_database_version(c)

    database.drop_tables(c)
    database.create_tables(c)

    # Add source information to table
    database.insert_source(
        c,
        source.name,
        source.shortname,
        source.version,
        source.description,
        source.legal,
        source.link,
        source.update_url,
        source.other,
        None,
    )

    insert_words(c, words)

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_file(page_filepath, langlinks_filepath, words):
    db = sqlite3.connect(page_filepath)
    c = db.cursor()

    c.execute(f"ATTACH DATABASE '{langlinks_filepath}' AS langlinks")

    # Get the list of all non-redirect article pages in this Wikipedia
    c.execute(
        ("""SELECT 
           page_id, page_title, l.ll_title 
         FROM 
           page AS p
         JOIN 
           langlinks.langlinks AS l
         ON 
           p.page_id = l.ll_from 
         WHERE 
           page_namespace = 0 
         AND 
           page_is_redirect = 0 
         AND 
           ll_lang = 'en'""")
    )
    rows = c.fetchall()

    for i, row in enumerate(rows):
        if not i % 100:
            logging.info(f"Processed entry #{i}")

        trad = str(row[1]).replace("_", " ")
        simp = yue_converter.convert(trad)
        jyut = pinyin_jyutping_sentence.jyutping(
                trad, tone_numbers=True, spaces=True
            )
        pin = (
            " ".join(lazy_pinyin(simp, style=Style.TONE3, neutral_tone_with_five=True))
            .lower()
            .replace("v", "u:")
        )

        if "搞清楚" in trad:
            # This is a disambiguation page, skip
            continue

        wikipedia.set_lang("zh-yue")
        definition_components = []
        cannot_be_parsed = False
        timeouts = 0
        while True:
            try:
                definition_components.append(wikipedia.summary(trad, auto_suggest=False, sentences=1))
                break
            except wikipedia.exceptions.DisambiguationError:
                logging.warning(f"DisambiguationError for word {trad}")
                cannot_be_parsed = True
                break
            except TimeoutError:
                logging.warning(f"Timed out for word {trad}, retrying")
                timeouts += 1
                time.sleep(120 * timeouts)
            except Exception as e:
                logging.error(e)
                break
        if cannot_be_parsed:
            continue

        wikipedia.set_lang("en")
        english_term = str(row[2])
        definition_components.append(english_term)
        timeouts = 0
        while True:
            try:
                english_description = wikipedia.summary(english_term, auto_suggest=False, sentences=1)
                english_description = english_description.replace(" ", "ﾠ")
                definition_components.append(english_description)
                break
            except wikipedia.exceptions.DisambiguationError:
                logging.warning(f"DisambiguationError for English word {english_term}")
                break
            except TimeoutError:
                logging.warning(f"Timed out for English word {english_term}, retrying")
                timeouts += 1
                time.sleep(120 * timeouts)
            except Exception as e:
                logging.error(e)
                break

        definition = objects.Definition(definition="\n".join(definition_components))
        freq = zipf_frequency(trad, "zh")

        entry = objects.Entry(trad=trad, simp=simp, jyut=jyut, pin=pin, freq=freq, defs=[definition])
        words[trad].append(entry)


if __name__ == "__main__":
    if len(sys.argv) != 12:
        print(
            (
                "Usage: python3 -m wikipedia.parse <database filename> "
                "<page.db file> <langlinks.db file> <source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m wikipedia.parse wikipedia/developer/wikipedia.db "
                "wikipedia/data/page.db wikipedia/data/langlinks.db Wikipedia WK 2024-01-01 "
                '"Wikipedia[note 3] is a free-content online encyclopedia, written and maintained '
                'by a community of volunteers, collectively known as Wikipedians, through open '
                'collaboration and the use of wiki-based editing system MediaWiki." '
                '"https://en.wikipedia.org/wiki/Wikipedia:Text_of_the_Creative_Commons_Attribution-ShareAlike_4.0_International_License"'
                '"https://www.wikipedia.org/" "" ""'
            )
        )
        sys.exit(1)

    cc_cedict.load()

    source = objects.SourceTuple(
        sys.argv[4],
        sys.argv[5],
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10],
        sys.argv[11],
    )
    logging.basicConfig(level='INFO') # Uncomment to enable debug logging
    parsed_words = defaultdict(list)
    parse_file(sys.argv[2], sys.argv[3], parsed_words)
    write(sys.argv[1], source, parsed_words)
