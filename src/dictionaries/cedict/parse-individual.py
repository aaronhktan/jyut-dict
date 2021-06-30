from wordfreq import zipf_frequency

from database import database, objects

import logging
import sqlite3
import sys


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
                    logging.error(f"Could not insert word {trad}, uh oh!")
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
    with open(filename, "r", encoding="utf8") as f:
        for index, line in enumerate(f):
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()
            trad = split[0]
            simp = split[1]
            pin = line[line.index("[") + 1 : line.index("]")].lower().replace("v", "u:")
            definitions = line[line.index("/") + 1 : -2].split("/")
            entry = objects.Entry(trad=trad, simp=simp, pin=pin, defs=definitions)

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
            "Usage: python3 script.py <database filename> <CC_CEDICT file> <Cantonese readings file> <source name> <source short name> <source version> <source description> <source legal> <source link> <source update url> <source other>"
        )
        print(
            'e.g. python3 script.py dict.db CEDICT.txt READINGS.txt CC-CEDICT CC 2018-07-09 "CC-CEDICT is a dictionary." "This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License." "http://www.mdbg.net/chindict/chindict.php?page=cc-cedict" "" ""'
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
