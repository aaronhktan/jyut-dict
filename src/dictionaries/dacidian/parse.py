from dragonmapper import transcriptions
from hanziconv import HanziConv
import jieba
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects
from database.utils import change_pinyin_to_match_phrase

from collections import defaultdict, namedtuple
import csv
import logging
import re
import sqlite3
import sys

DefinitionTuple = namedtuple("Definition", ["definition", "label", "examples"])
ExampleTuple = namedtuple("ExampleTuple", ["lang", "pron", "content"])


def read_csv(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            yield row


def insert_example(c, definition_id, starting_example_id, example):
    examples_inserted = 0

    trad = example.content
    simp = HanziConv.toSimplified(trad)
    jyut = ""
    pin = example.pron
    lang = example.lang

    example_id = database.insert_chinese_sentence(
        c, trad, simp, pin, jyut, lang, starting_example_id
    )

    # Check if example insertion was successful
    if example_id == -1:
        return 0
    else:
        examples_inserted += 1

    database.insert_definition_chinese_sentence_link(c, definition_id, example_id)

    return examples_inserted


def insert_words(c, words):
    # Reserved sentence IDs:
    #   - 0-999999999: Tatoeba
    #   - 1000000000-1999999999: words.hk
    #   - 2000000000-2999999999: CantoDict
    #   - 3000000000-3999999999: MoEDict
    #   - 4000000000-4999999999: Cross-Straits Dictionary
    #   - 5000000000-5999999999: Zhonghuayu Dacidian
    example_id = 5000000000

    for key in words:
        for entry in words[key]:
            entry_id = database.get_entry_id(
                c,
                entry.traditional,
                entry.simplified,
                entry.pinyin,
                entry.jyutping,
                entry.freq,
            )

            if entry_id == -1:
                entry_id = database.insert_entry(
                    c,
                    entry.traditional,
                    entry.simplified,
                    entry.pinyin,
                    entry.jyutping,
                    entry.freq,
                )
                if entry_id == -1:
                    logging.error(f"Could not insert word {entry.traditional}, uh oh!")
                    continue

            for definition in entry.definitions:
                definition_id = database.insert_definition(
                    c, definition.definition, definition.label, entry_id, 1, None
                )
                if definition_id == -1:
                    logging.error(
                        f"Could not insert definition {definition} for word {entry.traditional} "
                        "- check if the definition is a duplicate!"
                    )
                    continue

                for example in definition.examples:
                    examples_inserted = insert_example(
                        c, definition_id, example_id, example
                    )
                    example_id += examples_inserted


def write(entries, db_name):
    db = sqlite3.connect(db_name)
    c = db.cursor()

    database.write_database_version(c)
    database.drop_tables(c)
    database.create_tables(c)
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

    insert_words(c, entries)

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_file(filename, entries):
    for line in read_csv(filename):
        trad = line[5]
        # Simplified form is not provided if it is the same as traditional
        simp = line[6] if line[6] else trad

        # mainland_pron is set to True if different pronunciations are provided in columns 12 and 14
        if line[13] and line[13] != line[11]:
            mainland_pron = True
        else:
            mainland_pron = False

        pins = [line[12]]
        if mainland_pron:
            pins.append(line[14])

        try:
            # Converting from pinyin -> zhuyin inserts spaces between each character's pinyin
            # Converting from zhuyin -> pinyin conserves these spaces
            pins = [
                transcriptions.zhuyin_to_pinyin(
                    transcriptions.pinyin_to_zhuyin(line[11]), accented=False
                )
                for x in pins
            ]

            for x in pins:
                if x.count(" ") >= len(trad):
                    # This means that there was an extra space inserted somewhere; the pinyin is not valid
                    raise ValueError("Too many spaces in parsed Pinyin!")
        except Exception as e:
            # Parse zhuyin as backup
            pins = [line[11]]
            if mainland_pron:
                pins.append(line[11])

            # # Remove weird spaces in zhuyin
            # pins = list(map(lambda x: x.replace("　", " "), pins))

            try:
                pins = [
                    transcriptions.zhuyin_to_pinyin(x, accented=False)
                    for x in pins
                ]
            except Exception as e:
                logging.error(
                    f"Unable to split up Pinyin for word {trad}: {e}, skipping word..."
                )
                continue

        taiwan_pin = pins[0]
        if mainland_pron:
            mainland_pin = pins[1]

        jyut = pinyin_jyutping_sentence.jyutping(trad, tone_numbers=True, spaces=True)

        taiwan_defs = []
        if mainland_pron:
            mainland_defs = []
        # Columns 15-42 contain definitions in the form definition[例]example｜example
        for i in range(14, 44):
            # If there is nothing in this column, there are no further definitions for this word
            if not line[i]:
                break

            split_line = line[i].split("[例]")

            definition = split_line[0][2:] if split_line[0][1] == "." else split_line[0]
            taiwan_def_tuple = DefinitionTuple("​".join(jieba.cut(definition)), "", [])
            taiwan_defs.append(taiwan_def_tuple)
            if mainland_pron:
                mainland_def_tuple = DefinitionTuple(
                    "​".join(jieba.cut(definition)), "", []
                )
                mainland_defs.append(mainland_def_tuple)

            if len(split_line) >= 2:
                examples = split_line[1][:-1].replace("～", trad).split("｜")

                for example in examples:
                    example = re.sub(r"（.*）", "", example)  # fixes parsing for 哪 / nɑ5 (extra items in bracket)
                    example = example.replace("　", "")  # fixes parsing for 祺 / qi2 (full-width spaces in example)
                    # fixes parsing for 悸 / ji4 (spaces at the beginning or end of example)
                    # and 九錫 / jiu3 xi2 (spaces in middle of example)
                    example = example.replace(" ", "")
                    example_pinyin = lazy_pinyin(
                        " ".join(example).split(),
                        style=Style.TONE3,
                        neutral_tone_with_five=True,
                    )
                    example_pinyin = " ".join(example_pinyin).lower()
                    example_pinyin = example_pinyin.strip().replace("v", "u:")
                    try:
                        taiwan_example_pinyin = change_pinyin_to_match_phrase(
                            example, example_pinyin, trad, taiwan_pin
                        )
                    except:
                        print(f"{example}, {example_pinyin}, {trad}, {taiwan_pin}")
                    taiwan_def_tuple.examples.append(
                        ExampleTuple("cmn", taiwan_example_pinyin, example)
                    )
                    if mainland_pron:
                        try:
                            mainland_example_pinyin = change_pinyin_to_match_phrase(
                                example, example_pinyin, trad, mainland_pin
                            )
                        except:
                            print(f"{trad}, {taiwan_pin}, {example}")
                        mainland_def_tuple.examples.append(
                            ExampleTuple("cmn", mainland_example_pinyin, example)
                        )

        entry = objects.Entry(
            trad=trad, simp=simp, pin=taiwan_pin, jyut=jyut, defs=taiwan_defs
        )
        entries[trad].append(entry)

        if mainland_pron:
            entry = objects.Entry(
                trad=trad, simp=simp, pin=mainland_pin, jyut=jyut, defs=mainland_defs
            )
            entries[trad].append(entry)


def assign_frequencies(entries):
    for key in entries:
        for entry in entries[key]:
            freq = zipf_frequency(entry.traditional, "zh")
            entry.add_freq(freq)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<Dacidian file> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py dacidian.db 中華語文大辭典全稿-20160620.csv "
                "中華語文大辭典 DCD 2016-06-20 "
                '"本辭典為《兩岸常用詞典》的擴編，以收釋現代漢民族共同語（臺灣稱「國語」，大陸稱「普通話」'
                "中的通用詞語為主，同時適當收釋一些雙方各自特有而常用的詞語，反映兩岸用法異同，"
                "以方便兩岸交流和一般民眾使用。截至2015年12月，本辭典已收10,943字（含多音字則為13,004字）"
                '、複音詞和固定短語88,735條，合計共99,678條（含多音字則為101,739條）。" '
                '"《中華語文大辭典》由中華文化總會以 CC BY-NC-ND 4.0 之条款下提供。" '
                '"http://www.chinese-linguipedia.org/search.html" "" "words,sentences"'
            )
        )
        sys.exit(1)

    cc_cedict.load()

    entries = defaultdict(list)
    source = objects.SourceTuple(
        sys.argv[3],
        sys.argv[4],
        sys.argv[5],
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10],
    )
    parse_file(sys.argv[2], entries)
    # assign_frequencies(entries)
    # write(entries, sys.argv[1])
