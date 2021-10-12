from bs4 import BeautifulSoup
from hanziconv import HanziConv
from hkscs_unicode_converter import converter
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

import logging
import os
import re
import sqlite3
import sys

# Useful test pages:
#   - 尖嘴 for single definition
#   - 局 for two definitions
#   - 巴士 for three definitions
#   - 市 for comment
#   - 重 contains HKSCS characters in the Private-Use Areas of Unicode that must be converted
#   - 鏈 contains characters that are non-standard, not in HKSCS, and should be replaced with PRIVATE_USE_AREA_REPLACEMENT_STRING

PRIVATE_USE_AREA_REGEX = re.compile("[\ue000-\uf8ff]")
PRIVATE_USE_AREA_REPLACEMENT_STRING = "☒"
LABEL_REGEX = re.compile("MainContent_repeaterRecord_lbl詞彙類別_*")
JYUTPING_LETTERS_ID_REGEX = re.compile("MainContent_repeaterRecord_lbl粵語拼音_*")
JYUTPING_NUMBERS_ID_REGEX = re.compile("MainContent_repeaterRecord_lbl聲調_*")
# CUHK indicates differences in literary and colloquial pronunciation [0-9]\*[0-9].
# We don't want the literary pronunciation, so discard the first capturing group.
JYUTPING_NUMBERS_REGEX = re.compile("(?:\d\*)*(\d+)")
MEANING_REGEX = re.compile(
    "MainContent_repeaterRecord_repeaterTranslation_0_lblTranslation_*"
)
REMARK_REGEX = re.compile("MainContent_repeaterRecord_lblRemark_*")

JYUTPING_MAP = {"7": "1", "8": "3", "9": "6"}


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

    for entry in entries:
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

        for label, definition in entry.definitions:
            definition_id = database.insert_definition(
                c, definition, label, entry_id, 1, None
            )
            if definition_id == -1:
                logging.error(
                    f"Could not insert definition {definition} for word {entry.traditional}, uh oh!"
                )
                continue

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_word_file(file_name, words):
    with open(file_name, "r") as file:
        soup = BeautifulSoup(file, "html.parser")

        # Extract the word on the page
        trad = converter.convert_string(
            soup.find("span", class_="ChiCharFix").get_text()
        )
        if re.search(PRIVATE_USE_AREA_REGEX, trad):
            logging.warning(
                f"Hmm, looks like the word {trad} contains nonstandard characters, replacing with squares..."
            )
            trad = re.sub(
                PRIVATE_USE_AREA_REGEX, PRIVATE_USE_AREA_REPLACEMENT_STRING, trad
            )
        simp = HanziConv.toSimplified(trad)

        word = os.path.splitext(os.path.basename(file_name))[0]
        word = converter.convert_string(word)
        if re.search(PRIVATE_USE_AREA_REGEX, word):
            word = re.sub(
                PRIVATE_USE_AREA_REGEX, PRIVATE_USE_AREA_REPLACEMENT_STRING, word
            )
        if trad != word:
            logging.warning(
                f"Hmm, looks like the parsed word {trad} doesn't match the filename {word}"
            )
            return

        freq = zipf_frequency(trad, "zh")

        # Get the type of word
        label = soup.find("span", id=LABEL_REGEX).get_text()

        # Get the pronunciation, which is split up into the letter portion and the number portion
        jyutping_letters = soup.find("span", id=JYUTPING_LETTERS_ID_REGEX).get_text()
        jyutping_letters = jyutping_letters.split()

        jyutping_numbers = soup.find("span", id=JYUTPING_NUMBERS_ID_REGEX).get_text()
        jyutping_numbers = JYUTPING_NUMBERS_REGEX.findall(jyutping_numbers)
        jyutping_numbers = [
            JYUTPING_MAP[x] if x in JYUTPING_MAP else x for x in jyutping_numbers
        ]  # Replacement is needed because CUHK uses 7-8-9 notation for checked tones instead of 1-3-6

        jyut = [x[0] + x[1] for x in zip(jyutping_letters, jyutping_numbers)]
        jyut = " ".join(jyut)

        # Automatically generate pinyin
        pin = " ".join(
            lazy_pinyin(trad, style=Style.TONE3, neutral_tone_with_five=True)
        ).lower()

        # Extract the meanings
        meaning_elements = soup.find_all("span", id=MEANING_REGEX)
        defs = [
            (label, meaning_element.get_text()) for meaning_element in meaning_elements
        ]

        # Add remarks, if one exists on the page
        remark = soup.find("span", id=REMARK_REGEX).get_text()
        if remark:
            defs.append(("備註", remark))

        entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=defs)
        words.append(entry)


def parse_words_folder(folder_name, words):
    for index, entry in enumerate(os.scandir(folder_name)):
        if not index % 100:
            print(f"Parsed word #{index}")
        if entry.is_file() and entry.path.endswith(".html"):
            parse_word_file(entry.path, words)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<entries HTML folder> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py cuhk.db scraped_words/ 現代標準漢語與粵語對照資料庫 CUHK 2021-10-03 "
                '"香港通行廣州話（以下簡稱粵語），本地逾九成師生之母語為粵語。" '
                '"(c) 2014保留版權 香港中文大學 中國語言及文學系" "https://apps.itsc.cuhk.edu.hk/hanyu/Page/Cover.aspx" "" "words"'
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

    words = []
    parse_words_folder(sys.argv[2], words)
    write(sys.argv[1], source, words)
