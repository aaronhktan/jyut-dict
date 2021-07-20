from bs4 import BeautifulSoup, element
from wordfreq import zipf_frequency

from database import database, objects

import logging
import os
import re
import sqlite3
import sys

# Useful test pages:
#   - 鼻哥 for multiple definitions on a single line
#   - 一 for multiple definitions
#   - 惡 for multiple pronunciations
#   - 傾掂 for POS + multiple definitions on same line
#   - 庶 for weirdness involving newlines and <br> tags
#   - 福地 for lack of space between [1] and [Taoism/Daoism]
#   - 蚯蚓 for brackets in the middle of a definition
#   - 柏 for listing of pronunciations with commas instead of |
#   - 天秤 for listing of pronunciations with / instead of |
#   - 一手遮天 for idiom, 香港電台 for brandname, 香港 for place name

# Pages with known issues:
#   - 茄哩啡  (what is that Jyutping notation?)
#   - 等 (again, what is that Jyutping notation?)
#   - 蚺蛇 has different Jyutping for each definition, without any labelling
#   - 大使 has multiple definitions on the same line, without numbering
#   - 嘛 has nonstandard labels for Jyutping and Pinyin (instead of 粵 and 國|普, it names jyutping or pinyin)
#   - 的 has a nonstandard way of indicating different definitions

illegal_strings = ("Default PoS:", "Additional PoS:")


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


def parse_file(file_name, words):
    with open(file_name, "r") as file:
        soup = BeautifulSoup(file, "html.parser")

        # Extract the traditional and simplified forms
        try:
            forms = [
                x.strip()
                for x in soup.find("td", class_="chinesebig").get_text().split(" / ")
            ]
            if len(forms) > 1:
                trad = forms[0].strip()
                simp = forms[1].strip()
            else:
                trad = simp = forms[0].strip()
        except:
            logging.error(
                f"Couldn't find traditional and simplified forms in file {file_name}"
            )
            return

        word = os.path.splitext(os.path.basename(file_name))[0]
        if trad != word:
            if len(trad) == len(word):
                logging.debug(
                    f"{trad} and {word} have the same length, is this a simplified/traditional mixup?"
                )
                return
            else:
                logging.warning(
                    f"Hmm, looks like the parsed word {trad} doesn't match the filename {word}"
                )
                return

        freq = zipf_frequency(trad, "zh")

        # Extract the pronunciations
        # CantoDict indicates differences in literary/colloquial pronunciation with *, but we don't support that
        # So remove the stars
        jyut_element = soup.find("span", class_="cardjyutping")
        jyut = jyut_element.get_text() if jyut_element else ""
        jyut = re.sub(r"\d\*", "", jyut)

        pin_element = soup.find("span", class_="cardpinyin")
        pin = pin_element.get_text() if pin_element else ""

        # Extract the meaning element
        meaning_element = soup.find("td", class_="wordmeaning")

        # Check for special labels in compound words (brandname, idiom, placename, etc.)
        special_label = ""
        special_pos_elem = meaning_element.find("img", class_="flagicon")
        if special_pos_elem:
            special_label = special_pos_elem["alt"]

        # The layout of compound word pages is different from single-character pages
        real_meaning_element = meaning_element.find("div", class_=None)
        if real_meaning_element:
            meaning_element = real_meaning_element

        # Remove children (these usually contain useless fluff that interfere with definition parsing)
        children = meaning_element.find_all("div")
        children += meaning_element.find_all("span")
        for child in children:
            child.decompose()

        # Parse the meanings from the meaning element
        meanings = []
        # CantoDict puts some weird stuff in the meanings div, and the only way to separate
        # them out is to replace the <br> tags with "\n"
        for br in soup.find_all("br"):
            br.replace_with("\n")

        strings = re.compile(r"\[\d+\]\s?|\n").split(meaning_element.get_text())
        for string in strings:
            string = string.strip()
            if not string or any([x in string for x in illegal_strings]):
                continue

            # Check if this line is a line that denotes a specific pronunciation
            result = re.search(r"\[粵\]\s?(.+\d+)\s?[\||/|,]?\s?\[[國|普]\]\s?(.+\d+)", string)
            if result:
                # If there are previous definitions, add them to the words dict now
                # Since those meanings belong to the previous pronunciation
                if meanings:
                    entry = objects.Entry(
                        trad,
                        simp,
                        pin,
                        jyut,
                        freq=freq,
                        defs=meanings,
                    )
                    words.append(entry)

                # Then, extract the new pinyin and jyutping
                # and reset the meanings tuple
                jyut = result.group(1)
                jyut = re.sub(r"\d\*", "", jyut)
                pin = result.group(2)
                meanings = []
                continue

            # Try to isolate a label (usually a POS or [華]: indicating Mandarin-only usage or [粵]: indicating Cantonese-only usage)
            label = ""
            definition = string
            result = re.search(r"^\[(.*?)\]:?\s+(.*)", string)
            if result:
                label = result.group(1).strip()
                definition = result.group(2).strip()
            else:
                # Filter out bad non-standard strings
                if string[0] == "[" and string[-1] == "]":
                    continue

            if not label and special_label:
                label = special_label
            meanings.append((label, definition))

        if meanings:
            entry = objects.Entry(
                trad, simp, pin, jyut, freq=freq, defs=meanings
            )
            words.append(entry)


def parse_folder(folder_name, words):
    for index, entry in enumerate(os.scandir(folder_name)):
        if not index % 100:
            print(f"Parsed word #{index}")
        if entry.is_file() and entry.path.endswith(".html"):
            parse_file(entry.path, words)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<HTML folder> <source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py cantodict.db scraped/data/ CantoDict CD 2021-07-18 "
                '"CantoDict is a collaborative Chinese Dictionary project started in November 2003. '
                'Entries are added and mistakes corrected by a team of kind volunteers from around the world." '
                '"https://www.cantonese.sheik.co.uk/copyright.htm" "https://www.cantonese.sheik.co.uk/" "" ""'
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

    words = []
    parse_folder(sys.argv[2], words)
    write(sys.argv[1], source, words)
