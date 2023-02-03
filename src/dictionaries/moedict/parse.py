import jieba
import opencc
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects
from database.utils import pinyin_to_tone_numbers, change_pinyin_to_match_phrase

from collections import namedtuple
import itertools
import json
import logging
import re
import sqlite3
import sys
import traceback
import unicodedata

# Useful test words:
#   - 重: multiple heteronyms, labels for definitions
#   - 万: one heteronym has pinyin (should be included), other does not (should be excluded)
#   - 兵: contains example "傳 令兵", which has a space.
#   - 中心點: contains example "從他的報告中，可以知道他沒有掌握到這件評估案的　中心點", which contains the whitespace character "　"
#   - 不全: contains example "他所提出的理由，不全是對的", which contains a full-width comma
#   - 那搭（Namibia): example of weird formatting
#   - 鹽: has an example with enumeration commas
#   - 麻: contains example that should already be inserted into the database
#   - 削: contains 讀音 and 語音
#   - 剖: contains （又音）
#   - 乾兒: transcription for 兒 is super annoying
#   - 倘來之物: has {[90ba]} in the pronunciation
#   - 不到頭: has {[97ef]} before a comma
#   - 救應: has {[9afc]} after a period
#   - 三十年河東，三十年河西: has multiple punctuation marks one after the other

PINYIN_COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN = re.compile(r"（語音）")
PINYIN_LITERARY_PRONUNCIATION_REGEX_PATTERN = re.compile(r"（讀音）")
PINYIN_ALTERNATE_PRONUNCIATION_REGEX_PATTERN = re.compile(r"（又音）")
PINYIN_SECOND_ALTERNATE_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\(變\)")
EXCLUDE_VARIANT_REGEX_PATTERN = re.compile(r"{\[.*\]\}")
EXAMPLE_REGEX_PATTERN = re.compile(r"如：(.*)")
INDIVIDUAL_EXAMPLE_REGEX_PATTERN = re.compile(r"「(.*?)」")
WHITESPACE_REGEX_PATTERN = re.compile(r"[　 ]")
VARIANT_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\s\(變\).*")
COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\s（語音）.*")
STRANGE_ENTRY_REGEX_PATTERN = re.compile(r".*（.*\)")

PUNCTUATION_TABLE = {}
for i in range(sys.maxunicode):
    if unicodedata.category(chr(i)).startswith("P"):
        PUNCTUATION_TABLE[i] = " " + chr(i) + " "

converter = opencc.OpenCC("tw2s.json")


def insert_example(c, definition_id, starting_example_id, example):
    examples_inserted = 0

    trad = example.content
    simp = converter.convert(trad)
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

    insert_words(c, entries)

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
            # For now, ignore variant characters that aren't properly encoded in Unicode
            if re.match(EXCLUDE_VARIANT_REGEX_PATTERN, item["title"]):
                continue

            # These do not change no matter the heteronym
            trad = item["title"]
            simp = converter.convert(trad)
            jyut = pinyin_jyutping_sentence.jyutping(
                trad, tone_numbers=True, spaces=True
            )
            freq = zipf_frequency(trad, "zh")

            # Build up a list of definitions for each heteronym
            defs = []

            # Distinguish between heteronyms by their pinyin – if the pinyin of the
            # current heteronym does not match the old pinyin, then a new heteronym
            # must be created
            last_heteronym_pin = ""

            # Go through each heteronym, creating Entry objects for each one
            for heteronym in item["heteronyms"]:
                if "pinyin" not in heteronym:
                    logging.debug(
                        f'Could not find pinyin for heteronym of word {trad} with definitions {heteronym["definitions"]}'
                    )
                    continue

                pin = PINYIN_COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN.sub(
                    "", heteronym["pinyin"]
                )
                pin = PINYIN_LITERARY_PRONUNCIATION_REGEX_PATTERN.sub("", pin)
                pin = PINYIN_ALTERNATE_PRONUNCIATION_REGEX_PATTERN.sub("", pin)
                pin = PINYIN_SECOND_ALTERNATE_PRONUNCIATION_REGEX_PATTERN.sub("", pin)
                pin = pin.split()
                pin = [
                    pinyin_to_tone_numbers(syllable, trad).split() for syllable in pin
                ]
                pin = list(itertools.chain.from_iterable(pin))
                pin = pin[: len(trad)]
                pin = " ".join(pin)
                if last_heteronym_pin != "" and pin != last_heteronym_pin:
                    # Different pinyin means that we are now processing a new heteronym.
                    # We must create an Entry object for the definitions of the old heteronym
                    # and add it to the list of entries before processing the new one.
                    entry = objects.Entry(
                        trad, simp, last_heteronym_pin, jyut, freq=freq, defs=defs
                    )
                    words.append(entry)

                    # Reset the definitions list
                    defs = []

                for definition in heteronym["definitions"]:
                    label = definition["type"] if "type" in definition else ""

                    # Insert zero-width spaces so that we can reverse-search the definition
                    def_tuple = objects.DefinitionTuple(
                        "​".join(jieba.cut(definition["def"])), label, []
                    )

                    # Parse and add examples to this definition
                    if "example" in definition:
                        for example in definition["example"]:
                            if EXAMPLE_REGEX_PATTERN.match(example):
                                # Every example is surrounded by "如：<example>", so only keep the example
                                example = EXAMPLE_REGEX_PATTERN.match(example).group(1)
                                # Some examples contain multiple examples, so split them up by enclosing brackets 「」
                                example_texts = (
                                    INDIVIDUAL_EXAMPLE_REGEX_PATTERN.findall(example)
                                )
                            else:
                                logging.warning(
                                    f"Found example that does not fit the normal example regex pattern: {trad}, {example}"
                                )
                                # Fall back to splitting on Chinese enumeration comma
                                example_texts = example.split("、")

                            for example_text in example_texts:
                                # Strip out weird whitespace
                                example_text = WHITESPACE_REGEX_PATTERN.sub(
                                    "", example_text
                                )

                                # Translating using the PUNCTUATION_TABLE adds spaces to series of full-width
                                # punctuation marks, which is necessary so that the split() in
                                # change_pinyin_to_match_phrase() creates a list of Pinyin where each entry in that
                                # list corresponds 1:1 to each Unicode glyph.
                                #
                                # e.g. "《元．秦{[90ba]}夫《東堂老．第三折》：「忠..." passes through lazy_pinyin(), so it turns into:
                                # ['yuan2', '．', 'qin2', '{[90ba]}', 'fu1', '《', 'dong1', 'tang2', 'lao3', '．', 'di4', 'san1', 'zhe2', '》：「', 'zhong1']
                                # Then we add spaces between the punctuation marks and the {[90ba]} to get:
                                # "yuan2  ．  qin2  {   [  9 0 b a  ]   }  fu1  《  dong1 tang2 lao3  ．  di4 san1 zhe2  》  ：  「  zhong1 xiao4 shi4 li4 shen1 zhi1 ben3  ，  zhe4 qian2 cai2 shi4 tang3 lai2 zhi1 wu4  。  」"
                                # Finally, change_pinyin_to_match_phrase() calls split() on that string, and it sees:
                                # ['yuan2', '．', 'qin2', '{', '[', '9', '0', 'b', 'a', ']', '}', 'fu1', '《', 'dong1', 'tang2', 'lao3', '．', 'di4', 'san1', 'zhe2', '》', '：', '「', 'zhong1']
                                example_pinyin_list = lazy_pinyin(
                                    converter.convert(example_text),
                                    style=Style.TONE3,
                                    neutral_tone_with_five=True,
                                )
                                example_pinyin = []
                                for item in example_pinyin_list:
                                    if "{[" in item and "]}" in item:
                                        example_pinyin += " ".join(item).split()
                                    else:
                                        example_pinyin.append(item)
                                example_pinyin = " ".join(example_pinyin).lower()
                                example_pinyin = example_pinyin.translate(
                                    PUNCTUATION_TABLE
                                )
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
                                phrase_pinyin = pin
                                phrase_pinyin = VARIANT_PRONUNCIATION_REGEX_PATTERN.sub(
                                    "",
                                    phrase_pinyin,
                                )
                                phrase_pinyin = (
                                    COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN.sub(
                                        "",
                                        phrase_pinyin,
                                    )
                                )

                                # Do not try to match entries formatted like "那搭（Namibia)"
                                if not STRANGE_ENTRY_REGEX_PATTERN.match(trad):
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

                                def_tuple.examples.append(
                                    objects.ExampleTuple(
                                        "cmn", example_pinyin, example_text
                                    )
                                )

                    # Parse and add quotes to this definition
                    if "quote" in definition:
                        for quote in definition["quote"]:
                            quote_text = re.sub(WHITESPACE_REGEX_PATTERN, "", quote)

                            quote_pinyin_list = lazy_pinyin(
                                converter.convert(quote_text),
                                style=Style.TONE3,
                                neutral_tone_with_five=True,
                            )
                            quote_pinyin = []
                            for item in quote_pinyin_list:
                                if "{[" in item and "]}" in item:
                                    quote_pinyin += " ".join(item).split()
                                else:
                                    quote_pinyin.append(item)
                            quote_pinyin = " ".join(quote_pinyin).lower()
                            quote_pinyin = quote_pinyin.translate(PUNCTUATION_TABLE)
                            quote_pinyin = quote_pinyin.strip().replace("v", "u:")

                            phrase_pinyin = pin
                            phrase_pinyin = re.sub(
                                VARIANT_PRONUNCIATION_REGEX_PATTERN, "", phrase_pinyin
                            )
                            phrase_pinyin = re.sub(
                                COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN,
                                "",
                                phrase_pinyin,
                            )

                            if not re.match(STRANGE_ENTRY_REGEX_PATTERN, trad):
                                try:
                                    quote_pinyin = change_pinyin_to_match_phrase(
                                        quote_text, quote_pinyin, trad, phrase_pinyin
                                    )
                                except Exception as e:
                                    logging.warning(
                                        f"Couldn't change pinyin in quote for word {trad}: "
                                        f"{''.join(quote_text)}, {quote_pinyin}, {pin} "
                                        f"{e}"
                                    )
                                    traceback.print_exc()
                            def_tuple.examples.append(
                                objects.ExampleTuple("zho", quote_pinyin, quote_text)
                            )

                    # We currently ignore synonyms, antonyms, and "see also" links, because they are
                    # linked to definitions and we have no way to display that data...

                    defs.append(def_tuple)

                last_heteronym_pin = pin

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
                '"Ministry of Education Dictionary (MoEDict)" MOE 2021-08-06 '
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
