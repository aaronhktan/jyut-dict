from dragonmapper import transcriptions
import opencc
from pypinyin.contrib.tone_convert import to_tone3
from wordfreq import zipf_frequency

from database import database, objects

import json
import logging
import re
import sys
import sqlite3
import unicodedata

# Useful test words:

CANTONESE_REGEX_0 = re.compile(r"(.*) \[Cantonese.*?\] ― (.*?)\s\[Jyutping\] ― (.*)")
CANTONESE_REGEX_1 = re.compile(r"(.*?)\[Cantonese.*?\](.*?)\s\[Jyutping\]")

SUPERSCRIPT_EQUIVALENT = str.maketrans("¹²³⁴⁵⁶⁷⁸⁹⁰", "1234567890", "")

PUNCTUATION_TABLE = {}
PUNCTUATION_SET = set()
for i in range(sys.maxunicode):
    if unicodedata.category(chr(i)).startswith("P"):
        PUNCTUATION_TABLE[i] = " " + chr(i) + " "
        PUNCTUATION_SET.add(chr(i))

traditional_to_simplified_converter = opencc.OpenCC("hk2s.json")
simplified_to_traditional_converter = opencc.OpenCC("s2hk.json")


def insert_example(c, definition_id, starting_example_id, example):
    examples_inserted = 0

    trad = example.content
    simp = traditional_to_simplified_converter.convert(trad)
    jyut = ""
    pin = example.pron
    lang = example.lang

    example_id = database.insert_chinese_sentence(
        c, trad, simp, pin, jyut, lang, starting_example_id
    )

    # Check if example insertion was successful
    if example_id == -1:
        # If insertion was not successful, it might be because the example already exists in the database
        # Attempt to get the id of the row that contains that example
        example_id = database.get_chinese_sentence_id(c, trad, simp, pin, jyut, lang)

        # Something has gone wrong if unable to insert and unable to retrieve the id - bail out here
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
    #   - 5000000000-5999999999: ABC Chinese-English Dictionary
    #   - 6000000000-6999999999: ABC Cantonese-English Dictionary
    #   - 7000000000-7999999999: Wiktionary
    example_id = 4000000000

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

            for example in definition.examples:
                examples_inserted = insert_example(
                    c, definition_id, example_id, example
                )
                example_id += examples_inserted


def write(db_name, source, entries):
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


def parse_file(filename, words):
    for line in open(filename):
        data = json.loads(line)

        trad = data["word"]
        simp = traditional_to_simplified_converter.convert(trad)

        found_pin = found_jyut = False
        for pron in data["sounds"]:
            if pron["tags"] == ["Mandarin", "Pinyin", "standard"]:
                pin = pron["zh-pron"]
                found_pin = True
            elif pron["tags"] == ["Cantonese", "Guangzhou", "Jyutping"]:
                jyut = pron["zh-pron"]
                found_jyut = True
            if found_pin and found_jyut:
                break

        freq = zipf_frequency(trad, "zh")

        entry = objects.Entry(
            trad=trad,
            simp=simp,
            jyut=jyut if found_jyut else "",
            pin=pin if found_pin else "",
            freq=freq,
        )

        words.append(entry)

        for sense in data["senses"]:
            if "glosses" not in sense:
                continue

            definition = objects.DefinitionTuple(
                sense["glosses"][0],
                ", ".join(sense["tags"]) if "tags" in sense else "",
                [],
            )
            entry.append_to_defs(definition)

            if "examples" not in sense:
                continue

            for example in sense["examples"]:
                found_example = False
                if (
                    "roman" in example
                    and "english" in example
                    and transcriptions.is_pinyin_compatible(example["roman"])
                ):
                    found_example = True
                    example_text = example["text"].split("／")[0]

                    example_romanization_list = example["roman"].translate(PUNCTUATION_TABLE).strip().split(" ")
                    processed_example_romanization_list = []
                    for grouping in example_romanization_list:
                        if any(punct in grouping for punct in PUNCTUATION_SET):
                            processed_example_romanization_list.append(grouping)
                        else:
                            syllables = transcriptions.to_pinyin(transcriptions.to_zhuyin(grouping)).split(" ")
                            for syllable in syllables:
                                converted_syllable = to_tone3(
                                    syllable, v_to_u=True, neutral_tone_with_five=True
                                )
                                converted_syllable.replace("ü", "u:")
                                processed_example_romanization_list.append(converted_syllable)
                    example_romanization = " ".join(processed_example_romanization_list)

                    example_translation = example["english"]
                    lang = "cmn"
                elif "text" in example:
                    match = CANTONESE_REGEX_0.match(example["text"])
                    if match:
                        found_example = True
                        example_text = match.group(1).split("／")[0]
                        example_romanization = match.group(2).translate(
                            SUPERSCRIPT_EQUIVALENT
                        )
                        example_translation = match.group(3)
                        lang = "yue"

                    if not found_example:
                        match = CANTONESE_REGEX_1.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2).translate(
                                SUPERSCRIPT_EQUIVALENT
                            )
                            example_translation = example["english"]
                            lang = "yue"

                if found_example:
                    definition.examples.append(
                        objects.ExampleTuple(lang, example_romanization, example_text)
                    )
                else:
                    print("no match found for example", example)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<dict-wk.json filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m wiktionary.parse wiktionary.db ./dict-wk.json "
                '"Wiktionary" WT 2023-02-16 '
                '"Wiktionary, a collaborative project to produce a free-content multilingual dictionary.'
                'It aims to describe all words of all languages using definitions and descriptions in English." '
                '"Text is available under the Creative Commons Attribution-ShareAlike License; additional terms may apply." '
                '"https://en.wiktionary.org/wiki/Wiktionary:Main_Page" "" "words,sentences"'
            )
        )
        sys.exit(1)

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

    logging.getLogger().setLevel(logging.INFO)

    words = []
    parse_file(sys.argv[2], words)
    write(sys.argv[1], source, words)
