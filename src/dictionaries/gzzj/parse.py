import opencc
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict
import copy
import csv
import logging
import sqlite3
import sys

converter = opencc.OpenCC("hk2s.json")

NUMERICAL_VALUES = str.maketrans("①②③④⑤⑥⑦⑧⑨⑩", "\n\n\n\n\n\n\n\n\n\n", "㈠㈡㈢㈣")


def insert_words(c, words):
    for char in words:
        for entry in words[char]:
            entry_id = database.get_entry_id(
                c,
                entry.traditional,
                entry.simplified,
                entry.pinyin,
                entry.jyutping,
                entry.freq,
            )

            already_in_db = (entry_id == -1)

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

            if not already_in_db and len(words[char]) > 1:
                other_pronunciations = set()
                for other_entry in words[char]:
                    if other_entry.jyutping == entry.jyutping:
                        continue
                    other_pronunciations.add(
                        other_entry.traditional + " " + other_entry.jyutping
                    )
                entry.append_to_defs(
                    objects.DefinitionTuple(
                        "，".join(list(other_pronunciations)), "參看", []
                    )
                )

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


def write(db_name, source, words):
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

    insert_words(c, words)

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_file(filename, words):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile, delimiter="\t")
        for row in reader:
            traditional_variants = row["字頭"].split("|")
            trad = traditional_variants[0]
            simp = converter.convert(trad)

            pin = lazy_pinyin(
                simp,
                style=Style.TONE3,
                neutral_tone_with_five=True,
            )
            pin = " ".join(pin).lower()
            pin = pin.strip().replace("v", "u:")
            jyut = row["粵拼讀音"]

            freq = zipf_frequency(trad, "zh")

            defs = []
            if row["釋義"]:
                explanation = row["釋義"]
                explanation = explanation.translate(NUMERICAL_VALUES)
                definitions = explanation.split()
                for definition in definitions:
                    defs.append(objects.DefinitionTuple(definition, "釋義", []))
            if row["(輔助檢索用異體)"]:
                defs.append(objects.DefinitionTuple(row["(輔助檢索用異體)"], "異體", []))

            entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=defs)
            words[trad].add(entry)

            for variant in traditional_variants[1:]:
                trad = variant
                simp = converter.convert(trad)
                pin = lazy_pinyin(
                    simp,
                    style=Style.TONE3,
                    neutral_tone_with_five=True,
                )
                pin = " ".join(pin).lower()
                pin = pin.strip().replace("v", "u:")
                freq = zipf_frequency(trad, "zh")
                entry = objects.Entry(variant, simp, pin, jyut, freq=freq, defs=copy.deepcopy(defs))
                words[trad].add(entry)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 -m jyutnet.parse <database filename> "
                "<B01_讀音資料.csv> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m gzzj.parse ./gzzj/developer/gzzj.db "
                "./gzzj/data/B01_讀音資料.txt "
                "廣州話正音字典 GZZJ 2023-03-13 "
                '"1988 年，香港教育署語文教育學院中文系成立「常用字廣州話讀音研究委員會」，委員會由香港各大專院校學者組成，'
                "為坊間辭書不一致的讀音進行審音工作，其成果見於 1990 年的《常用字廣州話讀音表》。\n\n"
                "而在 1990 年，廣東語言學界成立「廣州話審音委員會」，開展了繼《常用字廣州話讀音表》後，第二次規模較大的粵語審音工作。"
                "委員會成員包括粵、港、澳三地二十多名學者；成果則見於 2001 年出版的《廣州話正音字典》。"
                "二書都是權威性的工具書，其注音皆有參考價值。\n\n"
                '本數碼檔案根據 2004 年第二版的《廣州話正音字典》編成。" '
                '"詹伯慧主編，2004/7 第二版，2007/2 第三次印刷" '
                '"https://github.com/jyutnet/cantonese-books-data/tree/master/2004_%E5%BB%A3%E5%B7%9E%E8%A9%B1%E6%AD%A3%E9%9F%B3%E5%AD%97%E5%85%B8" "" "words"'
            )
        )
        sys.exit(1)

    cc_cedict.load()

    words = defaultdict(set)
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
    parse_file(sys.argv[2], words)
    write(sys.argv[1], source, words)
