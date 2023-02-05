from dragonmapper import transcriptions
import jieba
import opencc
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict, namedtuple
import csv
import logging
import sqlite3
import sys

converter = opencc.OpenCC("tw2s.json")


def read_csv(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            yield row


def insert_words(c, words):
    for entry in words:
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


def parse_same_meaning_file(filename, words):
    for line in read_csv(filename):
        if len(line) != 17 or line[0] == "總分類":
            continue

        terms = defaultdict(set)

        for index in (4, 5, 6):
            if line[index]:
                terms["臺"].add(line[index])

        for index in (7, 8, 9):
            if line[index]:
                terms["陸"].add(line[index])

        for index in (10, 11, 12):
            if line[index]:
                terms["香"].add(line[index])

        for index in (13, 14, 15):
            if line[index]:
                terms["澳"].add(line[index])

        explanation = None
        if line[16]:
            explanation = objects.DefinitionTuple(
                "​".join(jieba.cut(line[16])), "差異說明", []
            )

        for location in terms:
            for term in terms[location]:
                trad = term
                simp = converter.convert(trad)
                if term == line[4] and line[2]:
                    # Use the provided pinyin, which always corresponds at least to the first Taiwan term
                    pin = transcriptions.zhuyin_to_pinyin(
                        line[2].replace("　", " "), accented=False
                    )
                else:
                    pin = lazy_pinyin(
                        simp,
                        style=Style.TONE3,
                        neutral_tone_with_five=True,
                    )
                    pin = " ".join(pin).lower()
                    pin = pin.strip().replace("v", "u:")
                jyut = pinyin_jyutping_sentence.jyutping(
                    trad, tone_numbers=True, spaces=True
                )
                freq = zipf_frequency(trad, "zh")

                defs = terms.keys()
                defs = map(
                    lambda x: objects.DefinitionTuple(
                        "、".join(terms[x]), line[1] + "：" + x, []
                    ),
                    defs,
                )
                defs = list(defs)

                if explanation:
                    defs.append(explanation)

                entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=defs)
                words.add(entry)


def parse_same_word_file(filename, words):
    for line in read_csv(filename):
        if len(line) != 2 or line[0] == "詞彙":
            continue

        trad = line[0]
        simp = converter.convert(trad)
        pin = lazy_pinyin(
            simp,
            style=Style.TONE3,
            neutral_tone_with_five=True,
        )
        pin = " ".join(pin).lower()
        pin = pin.strip().replace("v", "u:")
        jyut = pinyin_jyutping_sentence.jyutping(trad, tone_numbers=True, spaces=True)
        freq = zipf_frequency(trad, "zh")
        defs = [objects.DefinitionTuple("​".join(jieba.cut(line[1])), "臺陸用法和差異", [])]

        entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=defs)
        words.add(entry)


if __name__ == "__main__":
    if len(sys.argv) != 12:
        print(
            (
                "Usage: python3 -m two_shores_three_places.parse <database filename> "
                "<兩岸三地生活差異詞語彙編-同實異名.csv> "
                "<兩岸三地生活差異詞語彙編-同名異實.csv> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m two_shores_three_places.parse lnsd.db "
                "./two_shores_three_places/data/兩岸三地生活差異詞語彙編-同實異名.csv "
                "./two_shores_three_places/data/兩岸三地生活差異詞語彙編-同名異實.csv "
                '"兩岸三地生活差異詞語彙編—Two Shores Three Places" LNSD 2019-06-23 '
                '"The Two Shores Three Places dictionary compares usage of common terms in Mainland China, Taiwan, and Hong Kong." '
                '"《兩岸三地生活差異詞語彙編》由中華文化總會以 CC BY-NC-ND 4.0 之条款下提供。" '
                '"http://www.chinese-linguipedia.org/" "" "words"'
            )
        )
        sys.exit(1)

    cc_cedict.load()

    words = set()
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
    parse_same_meaning_file(sys.argv[2], words)
    parse_same_word_file(sys.argv[3], words)
    write(sys.argv[1], source, words)
