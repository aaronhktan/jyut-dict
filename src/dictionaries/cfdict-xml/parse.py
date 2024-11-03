import pinyin_jyutping_sentence
import pycantonese
from wordfreq import zipf_frequency

from database import database, objects

import logging
import re
import sqlite3
import sys
import xml.etree.ElementTree as ET

HAN_REGEX = re.compile(r"[\u4e00-\u9fff]")


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
                entry.jyutping if entry.jyutping != "" else entry.fuzzy_jyutping,
                entry.freq,
            )

            if entry_id == -1:
                entry_id = database.insert_entry(
                    c,
                    entry.traditional,
                    entry.simplified,
                    entry.pinyin,
                    entry.jyutping if entry.jyutping != "" else entry.fuzzy_jyutping,
                    entry.freq,
                )
                if entry_id == -1:
                    logging.error(f"Could not insert word {entry.traditional}, uh oh!")
                    continue

            for definition in entry.definitions:
                definition_id = database.insert_definition(
                    c, definition, "", entry_id, 1, None
                )
                if definition_id == -1:
                    logging.error(
                        f"Could not insert definition {definition} for word {entry.traditional}, uh oh!"
                    )
                    continue

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_file(filename, entries):
    tree = ET.parse(filename)
    root = tree.getroot()
    for word in root:
        definitions = []
        for attrib in word:
            match attrib.tag:
                case "trad":
                    trad = attrib.text
                case "simp":
                    simp = attrib.text
                case "py":
                    pin = attrib.text.lower()
                case "trans":
                    for translation in attrib:
                        definitions.append(translation.text)
                case "id" | "upd":
                    pass
                case other:
                    print(f"another attrib found, tag: {attrib.tag}")
        entry = objects.Entry(trad=trad, simp=simp, pin=pin, defs=definitions)

        jyut = pinyin_jyutping_sentence.jyutping(
            trad,
            tone_numbers=True,
            spaces=True,
        )

        # If pinyin_jyutping_sentences cannot convert to Jyutping, use
        # pycantonese to convert the character instead
        han_chars = HAN_REGEX.findall(jyut)
        for char in han_chars:
            char_jyutping = pycantonese.characters_to_jyutping(char)[0][1]
            if char_jyutping:
                jyut = jyut.replace(char, char_jyutping)

        entry.add_fuzzy_jyutping(jyut)

        if trad in entries:
            entries[trad].append(entry)
        else:
            entries[trad] = [entry]


def parse_cc_cedict_canto_readings(filename, entries):
    with open(filename, "r", encoding="utf8") as f:
        for line in f:
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()
            trad = split[0]
            simp = split[1]
            pin = "".join(
                line[line.index("[") + 1 : line.index("]")].lower().split()
            ).replace("v", "u:")
            jyut = line[line.index("{") + 1 : line.index("}")].lower()
            if trad not in entries:
                continue

            for entry in entries[trad]:
                # If it's an exact match, then set jyutping
                if entry.simplified == simp and "".join(entry.pinyin.split()) == pin:
                    entry.add_jyutping(jyut)
                # Otherwise, add as fuzzy
                else:
                    entry.add_fuzzy_jyutping(jyut)


def assign_frequencies(entries):
    for key in entries:
        for entry in entries[key]:
            freq = zipf_frequency(entry.traditional, "zh")
            entry.add_freq(freq)


if __name__ == "__main__":
    if len(sys.argv) != 12:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<CFDICT.xml file> <Cantonese readings file> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source contents>"
            )
        )
        print(
            (
                "e.g. python3 -m cfdict-xml.parse cfdict-xml/developer/dict.db "
                "cfdict-xml/data/CFDICT.xml cfdict-xml/data/READINGS.txt "
                'CFDICT-2016 CF2016 2016-01-17 "En 2010, David Houstin, '
                "fondateur de CHINE INFORMATIONS, a commencé à partager "
                "librement une partie de la base de données son dictionnaire "
                'français-chinois en ligne." '
                '"Cette création est mise à disposition sous un contrat '
                "Creative Commons Paternité - Partage des Conditions Initiales "
                "à l'Identique.\" "
                '"https://chine.in/mandarin/dictionnaire/CFDICT/" '
                '"https://chine.in/mandarin/dictionnaire/CFDICT/" "words"'
            )
        )
        sys.exit(1)

    entries = {}
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
    parse_file(sys.argv[2], entries)
    parse_cc_cedict_canto_readings(sys.argv[3], entries)
    assign_frequencies(entries)
    write(sys.argv[1], source, entries)
