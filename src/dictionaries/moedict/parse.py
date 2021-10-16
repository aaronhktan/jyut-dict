from hanziconv import HanziConv
import hanzidentifier
import jieba
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import namedtuple
import json
import logging
import re
import sqlite3
import sys

# Useful test words:
#   - 重: multiple heteronyms, labels for definitions
#   - 万: one heteronym has pinyin (should be included), other does not (should be excluded)

DefinitionTuple = namedtuple("Definition", ["definition", "label", "examples"])
ExampleTuple = namedtuple("ExampleTuple", ["lang", "pron", "content"])

PINYIN_CORRESPONDANCE_1 = {
    "ā": "a1",
    "á": "a2",
    "ǎ": "a3",
    "à": "a4",
    "a": "a5",
    "ō": "o1",
    "ó": "o2",
    "ǒ": "o3",
    "ò": "o4",
    "o": "o5",
    "ē": "e1",
    "é": "e2",
    "ě": "e3",
    "è": "e4",
    "e": "e5",
    "ī": "i1",
    "í": "i2",
    "ǐ": "i3",
    "ì": "i4",
    "i": "i5",
    "ū": "u1",
    "ú": "u2",
    "ǔ": "u3",
    "ù": "u4",
    "u": "u5",
    "ǖ": "u:1",
    "ǘ": "u:2",
    "ǚ": "u:3",
    "ǜ": "u:4",
    "ü": "u:5",
}
PINYIN_CORRESPONDANCE_2 = {
    "āi": "ai1",
    "ái": "ai2",
    "ǎi": "ai3",
    "ài": "ai4",
    "ai": "ai5",
    "ēi": "ei1",
    "éi": "ei2",
    "ěi": "ei3",
    "èi": "ei4",
    "ei": "ei5",
    "uī": "ui1",
    "uí": "ui2",
    "uǐ": "ui3",
    "uì": "ui4",
    "ui": "ui5",
    "āo": "ao1",
    "áo": "ao2",
    "ǎo": "ao3",
    "ào": "ao4",
    "ao": "ao5",
    "ōu": "ou1",
    "óu": "ou2",
    "ǒu": "ou3",
    "òu": "ou4",
    "ou": "ou5",
    "iū": "iu1",
    "iú": "iu2",
    "iǔ": "iu3",
    "iù": "iu4",
    "iu": "iu5",
    "iē": "ie1",
    "ié": "ie2",
    "iě": "ie3",
    "iè": "ie4",
    "ie": "ie5",
    "üē": "u:e1",
    "üé": "u:e2",
    "üě": "u:e3",
    "üè": "u:e4",
    "üe": "u:e5",
    "ēr": "er1",
    "ér": "er2",
    "ěr": "er3",
    "èr": "er4",
    "er": "er5",
    "ān": "an1",
    "án": "an2",
    "ǎn": "an3",
    "àn": "an4",
    "an": "an5",
    "ēn": "en1",
    "én": "en2",
    "ěn": "en3",
    "èn": "en4",
    "en": "en5",
    "īn": "in1",
    "ín": "in2",
    "ǐn": "in3",
    "ìn": "in4",
    "in": "in5",
    "ūn": "un1",
    "ún": "un2",
    "ǔn": "un3",
    "ùn": "un4",
    "un": "un5",
    "ǖn": "u:n1",
    "ǘn": "u:n2",
    "ǚn": "u:n3",
    "ǜn": "u:n4",
    "ün": "u:n5",
    "ār": "a1 er5",
    "ár": "a2 er5",
    "ǎr": "a3 er5",
    "àr": "a4 er5",
    "ar": "a5 er5",
    "ōr": "o1 er5",
    "ór": "o2 er5",
    "ǒr": "o3 er5",
    "òr": "o4 er5",
    "or": "o5 er5",
    "ēr": "e1 er5",
    "ér": "e2 er5",
    "ěr": "e3 er5",
    "èr": "e4 er5",
    "er": "e5 er5",
    "īr": "i1 er5",
    "ír": "i2 er5",
    "ǐr": "i3 er5",
    "ìr": "i4 er5",
    "ir": "i5 er5",
    "ūr": "u1 er5",
    "úr": "u2 er5",
    "ǔr": "u3 er5",
    "ùr": "u4 er5",
    "ur": "u5 er5",
    "ǖr": "u:1 er5",
    "ǘr": "u:2 er5",
    "ǚr": "u:3 er5",
    "ǜr": "u:4 er5",
    "ür": "u:5 er5",
}
PINYIN_CORRESPONDANCE_3 = {
    "āng": "ang1",
    "áng": "ang2",
    "ǎng": "ang3",
    "àng": "ang4",
    "ang": "ang5",
    "ēng": "eng1",
    "éng": "eng2",
    "ěng": "eng3",
    "èng": "eng4",
    "eng": "eng5",
    "īng": "ing1",
    "íng": "ing2",
    "ǐng": "ing3",
    "ìng": "ing4",
    "ing": "ing5",
    "ōng": "ong1",
    "óng": "ong2",
    "ǒng": "ong3",
    "òng": "ong4",
    "ong": "ong5",
    "āir": "ai1 er5",
    "áir": "ai2 er5",
    "ǎir": "ai3 er5",
    "àir": "ai4 er5",
    "air": "ai5 er5",
    "ēir": "ei1 er5",
    "éir": "ei2 er5",
    "ěir": "ei3 er5",
    "èir": "ei4 er5",
    "eir": "ei5 er5",
    "uīr": "ui1 er5",
    "uír": "ui2 er5",
    "uǐr": "ui3 er5",
    "uìr": "ui4 er5",
    "uir": "ui5 er5",
    "āor": "ao1 er5",
    "áor": "ao2 er5",
    "ǎor": "ao3 er5",
    "àor": "ao4 er5",
    "aor": "ao5 er5",
    "ōur": "ou1 er5",
    "óur": "ou2 er5",
    "ǒur": "ou3 er5",
    "òur": "ou4 er5",
    "our": "ou5 er5",
    "iūr": "iu1 er5",
    "iúr": "iu2 er5",
    "iǔr": "iu3 er5",
    "iùr": "iu4 er5",
    "iur": "iu5 er5",
    "iēr": "ie1 er5",
    "iér": "ie2 er5",
    "iěr": "ie3 er5",
    "ièr": "ie4 er5",
    "ier": "ie5 er5",
    "üēr": "u:e1 er5",
    "üér": "u:e2 er5",
    "üěr": "u:e3 er5",
    "üèr": "u:e4 er5",
    "üer": "u:e5 er5",
    "ānr": "an1 er5",
    "ánr": "an2 er5",
    "ǎnr": "an3 er5",
    "ànr": "an4 er5",
    "anr": "an5 er5",
    "ēnr": "en1 er5",
    "énr": "en2 er5",
    "ěnr": "en3 er5",
    "ènr": "en4 er5",
    "enr": "en5 er5",
    "īnr": "in1 er5",
    "ínr": "in2 er5",
    "ǐnr": "in3 er5",
    "ìnr": "in4 er5",
    "inr": "in5 er5",
    "ūnr": "un1 er5",
    "únr": "un2 er5",
    "ǔnr": "un3 er5",
    "ùnr": "un4 er5",
    "unr": "un5 er5",
    "ǖn": "u:n1 er5",
    "ǘn": "u:n2 er5",
    "ǚn": "u:n3 er5",
    "ǜn": "u:n4 er5",
    "ün": "u:n5 er5",
}
PINYIN_CORRESPONDANCE_4 = {
    "āngr": "ang1 er5",
    "ángr": "ang2 er5",
    "ǎngr": "ang3 er5",
    "àngr": "ang4 er5",
    "angr": "ang5 er5",
    "ēngr": "eng1 er5",
    "éngr": "eng2 er5",
    "ěngr": "eng3 er5",
    "èngr": "eng4 er5",
    "engr": "eng5 er5",
    "īngr": "ing1 er5",
    "íngr": "ing2 er5",
    "ǐngr": "ing3 er5",
    "ìngr": "ing4 er5",
    "ingr": "ing5 er5",
    "ōngr": "ong1 er5",
    "óngr": "ong2 er5",
    "ǒngr": "ong3 er5",
    "òngr": "ong4 er5",
    "ongr": "ong5 er5",
}

EXCLUDE_VARIANT_REGEX_PATTERN = re.compile(r"{\[.*\]\}")
EXAMPLE_REGEX_PATTERN = re.compile(r"如：(.*?)。+")


def convert_pinyin_to_tone_numbers(pinyin, word):
    # Convert pinyin to tone numbers instead of the tone marks provided by Unihan
    ret = pinyin
    if pinyin[-4:] in PINYIN_CORRESPONDANCE_4:
        ret = pinyin[:-4] + PINYIN_CORRESPONDANCE_4[pinyin[-4:]]
    elif pinyin[-3:] in PINYIN_CORRESPONDANCE_3:
        ret = pinyin[:-3] + PINYIN_CORRESPONDANCE_3[pinyin[-3:]]
    elif pinyin[-2:] in PINYIN_CORRESPONDANCE_2:
        ret = pinyin[:-2] + PINYIN_CORRESPONDANCE_2[pinyin[-2:]]
    elif pinyin[-1:] in PINYIN_CORRESPONDANCE_1:
        ret = pinyin[:-1] + PINYIN_CORRESPONDANCE_1[pinyin[-1:]]
    else:
        logging.warning(
            f"Pinyin {pinyin} for word {word} could not be converted to tone numbers, uh oh!"
        )

    return ret


def insert_example(c, definition_id, starting_example_id, example):
    # The example should be a list of ExampleTuples, such that
    # the first item is the 'source', and all subsequent ones are the
    # translations
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
    #   - 0-999999999: Tatoebaa
    #   - 1000000000-1999999999: words.hk
    #   - 2000000000-2999999999: CantoDict
    #   - 3000000000-3999999999: CantoDict
    example_id = 3000000000

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

    # Set version of database
    database.write_database_version(c)

    # Delete old tables and indices, then create new one
    database.drop_tables(c)
    database.create_tables(c)

    # Add sources to table
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
    with open(filename) as f:
        data = json.load(f)

        items_parsed = 0

        for item in data:
            # For now, ignore variant characters that aren't properly encoded in Unicode
            if re.match(EXCLUDE_VARIANT_REGEX_PATTERN, item["title"]):
                continue

            # These do not change no matter the heteronym
            trad = item["title"]
            simp = HanziConv.toSimplified(trad)
            jyut = pinyin_jyutping_sentence.jyutping(
                trad, tone_numbers=True, spaces=True
            )
            freq = zipf_frequency(trad, "zh")

            defs = []

            last_heteronym_pinyin = ""
            heteronyms = item["heteronyms"]
            for heteronym in heteronyms:
                if "pinyin" not in heteronym:
                    logging.error(
                        f'Could not find pinyin for heteronym of word {trad} with definitions {heteronym["definitions"]}'
                    )
                    continue

                if (
                    last_heteronym_pinyin != ""
                    and heteronym["pinyin"] != last_heteronym_pinyin
                ):
                    # Different pronunciaton; append the old entry and add a new entry
                    pin = " ".join(
                        [
                            convert_pinyin_to_tone_numbers(syllable, trad)
                            for syllable in last_heteronym_pinyin.split()
                        ]
                    )

                    entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=defs)
                    words.append(entry)

                    # Reset the definitions list
                    defs = []

                for definition in heteronym["definitions"]:
                    label = definition["type"] if "type" in definition else ""

                    # Insert zero-width spaces so that we can reverse-search the definition
                    def_tuple = DefinitionTuple(
                        "​".join(jieba.cut(definition["def"])), label, []
                    )

                    # Insert examples
                    if "example" in definition:
                        for example in definition["example"]:
                            example_texts = (
                                re.match(EXAMPLE_REGEX_PATTERN, example)
                                .group(1)
                                .split("、")
                                if re.match(EXAMPLE_REGEX_PATTERN, example)
                                else example.split("、")
                            )

                            for example_text in example_texts:
                                example_text = example_text.strip("「」")
                                def_tuple.examples.append(
                                    ExampleTuple("cmn", "", example_text)
                                )

                    if "quote" in definition:
                        for quote in definition["quote"]:
                            def_tuple.examples.append(
                                ExampleTuple("cmn", "", quote)
                            )

                    # We currently ignore synonyms, antonyms, and "see also" links, because they are
                    # linked to definitions and we have no way to display that data...

                    defs.append(def_tuple)

                last_heteronym_pinyin = heteronym["pinyin"]

            pin = (
                " ".join(
                    [
                        convert_pinyin_to_tone_numbers(syllable, trad)
                        for syllable in heteronym["pinyin"].split()
                    ]
                )
                if "pinyin" in heteronym
                else ""
            )
            entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=defs)
            words.append(entry)

            items_parsed += 1
            if not items_parsed % 500:
                print(f"Parsed entry #{items_parsed}")


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<dict-revised.json filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py moedict.db ./dict-revised.json "
                '"Ministry of Education Dictionary (MoeDict)" MOE 2021-08-06 '
                '"本典為一部歷史語言辭典，記錄中古至現代各類詞語，並大量引用古典文獻書證，字 音部分則兼收現代及傳統音讀。" '
                '"中華民國教育部《重編國語辭典修訂本》資料採「創用CC-姓名標示- 禁止改作 3.0 臺灣授權條款」釋出'
                '本授權條款允許使用者重製、散布、傳輸著作（包括商業性利用），但不得修改該著作，使用時必須遵照「使用說明」之內容要求。" '
                '"https://language.moe.gov.tw/001/Upload/Files/site_content/M0001/respub/dict_reviseddict_download.html" "" "words,sentences"'
            )
        )
        sys.exit(1)

    cc_cedict.load()

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
