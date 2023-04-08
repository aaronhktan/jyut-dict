from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict
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
        for entry in entries[key]:
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


def parse_variants(filename, words_traditional, words_simplified):
    variant_pairs = set()

    for line in read_csv(filename):
        if len(line) != 3 or line[0].startswith("#"):
            continue

        codepoint = line[0]
        fieldname = line[1]
        content = line[2]

        if fieldname not in ("kSimplifiedVariant", "kTraditionalVariant"):
            continue

        character = chr(int(codepoint[2:], 16))
        variants = [chr(int(item[2:], 16)) for item in content.split()]

        if fieldname == "kTraditionalVariant":
            for variant in variants:
                if (variant, character) not in variant_pairs:
                    # This character has a traditional variant - insert into words
                    entry = objects.Entry(variant, character, "", "")
                    words_traditional[variant].append(entry)
                    words_simplified[character].append(entry)
                    variant_pairs.add((variant, character))
        elif fieldname == "kSimplifiedVariant":
            for variant in variants:
                if (character, variant) not in variant_pairs:
                    # This character has a simplified variant - insert into words
                    entry = objects.Entry(character, variant, "", "")
                    words_traditional[character].append(entry)
                    words_simplified[variant].append(entry)
                    variant_pairs.add((character, variant))


def parse_readings(filename, words_traditional, words_simplified):
    for line in read_csv(filename):
        if len(line) != 3 or line[0].startswith("#"):
            continue

        codepoint = line[0]
        fieldname = line[1]
        content = line[2]

        if fieldname not in ("kCantonese", "kMandarin", "kDefinition"):
            continue

        character = chr(int(codepoint[2:], 16))

        entry_added = False

        if character in words_traditional:
            freq = zipf_frequency(character, "zh")

            for entry in words_traditional[character]:
                entry.add_freq(freq)
                if fieldname == "kCantonese":
                    entry.add_jyutping(content)
                elif fieldname == "kMandarin":
                    pin = convert_pinyin_to_tone_numbers(content, character)
                    entry.add_pinyin(pin)
                elif fieldname == "kDefinition":
                    entry.add_defs([("", x.strip()) for x in content.split(";")])

            entry_added = True

        if character in words_simplified:
            # Ignore simplified characters
            entry_added = True

        if not entry_added:
            trad = simp = character
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
            words_traditional[trad].append(entry)
            words_simplified[simp].append(entry)


def parse_dictionary_like_data(filename, words_traditional, words_simplified):
    for line in read_csv(filename):
        if len(line) != 3 or line[0].startswith("#"):
            continue

        codepoint = line[0]
        fieldname = line[1]
        content = line[2]

        if fieldname != "kCangjie":
            continue

        character = chr(int(codepoint[2:], 16))

        if character not in words_traditional and character not in words_simplified:
            logging.warning(
                f"{character} has Cangjie but no definitions or pronunciation - "
                f"please verify that this is right at https://www.unicode.org/cgi-bin/GetUnihanData.pl?codepoint={character}"
            )
            continue

        cangjie_added = False
        for entry in words_traditional[character]:
            if entry.traditional == entry.simplified:
                if any(label == "Cangjie Input" for (label, _) in entry.definitions):
                    # Don't add Cangjie twice!
                    continue
                entry.append_to_defs(("Cangjie Input", content))
                cangjie_added = True
            elif entry.traditional == character:
                # Sometimes, the Cangjie input code is the same for both traditional and simplified
                # even if the characters are different (see "齷")
                # So we re-label it to apply to both forms of the character
                inserted = False
                for i in range(len(entry.definitions)):
                    if (
                        entry.definitions[i][0].startswith("Cangjie Input -")
                        and entry.definitions[i][1] == content
                    ):
                        entry.definitions[i] = ("Cangjie Input", content)
                        inserted = True
                        break
                if not inserted:
                    entry.append_to_defs(("Cangjie Input - Traditional", content))
                cangjie_added = True

        for entry in words_simplified[character]:
            if entry.traditional == entry.simplified:
                # Assume that the traditional block above has handled inserting Cangjie for traditional == simplified
                continue
            elif entry.simplified == character:
                # Sometimes, the Cangjie input code is the same for both traditional and simplified
                # even if the characters are different (see "齷")
                # So we re-label it to apply to both forms of the character
                inserted = False
                for i in range(len(entry.definitions)):
                    if (
                        entry.definitions[i][0].startswith("Cangjie Input -")
                        and entry.definitions[i][1] == content
                    ):
                        entry.definitions[i] = ("Cangjie Input", content)
                        inserted = True
                        break
                if not inserted:
                    entry.append_to_defs(("Cangjie Input - Simplified", content))
                cangjie_added = True

        if not cangjie_added:
            logging.warning(
                f"The character {character} matches neither traditional nor simplified forms of the dictionary word: "
                f"[character: {character}, traditional: {trad}, "
                f"dictionary traditional: {words[trad].traditional}, dictionary simplified: {words[trad].simplified}"
            )


if __name__ == "__main__":
    if len(sys.argv) != 13:
        print(
            (
                "Usage: python3 -m unihan.parse <database filename> "
                "<Unihan_DictionaryLikeData.txt filepath> "
                "<Unihan_Readings.txt filepath> "
                "<Unihan_Variants.txt filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m unihan.parse unihan/developer/unihan.db "
                "unihan/data/Unihan_DictionaryLikeData.txt "
                "unihan/data/Unihan_Readings.txt unihan/data/Unihan_Variants.txt Unihan UNI 2021-08-06 "
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
        sys.argv[5],
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10],
        sys.argv[11],
        sys.argv[12],
    )

    logging.getLogger().setLevel(logging.INFO)

    words_traditional = defaultdict(list)
    words_simplified = defaultdict(list)
    parse_variants(sys.argv[4], words_traditional, words_simplified)
    parse_readings(sys.argv[3], words_traditional, words_simplified)
    parse_dictionary_like_data(sys.argv[2], words_traditional, words_simplified)
    write(sys.argv[1], source, words_traditional)
