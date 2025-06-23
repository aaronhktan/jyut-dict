import jieba
import opencc
import pinyin_jyutping_sentence
import pycantonese
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
import requests
from wordfreq import zipf_frequency

from collections import defaultdict
from database import database, objects
import datetime
import logging
import re
import sqlite3
import sys
import time
import traceback

yue_converter = opencc.OpenCC("hk2s.json")
zh_converter = opencc.OpenCC("tw2s.json")

SUPERSCRIPT_EQUIVALENT = str.maketrans("¹²³⁴⁵⁶⁷⁸⁹⁰", "1234567890", "")
JYUTPING_REGEX = re.compile(r"(.*?)（粵拼：(.*?)[）|；|，|/]")
LITERARY_CANTONESE_READING_REGEX_PATTERN = re.compile(r"\d\*")
HAN_REGEX = re.compile(r"[\u4e00-\u9fff]")

logging.getLogger().setLevel(logging.INFO)

def insert_words(c, words):
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


def get_summaries(wiki_lang, titles):
    url = f"https://{wiki_lang}.wikipedia.org/w/api.php?exintro&explaintext&redirects"

    params = {
        "prop": "extracts|pageprops",
        "titles": titles,
        "exlimit": "20",
        "exsentences": "1",
        "format": "json",
        "action": "query",
    }

    timeouts = 0
    while True:
        try:
            resp = requests.get(url=url, params=params, timeout=30)
            break
        except (requests.ConnectionError, requests.Timeout):
            logging.warning(f"Timed out for words {titles}, retrying")
            timeouts += 1
            time.sleep(120 * timeouts)
        except Exception as e:
            logging.error(e)
            break

    data = resp.json()

    parsed = dict()
    if "batchcomplete" not in data:
        logging.warning(f"Batch complete was not available in response {resp.url}")

    for page_id in data["query"]["pages"]:
        page = data["query"]["pages"][page_id]

        # Skip disambiguation pages
        if "pageprops" in page:
            if "disambiguation" in page["pageprops"]:
                continue

        parsed[page["title"]] = (
            page["extract"].replace(" ", "ﾠ") if "extract" in page else None
        )

    if "redirects" in data["query"]:
        for redirect in data["query"]["redirects"]:
            if redirect["to"] in parsed:
                parsed[redirect["from"]] = parsed[redirect["to"]]
            else:
                parsed[redirect["from"]] = None

    return parsed


def parse_file(page_filepath, langlinks_filepath, lang_src, lang_dest, words):
    match lang_src:
        case "zh-yue":
            converter = yue_converter
        case "zh":
            converter = zh_converter
        case other:
            logging.error(f"Source language must be zh or zh-yue, received {lang_src}")
            return

    db = sqlite3.connect(page_filepath)
    c = db.cursor()

    if lang_src != lang_dest:
        c.execute(f"ATTACH DATABASE '{langlinks_filepath}' AS langlinks")

        # Get the list of all non-redirect article pages in this Wikipedia
        c.execute(
            (
                """SELECT 
               page_title, l.ll_title 
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
               ll_lang = ?"""
            ),
            (lang_dest,),
        )
        rows = c.fetchall()
    else:
        c.execute(
            (
                """SELECT 
               p.page_title, o.page_title 
             FROM 
               page AS p
             JOIN 
               page AS o
             ON 
               p.page_id = o.page_id 
             WHERE 
               p.page_namespace = 0 
             AND 
               p.page_is_redirect = 0"""
            )
        )
        rows = c.fetchall()

    for i in range(0, len(rows), 20):
        # The maximum batch amount for a single Wikimedia API request is 20 items
        src_strings = [str(x[0]).replace("_", " ") for x in rows[i : i + 20]]
        dest_strings = [str(x[1]).replace("_", " ") for x in rows[i : i + 20]]
        correspondences = dict(zip(src_strings, dest_strings))

        if not i % 100 and i:
            logging.info(
                f"Processed entry #{i} at time {datetime.datetime.now().time()}"
            )

        src_summaries = get_summaries(lang_src, "|".join(src_strings))
        if lang_src != lang_dest:
            dest_summaries = get_summaries(lang_dest, "|".join(dest_strings))
        else:
            dest_summaries = src_summaries

        for trad in src_strings:
            if trad not in src_summaries:
                continue

            summary = src_summaries[trad]

            simp = converter.convert(trad)
            jyutping_match = JYUTPING_REGEX.search(summary) if summary else None
            jyut = ""
            if jyutping_match:
                jyutping_trad = jyutping_match.group(1)
                if jyutping_trad == trad:
                    jyut = jyutping_match.group(2)
                    jyut = jyut.translate(SUPERSCRIPT_EQUIVALENT)
                    jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN.sub("", jyut)
                    jyut = jyut.replace("ﾠ", " ")
            if not jyutping_match or not jyut:
                jyut = pinyin_jyutping_sentence.jyutping(
                    trad.replace("·", ""),
                    tone_numbers=True,
                    spaces=True,
                )

                # If pinyin_jyutping_sentences cannot convert to Jyutping, use
                # pycantonese to convert the character instead
                han_chars = HAN_REGEX.findall(jyut)
                for char in han_chars:
                    char_jyutping = pycantonese.characters_to_jyutping(char)[0][1]
                    if char_jyutping:
                        jyut = jyut.replace(char, char_jyutping)

            pin = (
                " ".join(
                    lazy_pinyin(
                        simp.replace("·", ""),
                        style=Style.TONE3,
                        neutral_tone_with_five=True,
                        v_to_u=True,
                    )
                )
                .lower()
                .replace("ü", "u:")
            )

            definition_components = []
            dest_key = correspondences[trad]
            if lang_src != lang_dest:
                definition_components.append(dest_key)
            if dest_key in dest_summaries:
                dest_summary = dest_summaries[dest_key]
                if dest_summary:
                    if lang_dest not in ("zh", "zh-yue"):
                        dest_summary = dest_summary.replace(" ", "ﾠ")
                    definition_components.append(dest_summary)

            definition = objects.Definition(definition="\n".join(definition_components))
            freq = zipf_frequency(trad, "zh")

            entry = objects.Entry(
                trad=trad, simp=simp, jyut=jyut, pin=pin, freq=freq, defs=[definition]
            )
            words[trad].append(entry)


if __name__ == "__main__":
    if len(sys.argv) != 14:
        print(
            (
                "Usage: python3 -m wiki.parse <database filename> "
                "<page.db file> <langlinks.db file> <source language> "
                "<destination language> <source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source contents>"
            )
        )
        print(
            (
                "e.g. python3 -m wiki.parse wikipedia/developer/wikipedia.db "
                "wikipedia/data/page.db wikipedia/data/langlinks.db zh-yue en Wikipedia WK 2024-01-01 "
                '"Wikipedia is a free-content online encyclopedia, written and maintained '
                "by a community of volunteers, collectively known as Wikipedians, through open "
                'collaboration and the use of wiki-based editing system MediaWiki." '
                '"Text is available under the Creative Commons Attribution-ShareAlike License 4.0." '
                '"https://www.wikipedia.org/" "" "words"'
            )
        )
        sys.exit(1)

    cc_cedict.load()

    source = objects.SourceTuple(
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10],
        sys.argv[11],
        sys.argv[12],
        sys.argv[13],
    )
    # logging.basicConfig(level='INFO') # Uncomment to enable debug logging
    parsed_words = defaultdict(list)
    parse_file(sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], parsed_words)
    write(sys.argv[1], source, parsed_words)
