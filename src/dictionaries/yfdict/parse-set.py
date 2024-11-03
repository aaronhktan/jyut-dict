import pinyin_jyutping_sentence
import pycantonese
from wordfreq import zipf_frequency

from database import database, objects

import logging
import re
import sqlite3
import sys

HAN_REGEX = re.compile(r"[\u4e00-\u9fff]")

sources = [
    objects.SourceTuple(
        "CFDICT",
        "CF",
        "2023-04-08",
        "En 2010, David Houstin, fondateur de CHINE INFORMATIONS, a commencé à partager librement une partie de la base de données son dictionnaire français-chinois en ligne.",
        "Cette création est mise à disposition sous un contrat Creative Commons Paternité - Partage des Conditions Initiales à l'Identique.",
        "https://chine.in/mandarin/dictionnaire/CFDICT/",
        "https://chine.in/mandarin/dictionnaire/CFDICT/",
        "words",
    ),
    objects.SourceTuple(
        "YFDICT",
        "YF",
        "2024-11-02",
        "YFDICT est un dictionnaire cantonais-français créé par Aaron Tan.",
        "Cette création est mise à disposition sous un contrat Creative Commons Paternité - Partage des Conditions Initiales à l'Identique.",
        "http://cantonais.org/telecharger",
        "http://cantonais.org/telecharger",
        "words",
    ),
]


def write(entries, db_name):
    db = sqlite3.connect(db_name)
    c = db.cursor()

    # Set version of database
    database.write_database_version(c)

    # Delete old tables and indices, then create new one
    database.drop_tables(c)
    database.create_tables(c)

    # Add sources to table
    for source in sources:
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
                    logging.error(f"Could not insert word {trad}, uh oh!")
                    continue

            for definition in entry.cedict_english:
                definition_id = database.insert_definition(
                    c, definition, "", entry_id, 1, None
                )
                if definition_id == -1:
                    logging.error(
                        f"Could not insert definition {definition} for word {entry.traditional}, uh oh!"
                    )
                    continue

            for label, definition in entry.canto_english:
                definition_id = database.insert_definition(
                    c, definition, label, entry_id, 2, None
                )
                if definition_id == -1:
                    logging.error(
                        f"Could not insert definition {definition} for word {entry.traditional}, uh oh!"
                    )
                    continue


    database.generate_indices(c)

    db.commit()
    db.close()


def parse_cfdict(filename, entries):
    with open(filename, "r", encoding="utf8") as f:
        for index, line in enumerate(f):
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()
            trad = split[0]
            simp = split[1]
            pin = line[line.index("[") + 1 : line.index("]")].lower().replace("v", "u:")
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
            eng = line[line.index("/") + 1 : -2].split("/")
            entry = objects.EntryWithCantoneseAndMandarin(
                trad=trad, simp=simp, pin=pin, cedict_eng=eng
            )
            entry.add_fuzzy_jyutping(jyut)

            if trad in entries:
                entries[trad].append(entry)
            else:
                entries[trad] = [entry]


def parse_yfdict(filename, entries):
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
            if not split:
                continue
            if chinese_unsure or meaning_unsure or translation_unsure:
                split = split[1:]

            trad = split[0]
            simp = split[1]
            pin = line[line.index("[") + 1 : line.index("]")].lower().replace("v", "u:")
            jyut = line[line.index("{") + 1 : line.index("}")].lower()
            
            fra = line[line.find("/", line.index("}")) + 1 : line.rfind("/")]
            fra = fra.split("/")

            if chinese_unsure:
                fra = [("⚠ le traducteur n'a pas pu vérifier l'existence de cette locution en cantonais", x) for x in fra]
            elif meaning_unsure:
                fra = [("⚠ le traducteur n'a pas pu vérifier la définition de cette locution en cantonais", x) for x in fra]
            elif translation_unsure:
                fra = [("⚠ cette traduction contient des fautes grammaticales ou doit être reformulée pour mieux expliquer la définition", x) for x in fra]
            else:
                fra = [("", x) for x in fra]

            entry = entry = objects.EntryWithCantoneseAndMandarin(
                trad=trad, simp=simp, pin=pin, jyut=jyut, canto_eng=fra
            )

            # Check if entry is already in entries list
            # If it is, then add jyutping and Cantonese definition
            # Otherwise, add the new entry
            if trad in entries:
                added = False
                for existing_entry in entries[trad]:
                    # Check that simplified form is a match
                    # Check that pinyin is a match
                    # Do not overwrite existing jyutping
                    if (
                        existing_entry.simplified == simp
                        and pin != ""
                        and "".join(existing_entry.pinyin.split())
                        == "".join(pin.split())
                        and not existing_entry.jyutping
                    ):
                        existing_entry.add_jyutping(jyut)
                        existing_entry.canto_english = fra
                        added = True
                        break

                if not added:
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
    if len(sys.argv) != 5:
        print(
            (
                "Usage: python3 -m yfdict.parse-set <database filename> "
                "<CFDICT file> <YFDICT file> <Cantonese Readings file>"
            )
        )
        print(
            "e.g. python3 -m yfdict.parse-set yfdict/developer/yfdict.db "
            "cedict/data/CFDICT.txt cedict/data/YFDICT.txt cedict/data/READINGS.txt"
        )
        sys.exit(1)

    entries = {}
    parse_cfdict(sys.argv[2], entries)
    parse_yfdict(sys.argv[3], entries)
    parse_yfdict_canto_readings(sys.argv[4], entries)
    assign_frequencies(entries)
    write(entries, sys.argv[1])
