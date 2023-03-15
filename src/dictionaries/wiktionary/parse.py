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

CANTONESE_REGEX_0 = re.compile(
    r"(.*)\s\[Cantonese.*?\]\s―\s(.*?)\s\[Jyutping\]\s―\s(.*)"
)
CANTONESE_REGEX_1 = re.compile(
    r"((?:.|\n)*) \[(?:.* )?Cantonese.*?\](?:From:\s.*?\n)?((?:.|\n)*) \[Jyutping\]"
)
CANTONESE_REGEX_2 = re.compile(r"(.*?) \[Cantonese, trad. and simp.\](From:\s.*?\\n)?")
CANTONESE_REGEX_3 = re.compile(r"(.*)\s\[Cantonese.*?\]")

JYUTPING_COLLOQUIAL_PRONUNCIATION = re.compile(r".⁻(.)")
JYUTPING_REGEX_0 = re.compile(r"(.*)\[Jyutping\]")

MANDARIN_REGEX_0 = re.compile(
    r"(.*?) \[MSC, trad\.\]\n(?:.*?) \[MSC, simp.\](.*) \[Pinyin\]"
)
MANDARIN_REGEX_1 = re.compile(
    r"((?:.|\n)*) \[MSC, simp\.\](?:From:(?:.*|\n*)\n)?((?:.|\n)*) \[Pinyin\]"
)
MANDARIN_REGEX_2 = re.compile(
    r"((?:.|\n)*) \[MSC, trad\.\](?:From:(?:.*|\n*)\n)?((?:.|\n)*) \[Pinyin\]"
)
MANDARIN_REGEX_3 = re.compile(
    r"(.*?) \[MSC, trad\. and simp\.\](?:From:.*\n?.*\n?)?(.*?) \[Pinyin\]"
)
MANDARIN_REGEX_4 = re.compile(r"(.*?) \[MSC, trad\. and simp\.\](.*)")
MANDARIN_REGEX_5 = re.compile(
    r"(.*?) \[Beijing Mandarin, trad\.\].* \[Beijing Mandarin, simp.\](?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_6 = re.compile(
    r"(.*?) \[Beijing Mandarin, simp\.\](?:From: .*?\n)?((.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_7 = re.compile(
    r"(.*?) \[Beijing Mandarin, trad\.\](?:From: .*?\n)?((.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_8 = re.compile(
    r"(.*?) \[Beijing Mandarin, trad\. and simp.\](?:From: .*?\n)?(?:(.*)(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_9 = re.compile(
    r"(.*?) \[Taiwanese Mandarin, simp\.\](From: .*?\n)?(?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_10 = re.compile(
    r"(.*?) \[Taiwanese Mandarin, trad\.\](From: .*?\n)?(?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_11 = re.compile(
    r"(.*?) \[Taiwanese Mandarin, trad\. and simp\.\](?:From:(?:.*|\n*)(?:\n|” ))(.*?) \[Pinyin\]"
)
MANDARIN_REGEX_12 = re.compile(r"(.*)\s―\s(.*)\s―\s(.*)")
MANDARIN_REGEX_13 = re.compile(r"(.*?)\s―\s(.*)")

PINYIN_TONELESS_SYLLABLE_PRONUNCIATION = re.compile(
    r"(?:.*)→ (.*) \(toneless final syllable variant\)"
)
PINYIN_EXTRA_ANNOTATION_REGEX = re.compile(r"(.*)(?: \(.*\))")
PINYIN_REGEX_0 = re.compile(r"(.*)\[Pinyin]")

SUPERSCRIPT_EQUIVALENT = str.maketrans("¹²³⁴⁵⁶⁷⁸⁹⁰", "1234567890", "")
HALF_TO_FULL = dict((i, i + 0xFEE0) for i in range(0x21, 0x7F))
PUNCTUATION_TABLE = {}
PUNCTUATION_SET = set()
for i in range(sys.maxunicode):
    if unicodedata.category(chr(i)).startswith("P"):
        PUNCTUATION_TABLE[i] = " " + chr(i) + " "
        PUNCTUATION_SET.add(chr(i))

LANGUAGE_INDICATOR = (
    "Cantonese",
    "MSC",
    "Mandarin",
)

IGNORED_TEXT = (
    # We should eventually parse Literary and Classical Chinese, but ignore for now
    "Literary Chinese",
    "Classical Chinese",
    "Written Vernacular Chinese",
    # Variations of Cantonese
    "Literary Cantonese",
    # Variations of Mandarin
    "dialectal Mandarin",
    "Central Plains Mandarin",
    "Guilin Mandarin",
    "Lanyin Mandarin",
    "Malaysian Mandarin",
    "Nanjing Mandarin",
    "Northeastern Mandarin",
    "Philippine Mandarin",
    "Singaporean Mandarin",
    "Tianjin Mandarin",
    "Yangzhou Mandarin",
    # Other Sinitic languages or writing systems
    "Gan, simp.",
    "Fangyan",
    "Hainanese",
    "Hakka",
    "Hokkien",
    "Min Bei",
    "Min Dong",
    "Pe̍h-ōe-jī",
    "Shanghainese",
    "Sichuanese",
    "Sino-Korean",
    "Sino-Vietnamese",
    "Suzhounese",
    "Taishanese",
    "Teochow",
    "Teochew",
    # Wiktionary stuff
    "alt. forms:",
    "Citations:man",
    "See also:",
    "zh-co",
    "zh-x",
)

traditional_to_simplified_converter = opencc.OpenCC("hk2s.json")
simplified_to_traditional_converter = opencc.OpenCC("s2hk.json")


def insert_example(c, definition_id, starting_example_id, example):
    examples_inserted = 0

    trad = example[0].content
    simp = traditional_to_simplified_converter.convert(trad)
    jyut = example[0].pron if example[0].lang == "yue" else ""
    pin = (
        example[0].pron if example[0].lang == "cmn" or example[0].lang == "zho" else ""
    )
    lang = example[0].lang

    example_id = database.insert_chinese_sentence(
        c, trad, simp, pin, jyut, lang, starting_example_id
    )

    # Check if example insertion was successful
    if example_id == -1:
        if trad == "X" or trad == "x":
            # Ignore examples that are just 'x'
            return 0
        else:
            # If insertion failed, it's probably because the example already exists
            # Get its rowid, so we can link it to this definition
            example_id = database.get_chinese_sentence_id(
                c, trad, simp, pin, jyut, lang
            )
            if example_id == -1:  # Something went wrong if example_id is still -1
                return 0
    else:
        examples_inserted += 1

    database.insert_definition_chinese_sentence_link(c, definition_id, example_id)

    for translation in example[1:]:
        sentence = translation.content
        lang = translation.lang

        # Check if translation already exists before trying to insert
        # Insert a translation only if the translation doesn't already exist in the database
        translation_id = database.get_nonchinese_sentence_id(c, sentence, lang)

        if translation_id == -1:
            translation_id = starting_example_id + examples_inserted
            database.insert_nonchinese_sentence(c, sentence, lang, translation_id)
            examples_inserted += 1

        # Then, link the translation to the example only if the link doesn't already exist
        link_id = database.get_sentence_link(c, example_id, translation_id)

        if link_id == -1:
            database.insert_sentence_link(c, example_id, translation_id, 1, True)

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


def parse_cantonese_romanization(romanization):
    if romanization == "":
        return romanization

    processed = romanization.translate(SUPERSCRIPT_EQUIVALENT)
    processed = JYUTPING_COLLOQUIAL_PRONUNCIATION.sub("\1", processed)
    return processed

def process_mandarin_romanization(romanization):
    if romanization == "":
        return romanization

    example_romanization_list = (
        romanization.translate(PUNCTUATION_TABLE).strip().split(" ")
    )
    processed_example_romanization_list = []
    for grouping in example_romanization_list:
        if grouping == "":
            continue
        elif (
            any(punct in grouping for punct in PUNCTUATION_SET) or grouping.isnumeric()
        ):
            processed_example_romanization_list.append(grouping)
        else:
            try:
                syllables = transcriptions.to_pinyin(
                    transcriptions.to_zhuyin(grouping.lower())
                ).split(" ")
            except ValueError:
                logging.debug(
                    f'Parsing romanization failed for syllable(s) "{grouping}", romanization is "{romanization}"'
                )
                processed_example_romanization_list.append(
                    grouping.translate(HALF_TO_FULL)
                )
                continue

            for syllable in syllables:
                converted_syllable = to_tone3(
                    syllable, v_to_u=True, neutral_tone_with_five=True
                )
                converted_syllable.replace("ü", "u:")
                processed_example_romanization_list.append(converted_syllable)

    return " ".join(processed_example_romanization_list)


def parse_file(filename, words):
    for line in open(filename):
        if not (len(words) % 500):
            logging.info(f"Word #{len(words)} processed")

        data = json.loads(line)

        trad = data["word"]
        simp = traditional_to_simplified_converter.convert(trad)

        found_pin = found_jyut = False
        if "sounds" in data:
            for pron in data["sounds"]:
                if "tags" not in pron:
                    continue

                if not found_pin and (
                    pron["tags"] == ["Mandarin", "Pinyin", "standard"]
                    or pron["tags"] == ["Mandarin", "standard"]
                ):
                    pin = pron["zh-pron"]
                    pin_match = PINYIN_TONELESS_SYLLABLE_PRONUNCIATION.match(pin)
                    if pin_match:
                        pin = pin_match.group(1)
                    pin_match = PINYIN_EXTRA_ANNOTATION_REGEX.match(pin)
                    if pin_match:
                        pin = pin_match.group(1)
                    pin = process_mandarin_romanization(pin)
                    found_pin = True
                elif not found_jyut and (
                    pron["tags"] == ["Cantonese", "Guangzhou", "Jyutping"]
                ):
                    jyut = parse_cantonese_romanization(pron["zh-pron"])

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

        pos = data["pos"] if "pos" in data else ""

        for sense in data["senses"]:
            if "glosses" not in sense:
                continue

            definition = objects.DefinitionTuple(
                sense["glosses"][0].split("\n")[0],
                ", ".join([pos] + sense["tags"]) if "tags" in sense else pos,
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
                    example_romanization = process_mandarin_romanization(
                        example["roman"]
                    )
                    example_translation = (
                        example["english"] if "english" in example else ""
                    )
                    lang = "cmn"
                elif "text" in example:
                    if all(
                        language not in example["text"]
                        for language in LANGUAGE_INDICATOR
                    ):
                        if "ref" not in example:
                            continue
                        elif all(
                            language not in example["ref"]
                            for language in LANGUAGE_INDICATOR
                        ):
                            continue
                    if any(ignored in example["text"] for ignored in IGNORED_TEXT):
                        continue
                    elif "ref" in example and any(
                        ignored in example["ref"] for ignored in IGNORED_TEXT
                    ):
                        continue

                    # Generally, the Chinese sentence is in example["text"]
                    # But sometimes, it is in example["ref"]
                    if not found_example:
                        match = CANTONESE_REGEX_0.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1).split("／")[0]
                            example_romanization = parse_cantonese_romanization(match.group(2))
                            example_translation = match.group(3)
                            lang = "yue"

                    if not found_example:
                        match = CANTONESE_REGEX_1.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = parse_cantonese_romanization(match.group(2))
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "yue"
                        elif "ref" in example:
                            match = CANTONESE_REGEX_1.match(example["ref"])
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = parse_cantonese_romanization(match.group(2))
                                example_translation = (
                                    example["text"] if "text" in example else ""
                                )
                                lang = "yue"

                    if not found_example:
                        match = CANTONESE_REGEX_2.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = ""
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "yue"
                        elif "ref" in example:
                            match = CANTONESE_REGEX_2.match(example["ref"])
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                jyutping_match = JYUTPING_REGEX_0.match(example["text"])
                                example_romanization = (
                                    jyutping_match.group(1) if jyutping_match else ""
                                )
                                example_romanization = example_romanization.translate(
                                    SUPERSCRIPT_EQUIVALENT
                                )
                                example_translation = (
                                    example["english"] if "english" in example else ""
                                )
                                lang = "yue"

                    if not found_example:
                        match = CANTONESE_REGEX_3.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = ""
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "yue"

                    if not found_example:
                        match = MANDARIN_REGEX_0.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_1.match(example["text"])
                        if match:
                            found_example = True
                            example_text = simplified_to_traditional_converter.convert(
                                match.group(1)
                            )
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_2.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_3.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_2.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = match.group(2)
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["english"] if "english" in example else ""
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_4.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_4.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = example["ref"]
                                pinyin_match = PINYIN_REGEX_0.match(
                                    example_romanization
                                )
                                example_romanization = (
                                    pinyin_match.group(1) if pinyin_match else ""
                                )
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["english"] if "english" in example else ""
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_5.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_6.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_7.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_8.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_8.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = (
                                    match.group(2) if match.group(2) else ""
                                )
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["english"] if "english" in example else ""
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_9.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_10.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_11.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_11.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = (
                                    match.group(2) if match.group(2) else ""
                                )
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["text"] if "text" in example else ""
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_12.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = match.group(3)
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_13.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                ""  # This regex does not have pronunciation
                            )
                            lang = "cmn"

                if found_example:
                    definition.examples.append([])
                    definition.examples[-1].append(
                        objects.ExampleTuple(lang, example_romanization, example_text)
                    )
                    definition.examples[-1].append(
                        objects.Example(lang="eng", content=example_translation)
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
