from dragonmapper import transcriptions
from hanziconv import HanziConv
import hanzidentifier
import jieba
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects
from database.utils import pinyin_to_tone_numbers

from collections import namedtuple
import copy
import json
import logging
import re
import sqlite3
import sys
import traceback

# Useful test words:
#   - 歛: multiple heteronyms, labels for definitions
#   - 穀: some duplicate definitions (姓。) and has labels for literary use (〈書〉)
#   - 横征暴敛: Pinyin has dashes in it (bad!)

DefinitionTuple = namedtuple("Definition", ["definition", "label", "examples"])
ExampleTuple = namedtuple("ExampleTuple", ["lang", "pron", "content"])

EXAMPLE_REGEX_PATTERN = re.compile(r"例⃝(.*?)。+")
WHITESPACE_REGEX_PATTERN = re.compile(r"[　 ]")
VARIANT_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\s\(變\).*")
COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\s（語音）.*")
STRANGE_ENTRY_REGEX_PATTERN = re.compile(r".*（.*\)")


# Since the pinyin returned by lazy_pinyin doesn't always match the pinyin
# given in the heteronym, attempt to replace pinyin corresponding to the
# characters in this heteronym with the pinyin provided by the JSON file.
#
# e.g. example_text = "重新"; example_pinyin = "zhong4 xin1" (returned by lazy_pinyin)
# trad = "重", phrase_pinyin = "chong2" (provided by JSON file)
# means that we should convert "zhong4 xin1" to "chong2 xin1"
def change_pinyin_to_match_phrase(example, example_pinyin, phrase, phrase_pinyin):
    phrase_indices = [i.start() for i in re.finditer(phrase, example)]
    example_pinyin_list = example_pinyin.split()
    phrase_pinyin_list = phrase_pinyin.split()

    for i in phrase_indices:
        # I can't do a simple replacement with list slicing, because
        # sometimes the example contains punctuation that the pinyin does not have
        # (e.g. "三十年河東，三十年河西" -> "san1 shi2 nian2 he2 dong1 san1 shi2 nian2 he2 xi1")
        # so we must loop through the example, ignoring punctuation
        example_index = 0
        phrase_pinyin_index = 0
        while phrase_pinyin_index < len(phrase_pinyin_list):
            if example_pinyin_list[i + example_index] == "，":
                example_index += 1
                continue
            example_pinyin_list[i + example_index] = phrase_pinyin_list[
                phrase_pinyin_index
            ]
            example_index += 1
            phrase_pinyin_index += 1

    return " ".join(example_pinyin_list)


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
    #   - 0-999999999: Tatoeba
    #   - 1000000000-1999999999: words.hk
    #   - 2000000000-2999999999: CantoDict
    #   - 3000000000-3999999999: MoEDict
    #   - 4000000000-4999999999: Cross-Straits Dictionary
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

        # Each item in the JSON correspond to one or more entries in the dictionary
        # Most items map 1:1 to entries, e.g. "物質" is a single entry
        # Some items are 多音字, so they map to multiple entries (e.g. 重 -> zhòng and chóng)
        #
        # In the vocabulary of the MoEDict, each item may correspond to multiple heteronyms,
        # and each heteronym maps to a single entry.
        for item in data:
            # These do not change no matter the heteronym
            trad = item["title"]
            simp = HanziConv.toSimplified(trad)
            jyut = pinyin_jyutping_sentence.jyutping(
                trad, tone_numbers=True, spaces=True
            )
            freq = zipf_frequency(trad, "zh")

            # Some items have multiple pronunciations (one for Taiwan, one for Mainland China)
            taiwan_pin = mainland_pin = ""

            # Build up a list of definitions for each heteronym
            taiwan_defs = []
            mainland_defs = []

            # Distinguish between heteronyms by their pinyin – if the pinyin of the
            # current heteronym does not match the old pinyin, then a new heteronym
            # must be created
            last_heteronym_pin = ""
            last_taiwan_pin = last_mainland_pin = ""

            # Go through each heteronym, creating Entry objects for each one
            for heteronym in item["heteronyms"]:
                if "pinyin" not in heteronym:
                    logging.error(
                        f'Could not find pinyin for heteronym of word {trad} with definitions {heteronym["definitions"]}'
                    )
                    continue

                pins = heteronym["pinyin"].split("<br>陸⃝")
                # Converting from pinyin -> zhuyin inserts spaces between characters
                # Converting from zhuyin -> pinyin conserves these spaces
                try:
                    pins = [transcriptions.zhuyin_to_pinyin(transcriptions.pinyin_to_zhuyin(x), accented=False) for x in pins]
                except Exception as e:
                    logging.error(f"Unable to split up Pinyin for word {trad}: {e}")
                    continue
                # Remove commas in pinyin
                pins = list(map(lambda x: x.replace(",", ""), pins))

                if len(pins) > 1:
                    taiwan_pin = pins[0]
                    mainland_pin = pins[1]
                else:
                    taiwan_pin = mainland_pin = pins[0]

                if last_heteronym_pin != "" and heteronym["pinyin"] != last_heteronym_pin:
                    # Different pinyin means that we are now processing a new heteronym.
                    # We must create an Entry object for the definitions of the old heteronym
                    # and add it to the list of entries before processing the new one.
                    entry = objects.Entry(
                        trad, simp, last_taiwan_pin, jyut, freq=freq, defs=taiwan_defs
                    )
                    words.append(entry)

                    if last_mainland_pin != last_taiwan_pin:
                        entry = objects.Entry(
                            trad, simp, last_mainland_pin, jyut, freq=freq, defs=mainland_defs
                        )
                        words.append(entry)

                    # Reset the definitions list
                    taiwan_defs = []
                    mainland_defs = []

                for definition in heteronym["definitions"]:
                    taiwan_label = "臺" if taiwan_pin != mainland_pin else ""
                    mainland_label = "陸" if mainland_pin != taiwan_pin else ""

                    # Insert zero-width spaces so that we can reverse-search the definition
                    taiwan_def_tuple = DefinitionTuple(
                        "​".join(jieba.cut(definition["def"])), taiwan_label, []
                    )
                    mainland_def_tuple = DefinitionTuple(
                        "​".join(jieba.cut(definition["def"])), mainland_label, []
                    )

                    # Parse and add examples to this definition
                    if "example" in definition:
                        for example in definition["example"]:
                            if re.match(EXAMPLE_REGEX_PATTERN, example):
                                example_texts = (
                                    # Every example is surrounded by "例⃝<example>。", so only keep the example
                                    re.match(EXAMPLE_REGEX_PATTERN, example).group(1)
                                    # Some examples contain multiple examples, so split them up by the enumeration comma
                                    .split("、")
                                )
                            else:
                                example_texts = example.split("、")

                            for example_text in example_texts:
                                # Strip out Chinese quotation marks at the beginning and end of the example
                                example_text = example_text.strip("「」")
                                # Strip out weird whitespace
                                example_text = re.sub(
                                    WHITESPACE_REGEX_PATTERN, "", example_text
                                )

                                # Joining and splitting separates series of full-width punctuation marks
                                # into separate items,  which is necessary so that lazy_pinyin() returns
                                # separate items for each full-width punctuation mark in the list it returns
                                #
                                # e.g. "《儒林外史．第四六回》：「成老爹道..." turns into
                                # "《 儒 林 外 史 ． 第 四 六 回 》 ： 「 成 老 爹 道", which turns into
                                # ['《', '儒', '林', '外', '史', '．', '第', '四', '六', '回', '》', '：', '「', '成', '老', '爹', '道']
                                # (Notice how "》：「"" is now split up into three different items)
                                example_pinyin = lazy_pinyin(
                                    " ".join(example_text).split(),
                                    style=Style.TONE3,
                                    neutral_tone_with_five=True,
                                )
                                example_pinyin = " ".join(example_pinyin).lower()
                                example_pinyin = example_pinyin.strip().replace(
                                    "v", "u:"
                                )

                                # Since the pinyin returned by lazy_pinyin doesn't always match the pinyin
                                # given in the heteronym, attempt to replace pinyin corresponding to the
                                # characters in this heteronym with the pinyin provided by the JSON file.
                                #
                                # e.g. example_text = "重新"; example_pinyin = "zhong4 xin1" (returned by lazy_pinyin)
                                # trad = "重", phrase_pinyin = "chong2"
                                # means that we should convert "zhong4 xin1" to "chong2 xin1"

                                # Strip out variant pronunciations for conversion purposes
                                for index, pin in enumerate([taiwan_pin, mainland_pin]):
                                    phrase_pinyin = pin
                                    phrase_pinyin = re.sub(
                                        VARIANT_PRONUNCIATION_REGEX_PATTERN,
                                        "",
                                        phrase_pinyin,
                                    )
                                    phrase_pinyin = re.sub(
                                        COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN,
                                        "",
                                        phrase_pinyin,
                                    )

                                    # Do not try to match entries formatted like "那搭（Namibia)"
                                    if not re.match(STRANGE_ENTRY_REGEX_PATTERN, trad):
                                        try:
                                            example_pinyin = change_pinyin_to_match_phrase(
                                                example_text,
                                                example_pinyin,
                                                trad,
                                                phrase_pinyin,
                                            )
                                        except Exception as e:
                                            logging.warning(
                                                f"Couldn't change pinyin in example for word {trad}: "
                                                f"{''.join(example_text)}, {example_pinyin}, {pin}, "
                                                f"{e}"
                                            )
                                            traceback.print_exc()

                                    if index == 0:
                                        taiwan_def_tuple.examples.append(
                                            ExampleTuple("cmn", example_pinyin, example_text)
                                        )
                                    elif index == 1:
                                        mainland_def_tuple.examples.append(
                                            ExampleTuple("cmn", example_pinyin, example_text)
                                        )

                    # We currently ignore synonyms, antonyms, and "see also" links, because they are
                    # linked to definitions and we have no way to display that data...

                    taiwan_defs.append(taiwan_def_tuple)
                    mainland_defs.append(mainland_def_tuple)

                last_heteronym_pin = heteronym["pinyin"]
                last_taiwan_pin = taiwan_pin
                last_mainland_pin = mainland_pin

            entry = objects.Entry(trad, simp, taiwan_pin, jyut, freq=freq, defs=taiwan_defs)
            words.append(entry)

            if mainland_pin != taiwan_pin:
                entry = objects.Entry(
                    trad, simp, mainland_pin, jyut, freq=freq, defs=mainland_defs
                )
                words.append(entry)

            items_parsed += 1
            if not items_parsed % 500:
                print(f"Parsed entry #{items_parsed}")


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<dict-csld.json filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py cross_straits.db ./dict-clsd.json "
                '"Cross-Straits Language Database" CSLD 2021-08-06 '
                '"兩岸差異用詞主要依據《中華語文大辭典》所收兩岸差異語詞加以分類表列，以一般性常用語詞為主，'
                '如生活、文化、社會等各領域的用語，也包含學術專業語詞，如物理、化學、生物等各學科的用語。" '
                '"《兩岸詞典》由中華文化總會以 CC BY-NC-ND 4.0 之条款下提供。" '
                '"http://www.chinese-linguipedia.org/" "" "words,sentences"'
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
