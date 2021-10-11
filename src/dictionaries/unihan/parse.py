import hanzidentifier
from hanziconv import HanziConv
from wordfreq import zipf_frequency

from database import database, objects

import csv
import logging
import sqlite3
import sys

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
}

bad = 0

def read_csv(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile, delimiter="\t")
        for row in reader:
            yield row


def convert_pinyin_to_tone_numbers(pinyin, word):
    # Convert pinyin to tone numbers instead of the tone marks provided by Unihan
    ret = pinyin
    if pinyin[-3:] in PINYIN_CORRESPONDANCE_3:
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

    for key in entries:
        entry = entries[key]
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
                    f"Could not insert definition {definition} for word {entry.traditional} "
                    "- check if the definition is a duplicate!"
                )
                continue

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_dictionary_like_data(filename, words):
    for line in read_csv(filename):
        if len(line) != 3 or line[0].startswith("#"):
            continue

        codepoint = line[0]
        fieldname = line[1]
        content = line[2]

        if fieldname != "kCangjie":
            continue

        character = chr(int(codepoint[2:], 16))
        trad = HanziConv.toTraditional(character)
        if trad not in words:
            logging.info(
                f"{character} not in database but Cangjie found - may be compatibility variant. "
                "Check https://www.unicode.org/charts/unihan.html"
            )
            continue

        if words[trad].traditional == words[trad].simplified:
            if any(label == "Cangjie Input" for (label, _) in words[trad].definitions):
                # Don't add Cangjie twice!
                continue
            words[trad].append_to_defs(("Cangjie Input", content))
        elif words[trad].simplified == character:
            # Sometimes, the Cangjie input code is the same for both traditional and simplified
            # even if the characters are different (see "齷")
            # So we re-label it to apply to both forms of the character
            inserted = False
            for i in range(len(words[trad].definitions)):
                if words[trad].definitions[i][1] == content:
                    words[trad].definitions[i] = ("Cangjie Input", content)
                    inserted = True
                    break
            if not inserted:
                words[trad].append_to_defs(("Cangjie Input - Simplified", content))
        elif words[trad].traditional == character:
            # Sometimes, the Cangjie input code is the same for both traditional and simplified
            # even if the characters are different (see "齷")
            # So we re-label it to apply to both forms of the character
            inserted = False
            for i in range(len(words[trad].definitions)):
                if words[trad].definitions[i][1] == content:
                    words[trad].definitions[i] = ("Cangjie Input", content)
                    inserted = True
                    break
            if not inserted:
                words[trad].append_to_defs(("Cangjie Input - Traditional", content))
        else:
            logging.warning(
                f"The character {character} doesn't match either traditional nor simplified forms of the dictionary word: "
                f"[character: {character}, traditional: {trad}, "
                f"dictionary traditional: {words[trad].traditional}, dictionary simplified: {words[trad].simplified}"
            )


def parse_readings(filename, words):
    for line in read_csv(filename):
        if len(line) != 3 or line[0].startswith("#"):
            continue

        codepoint = line[0]
        fieldname = line[1]
        content = line[2]

        if fieldname not in ("kCantonese", "kMandarin", "kDefinition"):
            continue

        character = chr(int(codepoint[2:], 16))
        # Skip simplified variants
        # For some reason, some characters, such as "𦲷", are neither traditional nor simplified
        # So we need to include them
        if not hanzidentifier.is_traditional(character) and not (
            (not hanzidentifier.is_traditional(character))
            and (not hanzidentifier.is_simplified(character))
        ):  # Skip simplified variants
            continue

        if character in words:
            if fieldname == "kCantonese":
                words[character].add_jyutping(content)
            elif fieldname == "kMandarin":
                pin = convert_pinyin_to_tone_numbers(content, character)
                words[character].add_pinyin(pin)
            elif fieldname == "kDefinition":
                words[character].add_defs([("", x.strip()) for x in content.split(";")])
        else:
            trad = character
            simp = HanziConv.toSimplified(trad)
            freq = zipf_frequency(trad, "zh")

            jyut = content if fieldname == "kCantonese" else ""
            pin = (
                convert_pinyin_to_tone_numbers(content, trad)
                if fieldname == "kMandarin"
                else ""
            )
            defs = (
                [("", x.strip()) for x in content.split(";")]
                if fieldname == "kDefinition"
                else []
            )

            entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=defs)
            words[character] = entry


if __name__ == "__main__":
    if len(sys.argv) != 12:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<Unihan_DictionaryLikeData.txt filepath> "
                "<Unihan_Readings.txt filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py unihan.db ./Unihan_DictionaryLikeData.txt "
                "./Unihan_Readings.txt Unihan UNI 2021-08-06 "
                '"The Unihan database is the repository for the Unicode Consortium’s '
                "collective knowledge regarding the CJK Unified Ideographs "
                "contained in the Unicode Standard. It contains mapping data to "
                "allow conversion to and from other coded character sets and additional "
                "information to help implement support for the various languages "
                'which use the Han ideographic script." '
                '"Copyright © 1991-2021 Unicode, Inc. All rights reserved. '
                "Distributed under the Terms of Use in https://www.unicode.org/copyright.html. "
                'See also https://www.unicode.org/license.html." '
                '"https://www.unicode.org/charts/unihan.html" "" "words"'
            )
        )
        sys.exit(1)

    source = objects.SourceTuple(
        sys.argv[4],
        sys.argv[5],
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10],
        sys.argv[11],
    )

    logging.getLogger().setLevel(logging.INFO)

    words = {}
    parse_readings(sys.argv[3], words)
    parse_dictionary_like_data(sys.argv[2], words)
    write(sys.argv[1], source, words)
