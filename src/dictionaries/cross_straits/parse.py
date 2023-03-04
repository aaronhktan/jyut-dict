from dragonmapper import transcriptions
import jieba
import opencc
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects
from database.utils import change_pinyin_to_match_phrase

from collections import namedtuple
import copy
import json
import logging
import re
import sqlite3
import sys
import traceback
import unicodedata

# Useful test words:
#   - 歛: multiple heteronyms, labels for definitions
#   - 穀: some duplicate definitions (姓。) and has labels for literary use (〈書〉)
#   - 横征暴敛: Pinyin has dashes in it (bad!)
#   - 空橋: contains 陸⃝ in the definition
#   - 空擋: contains 臺⃝ in the definition
#   - 空濛: has a literary definition
#   - 筋道: contains pinyin with weird a (ɑ)
#   - 一星半點兒: contains both "diǎr" and a dash in the middle of the pinyin
#   - 傍邊兒: contains "biār", has a colloquial use (〈口〉)
#   - 如: contains definitions with preposition POS label and conjunction POS label
#   - 全銜: incorrect Pinyin (xíɑn) which confuses dragonmapper
#   - 㾕: has an example which contains an enumeration comma
#   - 打緊: has pronunciation guide（ㄉㄟˇ　děi）in example
#   - 廠齡: has number in example

EXAMPLE_REGEX_PATTERN = re.compile(r"例⃝(.*?)。+")
INDIVIDUAL_EXAMPLE_REGEX_PATTERN = re.compile(r"「(.*?)」")
WHITESPACE_REGEX_PATTERN = re.compile(r"[　 ]")
VARIANT_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\s\(變\).*")
COLLOQUIAL_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\s（語音）.*")
STRANGE_ENTRY_REGEX_PATTERN = re.compile(r".*（.*\)")

LITERARY_REGEX_PATTERN = re.compile(r"〈(書)〉(.*)")
COLLOQUIAL_REGEX_PATTERN = re.compile(r"〈(口)〉(.*)")
POS_REGEX_PATTERN = re.compile(r"([量介連助敬謙嘆數代]詞)。(.+)")
LABEL_REGEX_PATTERNS = [
    LITERARY_REGEX_PATTERN,
    COLLOQUIAL_REGEX_PATTERN,
    POS_REGEX_PATTERN,
]

PUNCTUATION_TABLE = {}
for i in range(sys.maxunicode):
    if unicodedata.category(chr(i)).startswith("P"):
        PUNCTUATION_TABLE[i] = " " + chr(i) + " "

converter = opencc.OpenCC("tw2s.json")


# These are known to trip up Dragonmapper, but since there are so few just manually parse it ourselves
KNOWN_INVALID_SYLLABLES = {
    "呵": {"ō<br>陸⃝hē": ["o1", "he1"]},
    "哦": {"ó<br>陸⃝ò": ["o2", "o4"], "ó": ["o2"]},
    "哼唷": {"hēngyō<br>陸⃝hēngyāo": ["heng1 yo1", "heng1 yao1"]},
    "唷": {"yō<br>陸⃝yāo": ["yo1", "yao1"]},
    "喔": {"ō<br>陸⃝wō": ["o1", "wo1"]},
    "嗯": {
        "en<br>陸⃝éng": ["en5", "eng2"],
        "en<br>陸⃝ěng": ["en5", "eng3"],
        "en<br>陸⃝èng": ["en5", "eng4"],
    },
    "嗲": {"diē<br>陸⃝diǎ": ["die1", "dia3"]},
    "噢": {"yǔ<br>陸⃝ō": ["yu3", "o1"]},
    "嚄": {"ǒ": ["o3"]},
    "沙嗲": {"shādiē<br>陸⃝shādiǎ": ["sha1 die1", "sha1 dia3"]},
    "發嗲": {"fādiē<br>陸⃝fādiǎ": ["fa1 die1", "fa1 dia3"]},
}


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
    with open(filename) as f:
        data = json.load(f)

        items_parsed = 0

        # Each item in the JSON correspond to one or more entries in the dictionary
        # Most items map 1:1 to entries, e.g. "物質" is a single entry
        # Some items are 多音字, so they map to multiple entries (e.g. 重 -> zhòng and chóng)
        #
        # In the vocabulary of the the CSLD, each item may correspond to multiple heteronyms,
        # and each heteronym maps to a single entry.
        for item in data:
            # These do not change no matter the heteronym
            trad = item["title"]
            simp = converter.convert(trad)
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
                    logging.debug(
                        f'Could not find pinyin for heteronym of word {trad} with definitions {heteronym["definitions"]}'
                    )
                    continue

                # Filter out known bad pinyin
                if (
                    trad in KNOWN_INVALID_SYLLABLES
                    and heteronym["pinyin"] in KNOWN_INVALID_SYLLABLES[trad]
                ):
                    pins = KNOWN_INVALID_SYLLABLES[trad][heteronym["pinyin"]]
                else:
                    pins = heteronym["pinyin"].split("<br>陸⃝")

                    # Some weird a's cause dragonmapper to break, so replace them with standard a's.
                    pins = list(map(lambda x: x.replace("ɑ", "a"), pins))

                    # Remove dashes in pinyin
                    pins = list(map(lambda x: x.replace("-", " "), pins))

                    # Remove apostrophes in pinyin
                    pins = list(map(lambda x: x.replace("'", " "), pins))

                    # Remove commas in pinyin
                    pins = list(map(lambda x: x.replace(",", ""), pins))

                    # Remove weird characters
                    pins = list(map(lambda x: x.replace("陸⃟", ""), pins))

                    # Dragonmapper cannot handle some erhua
                    pins = list(map(lambda x: x.replace("diǎr", "diǎn er"), pins))
                    pins = list(map(lambda x: x.replace("biār", "biān er"), pins))

                    try:
                        # Converting from pinyin -> zhuyin inserts spaces between characters
                        # Converting from zhuyin -> pinyin conserves these spaces
                        pins = [
                            transcriptions.zhuyin_to_pinyin(
                                transcriptions.pinyin_to_zhuyin(x), accented=False
                            )
                            for x in pins
                        ]

                        for x in pins:
                            if x.count(" ") >= len(trad):
                                # This means that there was an extra space inserted somewhere; the pinyin is not valid
                                raise ValueError("Too many spaces in parsed Pinyin!")
                    except Exception as e:
                        # Try parsing zhuyin as a backup
                        pins = heteronym["bopomofo"].split("<br>陸⃝")

                        # Remove weird spaces in zhuyin
                        pins = list(map(lambda x: x.replace("　", " "), pins))

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

                if len(pins) > 1:
                    taiwan_pin = pins[0]
                    mainland_pin = pins[1]
                else:
                    taiwan_pin = mainland_pin = pins[0]

                if (
                    last_heteronym_pin != ""
                    and heteronym["pinyin"] != last_heteronym_pin
                ):
                    # A new different pinyin means that we are now processing a new heteronym.
                    # We must create an Entry object for the definitions of the old heteronym
                    # and add it to the list of entries before processing the new one.
                    entry = objects.Entry(
                        trad, simp, last_taiwan_pin, jyut, freq=freq, defs=taiwan_defs
                    )
                    words.append(entry)

                    if last_mainland_pin != last_taiwan_pin:
                        entry = objects.Entry(
                            trad,
                            simp,
                            last_mainland_pin,
                            jyut,
                            freq=freq,
                            defs=mainland_defs,
                        )
                        words.append(entry)

                    # Reset the definitions list
                    taiwan_defs = []
                    mainland_defs = []

                for definition in heteronym["definitions"]:
                    taiwan_label = "臺" if taiwan_pin != mainland_pin else ""
                    mainland_label = "陸" if mainland_pin != taiwan_pin else ""

                    definition_text = definition["def"]

                    # Take out parts of definitions that should be in labels
                    for pattern in LABEL_REGEX_PATTERNS:
                        if re.match(pattern, definition_text):
                            definition_label, definition_text = re.match(
                                pattern, definition_text
                            ).group(1, 2)
                            taiwan_label += (
                                "、" + definition_label
                                if taiwan_label
                                else definition_label
                            )
                            mainland_label += (
                                "、" + definition_label
                                if mainland_label
                                else definition_label
                            )

                    # Remove 臺⃝ and 陸⃝ from definitions, since Qt cannot display them
                    definition_text = definition_text.replace("臺⃝", "臺：")
                    definition_text = definition_text.replace("陸⃝", "陸：")

                    # Insert zero-width spaces so that we can reverse-search the definition
                    taiwan_def_tuple = objects.DefinitionTuple(
                        "​".join(jieba.cut(definition_text)), taiwan_label, []
                    )
                    mainland_def_tuple = objects.DefinitionTuple(
                        "​".join(jieba.cut(definition_text)), mainland_label, []
                    )

                    # Parse and add examples to this definition
                    if "example" in definition:
                        for example in definition["example"]:
                            if re.match(EXAMPLE_REGEX_PATTERN, example):
                                # Every example is surrounded by "如：<example>", so only keep the example
                                example = re.match(
                                    EXAMPLE_REGEX_PATTERN, example
                                ).group(1)
                                # Some examples contain multiple examples, so split them up by enclosing brackets 「」
                                example_texts = re.findall(
                                    INDIVIDUAL_EXAMPLE_REGEX_PATTERN, example
                                )
                            else:
                                logging.warning(
                                    f"Found example that does not fit the normal example regex pattern: {trad}, {example}"
                                )
                                # Fall back to splitting on Chinese enumeration comma
                                example_texts = example.split("、")

                            for example_text in example_texts:
                                # Strip out weird whitespace
                                example_text = re.sub(
                                    WHITESPACE_REGEX_PATTERN, "", example_text
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
                                    if len(item) > 1:
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
                                            example_pinyin = (
                                                change_pinyin_to_match_phrase(
                                                    example_text,
                                                    example_pinyin,
                                                    trad,
                                                    phrase_pinyin,
                                                )
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
                                            objects.ExampleTuple(
                                                "zho", example_pinyin, example_text
                                            )
                                        )
                                    elif index == 1:
                                        mainland_def_tuple.examples.append(
                                            objects.ExampleTuple(
                                                "zho", example_pinyin, example_text
                                            )
                                        )

                    taiwan_defs.append(taiwan_def_tuple)
                    mainland_defs.append(mainland_def_tuple)

                last_heteronym_pin = heteronym["pinyin"]
                last_taiwan_pin = taiwan_pin
                last_mainland_pin = mainland_pin

            entry = objects.Entry(
                trad, simp, taiwan_pin, jyut, freq=freq, defs=taiwan_defs
            )
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
