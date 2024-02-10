import hanzidentifier
import opencc
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
import pinyin_jyutping_sentence

from database import database, objects

import collections
import sqlite3
import sys

simplified_to_traditional_converter = opencc.OpenCC("s2hk.json")
traditional_to_simplified_converter = opencc.OpenCC("t2s.json")


def write(chinese_sentences, nonchinese_sentences, links, db_name):
    print("Writing to database file")

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

    # Add sentences to tables
    for key in chinese_sentences:
        sentence = chinese_sentences[key]
        database.insert_chinese_sentence(
            c,
            sentence.traditional,
            sentence.simplified,
            sentence.pinyin,
            sentence.jyutping,
            sentence.language,
            sentence.id,
        )

    for key in nonchinese_sentences:
        sentence = nonchinese_sentences[key]
        database.insert_nonchinese_sentence(
            c,
            sentence.sentence,
            sentence.language,
            sentence.id,
        )

    # Add links
    for source_sentence_id in links:
        for target_sentence_id in links[source_sentence_id]:
            direct = links[source_sentence_id][target_sentence_id]
            database.insert_sentence_link(
                c,
                source_sentence_id,
                target_sentence_id,
                1,
                direct,
            )

    db.commit()
    db.close()


# This function checks sentences.
# If it's a source sentence, we create a Chinese Sentence object
# Otherwise, we create a non-Chinese Sentence object
def parse_sentence_file(
    filename,
    source,
    target,
    sentences,
    nonchinese_sentences,
    intermediate_ids,
    enable_jyutping,
    enable_pinyin,
):
    print("Parsing sentence file...")
    with open(filename, "r", encoding="utf8") as f:
        for index, line in enumerate(f):
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()
            lang = split[1]
            sentence_start = line.index("\t", line.index("\t") + 1) + 1
            sentence = line[sentence_start:]

            sentence_id = split[0]

            if lang == source:
                if hanzidentifier.is_simplified(sentence):
                    trad = simplified_to_traditional_converter.convert(sentence)
                    simp = sentence
                else:
                    trad = sentence
                    simp = traditional_to_simplified_converter.convert(sentence)
                pin = ""
                if enable_pinyin:
                    pin = (
                        " ".join(
                            lazy_pinyin(
                                simp,
                                style=Style.TONE3,
                                neutral_tone_with_five=True,
                                v_to_u=True,
                            )
                        )
                        .lower()
                        .replace("ü", "u:")
                    )
                jyut = ""
                if enable_jyutping:
                    jyut = pinyin_jyutping_sentence.jyutping(
                        trad, tone_numbers=True, spaces=True
                    )
                sentence_row = objects.ChineseSentence(
                    sentence_id,
                    trad,
                    simp,
                    pin,
                    jyut,
                    lang,
                )

                sentences[sentence_id] = sentence_row
                continue

            if lang == target:
                sentence = line[sentence_start:].strip()
                sentence_translation = objects.NonChineseSentence(
                    sentence_id, sentence, lang
                )
                nonchinese_sentences[sentence_id] = sentence_translation
                continue

            intermediate_ids.add(sentence_id)


def parse_links_file(
    filename,
    sentences,
    nonchinese_sentences,
    chinese_sentences_filtered,
    nonchinese_sentences_filtered,
    intermediate_ids,
    links,
):
    print("Parsing links file...")

    with open(filename, "r", encoding="utf8") as f:
        # Add direct translations
        print("Parsing direct links...")
        for line in f:
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()
            source_id = split[0]
            target_id = split[1]

            if source_id in sentences and target_id in nonchinese_sentences:
                if source_id not in links:
                    links[source_id] = {}
                links[source_id][target_id] = True
                chinese_sentences_filtered[source_id] = chinese_sentences[source_id]
                nonchinese_sentences_filtered[target_id] = nonchinese_sentences[
                    target_id
                ]

        # Also add translations of translations
        print("Parsing indirect links...")
        first_intermediate = {}
        second_intermediate = {}
        f.seek(0, 0)
        for line in f:
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()
            first_id = split[0]
            second_id = split[1]

            # Find all links between source language and intermediate language
            if first_id in sentences and second_id in intermediate_ids:
                first_intermediate[second_id] = first_id

            # Find all links between intermediate language and target language
            if first_id in intermediate_ids and second_id in nonchinese_sentences:
                second_intermediate[first_id] = second_id

        # Match them up
        for key in first_intermediate:
            if key in second_intermediate:
                source_id = first_intermediate[key]
                target_id = second_intermediate[key]
                if source_id not in links:
                    links[source_id] = {}
                if target_id in links[source_id]:
                    continue
                links[source_id][target_id] = False
                chinese_sentences_filtered[source_id] = chinese_sentences[source_id]
                nonchinese_sentences_filtered[target_id] = nonchinese_sentences[
                    target_id
                ]


if __name__ == "__main__":
    if len(sys.argv) != 16:
        print(
            (
                "Usage: python3 -m tatoeba.parse <database filename> "
                "<Tatoeba sentences file> <Tatoeba links file> "
                "<source language> <target language> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
                "<enable generation of jyutping (enable_jyutping = yes)>"
                "<enable generation of pinyin (enable_pinyin = yes)>"
            )
        )
        print(
            (
                "e.g. python3 -m tatoeba.parse tatoeba/developer/yue-eng.db "
                "tatoeba/data/sentences.csv tatoeba/data/links.csv "
                "yue eng Tatoeba TTB 2018-07-09 "
                '"Tatoeba is a collection of sentences." '
                '"These files are released under CC BY 2.0 FR." '
                '"https://tatoeba.org/eng/downloads" "" ""'
                "enable_jyutping disable_pinyin"
            )
        )
        sys.exit(1)

    cc_cedict.load()

    enable_jyutping = sys.argv[14] == "enable_jyutping"
    enable_pinyin = sys.argv[15] == "enable_pinyin"

    chinese_sentences = {}  # Use this to store all the source sentences
    nonchinese_sentences = {}  # Use this to store all the target sentences
    intermediate_ids = set()  # Use this to store ids of sentences between source/target
    chinese_sentences_filtered = (
        {}
    )  # Store only source sentences that match a target sentence
    nonchinese_sentences_filtered = (
        {}
    )  # Store only target sentences that match a source sentence
    links = {}  # Use this to store all the links between sentences
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
    parse_sentence_file(
        sys.argv[2],
        sys.argv[4],
        sys.argv[5],
        chinese_sentences,
        nonchinese_sentences,
        intermediate_ids,
        enable_jyutping,
        enable_pinyin,
    )
    parse_links_file(
        sys.argv[3],
        chinese_sentences,
        nonchinese_sentences,
        chinese_sentences_filtered,
        nonchinese_sentences_filtered,
        intermediate_ids,
        links,
    )
    write(chinese_sentences_filtered, nonchinese_sentences_filtered, links, sys.argv[1])
