from dragonmapper import transcriptions
import opencc
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin.contrib.tone_convert import to_normal, to_tone3
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

import copy
import json
import logging
import re
import sys
import sqlite3
import unicodedata

JYUTPING_COLLOQUIAL_PRONUNCIATION = re.compile(r".⁻(.)")

PINYIN_TONELESS_SYLLABLE_PRONUNCIATION = re.compile(
    r"(?:.*)→ (.*) \(toneless final syllable variant\)"
)
PINYIN_EXTRA_ANNOTATION_REGEX = re.compile(r"(.*)(?: \(.*\))")
PINYIN_REGEX_0 = re.compile(r"(.*)\[Pinyin]")
PINYIN_ERHUA_REGEX = re.compile(r".\d")

KNOWN_WEIRD_BOPOMOFO = {
    "˙ㄏㄫ": "hng5",
    "˙ㄏㄇ": "hm5",
    "˙ㄇ": "m5",
    "˙ㄎㄧㄡ": "kiu5",
    "˙ㄧㄛ": "yo5",
    "ㄎㄟ": "kei1",
    "ㄎㄟˊ": "kei2",
    "ㄎㄟˇ": "kei3",
    "˙ㄎˋ": "kei4",
    "˙ㄎㄟ": "kei5",
    "ㄛ": "o1",
    "ㄛˊ": "o2",
    "ㄛˇ": "o3",
    "ㄛˋ": "o4",
    "˙ㄛ": "o5",
}

SUPERSCRIPT_EQUIVALENT = str.maketrans("¹²³⁴⁵⁶⁷⁸⁹⁰", "1234567890", "")
HALF_TO_FULL = dict((i, i + 0xFEE0) for i in range(0x21, 0x7F))
PUNCTUATION_TABLE = {}
PUNCTUATION_SET = set()
for i in range(sys.maxunicode):
    if unicodedata.category(chr(i)).startswith("P"):
        PUNCTUATION_TABLE[i] = " " + chr(i) + " "
        PUNCTUATION_SET.add(chr(i))

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
    "Hakka Transliteration Scheme",
    "Lanyin Mandarin",
    "Malaysian Mandarin",
    "Nanjing Mandarin",
    "Northeastern Mandarin",
    "Philippine Mandarin",
    "Singaporean Mandarin",
    "Tianjin Mandarin",
    "Yangzhou Mandarin",
    # Other Sinitic languages or writing systems
    "Bàng-uâ-cê / IPA",
    "Eastern Min",
    "Fangyan",
    "Gan, simp.",
    "Guangdong Romanization",
    "Hainanese",
    "Hakka",
    "Hakka Common Romanization Scheme",
    "Hakka Transliteration Scheme",
    "Hangzhounese",
    "Hokkien",
    "IPA",
    "Jyutping++",
    "Kienning Colloquial Romanized",
    "Meixian Hakka",
    "Min Bei",
    "Min Dong",
    "Nankinese Pinyin",
    "Nanning Pinghua",
    "Northern Wu",
    "Pha̍k-fa-sṳ",
    "Pe̍h-ōe-jī",
    "Pouseng Ping'ing",
    "Shanghainese",
    "Sichuanese",
    "Sichuanese Pinyin",
    "Sino-Korean",
    "Sino-Vietnamese",
    "Sixian Hakka",
    "Suzhounese",
    "Taishanese",
    "Taiwanese Hakka Romanization System",
    "Teochow",
    "Teochew",
    "Waxiang",
    "Wenzhounese",
    "Wugniu",
    "Wiktionary",
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

    example_id = database.insert_example(
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
            example_id = database.get_example_id(c, trad, simp, pin, jyut, lang)
            if example_id == -1:  # Something went wrong if example_id is still -1
                return 0
    else:
        examples_inserted += 1

    database.insert_definition_example_link(c, definition_id, example_id)

    for translation in example[1:]:
        sentence = translation.content if translation.content else "X"
        lang = translation.lang

        # Check if translation already exists before trying to insert
        # Insert a translation only if the translation doesn't already exist in the database
        translation_id = database.get_translation_id(c, sentence, lang)

        if translation_id == -1:
            translation_id = starting_example_id + examples_inserted
            database.insert_translation(c, sentence, lang, translation_id)
            examples_inserted += 1

        # Then, link the translation to the example only if the link doesn't already exist
        link_id = database.get_example_link(c, example_id, translation_id)

        if link_id == -1:
            database.insert_example_link(c, example_id, translation_id, 1, True)

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
    example_id = 7000000000

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

        # print(entry, entry.definitions)
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
    processed = JYUTPING_COLLOQUIAL_PRONUNCIATION.sub(r"\1", processed)
    return processed


def process_mandarin_romanization(romanization):
    if romanization == "":
        return romanization

    romanization = romanization.replace("'", "")

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
            processed_example_romanization_list.append(grouping.translate(HALF_TO_FULL))
        else:
            try:
                syllables = transcriptions.to_pinyin(
                    transcriptions.to_zhuyin(grouping.lower())
                ).split(" ")
            except ValueError:
                logging.debug(
                    f'Parsing romanization failed for syllable(s) "{grouping}", romanization is "{romanization}"'
                )

                if grouping in KNOWN_WEIRD_BOPOMOFO:
                    grouping = KNOWN_WEIRD_BOPOMOFO[grouping]
                elif grouping[0] == "˙":
                    # Remove the neutral tone because Dragonmapper cannot handle it
                    # Dragonmapper will add the first tone since there is no tone indicated by Bopomofo symbols
                    grouping = transcriptions.to_pinyin(grouping[1:].lower())
                    # Remove the first tone added by Dragonmapper using pypinyin
                    grouping = to_normal(grouping)
                    grouping += "5"
                else:
                    grouping = grouping.translate(HALF_TO_FULL)

                processed_example_romanization_list.append(grouping)
                continue

            for syllable in syllables:
                converted_syllable = to_tone3(
                    syllable, v_to_u=True, neutral_tone_with_five=True
                )
                converted_syllable = converted_syllable.replace("ü", "u:")

                # Erhua is not well supported in Jyut Dictionary, so convert "r" to "er"
                if PINYIN_ERHUA_REGEX.match(syllable):
                    converted_syllable = "e" + converted_syllable

                processed_example_romanization_list.append(converted_syllable)

    return " ".join(processed_example_romanization_list)


def parse_file(filename, words):
    for line in open(filename):
        if not (len(words) % 500):
            logging.info(f"Word #{len(words)} processed")

        data = json.loads(line)

        if "lang_code" not in data or data["lang_code"] != "zh":
            continue

        trad = data["word"]
        simp = traditional_to_simplified_converter.convert(trad)

        jyutping_list = []
        pinyin_list = []
        bopomofo_to_pinyin_list = []
        mainland_taiwain_pinyin_list = []
        if "sounds" in data:
            for pron in data["sounds"]:
                if "tags" not in pron:
                    continue

                if pron["tags"] == ["Mandarin", "Pinyin", "Standard-Chinese"] or pron[
                    "tags"
                ] == [
                    "Mandarin",
                    "Pinyin",
                    "Standard-Chinese",
                    "toneless-final-syllable-variant",
                ]:
                    if "zh_pron" in pron:
                        pin = pron["zh_pron"]
                        pin_match = PINYIN_EXTRA_ANNOTATION_REGEX.match(pin)
                        if pin_match:
                            pin = pin_match.group(1)
                        pinyin_list.append(process_mandarin_romanization(pin))
                elif pron["tags"] == [
                    "Mandarin",
                    "Bopomofo",
                    "Standard-Chinese",
                ] or pron["tags"] == [
                    "Mandarin",
                    "Bopomofo",
                    "Standard-Chinese",
                    "toneless-final-syllable-variant",
                ]:
                    if "zh_pron" in pron:
                        bopomofo = pron["zh_pron"]
                        bopomofo_to_pinyin_list.append(
                            process_mandarin_romanization(bopomofo)
                        )
                elif (
                    pron["tags"]
                    == [
                        "Mainland-China",
                        "Mandarin",
                        "Standard-Chinese",
                        "Bopomofo",
                    ]
                    or pron["tags"]
                    == [
                        "Mainland-China",
                        "Mandarin",
                        "Standard-Chinese",
                        "Bopomofo",
                        "toneless-final-syllable-variant",
                    ]
                    or pron["tags"]
                    == [
                        "Mandarin",
                        "Standard-Chinese",
                        "Taiwan",
                        "Bopomofo",
                    ]
                    or pron["tags"]
                    == [
                        "Mandarin",
                        "Standard-Chinese",
                        "Taiwan",
                        "Bopomofo",
                        "toneless-final-syllable-variant",
                    ]
                ):
                    if "zh_pron" in pron:
                        bopomofo = pron["zh_pron"]
                        mainland_taiwain_pinyin_list.append(
                            process_mandarin_romanization(bopomofo)
                        )
                elif pron["tags"] == ["Cantonese", "Guangzhou", "Jyutping"]:
                    if "zh_pron" in pron:
                        jyutping_list.append(
                            parse_cantonese_romanization(pron["zh_pron"])
                        )

        if len(mainland_taiwain_pinyin_list) > len(pinyin_list):
            # There is a variance in pronunciation between Mainland China and Taiwan
            pinyin_list = mainland_taiwain_pinyin_list
        elif not pinyin_list or len(bopomofo_to_pinyin_list) != len(pinyin_list):
            # Either there is no Pinyin (due to Wiktionary parsing error),
            # or the Pinyin list is somehow longer than the Bopomofo list (which
            # should never happen). In these cases, the Bopomofo is usually more
            # reliable.
            pinyin_list = bopomofo_to_pinyin_list

        if not jyutping_list:
            jyutping_list = [
                pinyin_jyutping_sentence.jyutping(trad, tone_numbers=True, spaces=True)
            ]
        if not pinyin_list:
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
            pinyin_list = [pin]

        freq = zipf_frequency(trad, "zh")

        entry = objects.Entry(
            trad=trad,
            simp=simp,
            jyut=jyutping_list[0],
            pin=pinyin_list[0],
            freq=freq,
            defs=set(),
        )

        words.append(entry)

        pos = data["pos"] if "pos" in data else ""

        for sense in data["senses"]:
            if "glosses" not in sense:
                continue
            gloss = sense["glosses"][0].split("\n")[0]

            synonym_set = set()
            if "synonyms" in sense:
                for synonym in sense["synonyms"]:
                    if "word" in synonym and "／" not in synonym["word"]:
                        synonym_set.add(synonym["word"].replace(" (", ""))

            antonym_set = set()
            if "antonyms" in sense:
                for antonym in sense["antonyms"]:
                    if "word" in antonym and "／" not in antonym["word"]:
                        antonym_set.add(antonym["word"].replace(" (", ""))

            definition_text = (
                gloss
                + ("\n(syn.) " + ", ".join(list(synonym_set)) if synonym_set else "")
                + ("\n(ant.) " + ", ".join(list(antonym_set)) if antonym_set else "")
            )

            definition = objects.Definition(
                definition=definition_text,
                label=", ".join([pos] + sense["tags"]) if "tags" in sense else pos,
                examples=[],
            )

            if not entry.append_to_defs(definition):
                # This definition is not unique. Skip adding it.
                # This occurs because glosses are often repeated in Wiktionary entries
                # for some reason.
                continue

            if "examples" not in sense:
                continue

            for example in sense["examples"]:
                found_example = False
                if (
                    "roman" in example
                    and transcriptions.is_pinyin_compatible(example["roman"])
                    and "tags" in example
                    and "Traditional-Chinese" in example["tags"]
                    and not any(ignored in example["tags"] for ignored in IGNORED_TEXT)
                ):
                    found_example = True
                    example_text = example["text"].split("／")[0]
                    example_romanization = process_mandarin_romanization(
                        example["roman"]
                    )
                    example_translation = (
                        example["english"] if "english" in example else "x"
                    )
                    lang = "cmn"
                elif (
                    "roman" in example
                    and "tags" in example
                    and "Jyutping" in example["tags"]
                    and "Traditional-Chinese" in example["tags"]
                    and not any(ignored in example["tags"] for ignored in IGNORED_TEXT)
                ):
                    found_example = True
                    example_text = example["text"]
                    example_romanization = parse_cantonese_romanization(
                        example["roman"]
                    )
                    example_translation = (
                        example["english"] if "english" in example else "x"
                    )
                    lang = "yue"

                if found_example:
                    definition.examples.append([])
                    definition.examples[-1].append(
                        objects.ExampleTuple(lang, example_romanization, example_text)
                    )
                    definition.examples[-1].append(
                        objects.Example(lang="eng", content=example_translation)
                    )
                else:
                    if (
                        "tags" in example
                        and "Traditional-Chinese" in example["tags"]
                        and not any(
                            ignored in example["tags"] for ignored in IGNORED_TEXT
                        )
                    ):
                        if "raw_tags" in example and not any(
                            ignored in example["raw_tags"] for ignored in IGNORED_TEXT
                        ):
                            logging.warning(f"no match found for example: {example}")

        for jyutping in jyutping_list[1:]:
            new_entry = copy.deepcopy(entry)
            new_entry.add_jyutping(jyutping)
            words.append(new_entry)

        for pinyin in pinyin_list[1:]:
            new_entry = copy.deepcopy(entry)
            new_entry.add_pinyin(pinyin)
            words.append(new_entry)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 -m wiktionary.parse <database filename> "
                "<dict-wk.json filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m wiktionary.parse wiktionary/developer/wiktionary.db wiktionary/data/dict-wk.json "
                '"Wiktionary" WT 2023-02-16 '
                '"Wiktionary is a collaborative project to produce a free-content multilingual dictionary. '
                'It aims to describe all words of all languages using definitions and descriptions in English." '
                '"Text is available under the Creative Commons Attribution-ShareAlike License; additional terms may apply." '
                '"https://en.wiktionary.org/wiki/Wiktionary:Main_Page" "" "words,examples"'
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

    cc_cedict.load()

    logging.getLogger().setLevel(logging.INFO)

    words = []
    parse_file(sys.argv[2], words)
    write(sys.argv[1], source, words)
