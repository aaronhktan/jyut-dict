import pinyin_jyutping_sentence
import pycantonese
from wordfreq import zipf_frequency

from database import database, objects

import logging
import re
import sqlite3
import sys

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


def parse_file(filename, entries):
    with open(filename, "r", encoding="utf8") as f:
        for index, line in enumerate(f):
            if len(line) == 0 or line[0] == "#":
                continue

            chinese_unsure, meaning_unsure, translation_unsure = False, False, False
            if line.startswith("????c"):
                chinese_unsure = True
            elif line.startswith("????m"):
                meaning_unsure = True
            elif line.startswith("????f"):
                translation_unsure = True

            split = line.split()
            if not split:
                continue
            if chinese_unsure or meaning_unsure or translation_unsure:
                split = split[1:]

            trad = split[0]
            simp = split[1]
            pin = line[line.index("[") + 1 : line.index("]")].lower().replace("v", "u:")

            has_jyut = False
            if "{" in line:
                jyut = line[line.index("{") + 1 : line.index("}")].lower()
                has_jyut = True
            else:
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

            definitions = line[line.index("/") + 1 : -2].split("/")

            if chinese_unsure:
                definitions = [("⚠ le traducteur n'a pas pu vérifier l'existence de cette locution en cantonais", x) for x in definitions]
            elif meaning_unsure:
                definitions = [("⚠ le traducteur n'a pas pu vérifier la définition de cette locution en cantonais", x) for x in definitions]
            elif translation_unsure:
                definitions = [("⚠ cette traduction contient des fautes grammaticales ou doit être reformulée pour mieux expliquer la définition", x) for x in definitions]
            else:
                definitions = [("", x) for x in definitions]

            entry = objects.Entry(trad=trad, simp=simp, pin=pin, defs=definitions)
            if has_jyut:
                entry.add_jyutping(jyut)
            else:
                entry.add_fuzzy_jyutping(jyut)

            if trad in entries:
                entries[trad].append(entry)
            else:
                entries[trad] = [entry]


def parse_yfdict_canto_readings(filename, entries):
    with open(filename, "r", encoding="utf8") as f:
        for line in f:
            if len(line) == 0 or line[0] == "#":
                continue

            chinese_unsure, meaning_unsure, translation_unsure = False, False, False
            if line.startswith("????c"):
                chinese_unsure = True
            elif line.startswith("????m"):
                meaning_unsure = True
            elif line.startswith("????f"):
                translation_unsure = True


            split = line.split()
            if chinese_unsure or meaning_unsure or translation_unsure:
                split = split[1:]

            trad = split[0]
            simp = split[1]
            pin = "".join(line[line.index("[") + 1 : line.index("]")].lower().split())
            jyut = line[line.index("{") + 1 : line.index("}")].lower()
            if trad not in entries:
                continue

            # Add jyutping pronunciation if traditional, simplified, and pinyin match
            # And jyutping pronunciation has not already been added
            for existing_entry in entries[trad]:
                # If it's an exact match, then set jyutping
                if (
                    existing_entry.simplified == simp
                    and pin != ""
                    and "".join(existing_entry.pinyin.split()) == "".join(pin.split())
                    and not existing_entry.jyutping
                ):
                    existing_entry.add_jyutping(jyut)
                # Otherwise, add as fuzzy
                else:
                    existing_entry.add_fuzzy_jyutping(jyut)


def assign_frequencies(entries):
    for key in entries:
        for entry in entries[key]:
            freq = zipf_frequency(entry.traditional, "zh")
            entry.add_freq(freq)


if __name__ == "__main__":
    if len(sys.argv) != 12:
        print(
            (
                "Usage: python3 -m yfdict.parse-individual <database filename> "
                "<CC_CEDICT file> <Cantonese readings file> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m yfdict.parse-individual yfdict/developer/dict.db "
                "yfdict/data/YFDICT.txt yfdict/data/READINGS.txt "
                'YFDICT YF 2024-11-02 "YFDICT est un dictionnaire cantonais-français créé par Aaron Tan." '
                '"Cette création est mise à disposition sous un contrat Creative Commons Paternité - Partage des Conditions Initiales à l\'Identique." '
                '"http://cantonais.org/telecharger" "http://cantonais.org/telecharger" "words"'
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
    parse_yfdict_canto_readings(sys.argv[3], entries)
    assign_frequencies(entries)
    write(sys.argv[1], source, entries)
