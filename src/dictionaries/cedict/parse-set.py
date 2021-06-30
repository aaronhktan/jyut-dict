from wordfreq import zipf_frequency

from database import database, objects

import logging
import sqlite3
import sys

sources = [
    objects.SourceTuple(
        "CC-CEDICT",
        "CC",
        "2018-07-09",
        "CC-CEDICT is a continuation of the CEDICT project started by Paul Denisowski in 1997 with the aim to provide a complete downloadable Chinese to English dictionary with pronunciation in pinyin for the Chinese characters.",
        "This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.",
        "http://www.mdbg.net/chindict/chindict.php?page=cc-cedict",
        "",
        "",
    ),
    objects.SourceTuple(
        "CC-CANTO",
        "CCY",
        "2016-01-15",
        "CC-Canto is an open-source Cantonese-to-English dictionary with about 22,000 entries, designed to be used alongside CC-CEDICT.",
        "This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 License.",
        "http://cantonese.org/download.html",
        "",
        "",
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

            for definition in entry.canto_english:
                definition_id = database.insert_definition(
                    c, definition, "", entry_id, 2, None
                )
                if definition_id == -1:
                    logging.error(
                        f"Could not insert definition {definition} for word {entry.traditional}, uh oh!"
                    )
                    continue

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_cc_cedict(filename, entries):
    with open(filename, "r", encoding="utf8") as f:
        for index, line in enumerate(f):
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()
            trad = split[0]
            simp = split[1]
            pin = line[line.index("[") + 1 : line.index("]")].lower().replace("v", "u:")
            eng = line[line.index("/") + 1 : -2].split("/")
            entry = objects.EntryWithCantoneseAndMandarin(trad=trad, simp=simp, pin=pin, cedict_eng=eng)

            if trad in entries:
                entries[trad].append(entry)
            else:
                entries[trad] = [entry]


def parse_cc_canto(filename, entries):
    with open(filename, "r", encoding="utf8") as f:
        for line in f:
            if len(line) == 0 or line[0] == "#":
                continue

            split = line.split()  # Splits by whitespace
            trad = split[0]
            simp = split[1]
            pin = line[line.index("[") + 1 : line.index("]")].lower().replace("v", "u:")
            jyut = line[line.index("{") + 1 : line.index("}")].lower()
            eng = line[line.find("/", line.index("}")) + 1 : line.rfind("/")]
            entry = entry = objects.EntryWithCantoneseAndMandarin(trad=trad, simp=simp, pin=pin, jyut=jyut, canto_eng=[eng])

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
                        existing_entry.add_canto_eng(eng)
                        added = True
                        break

                if not added:
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
            "Usage: python3 script.py <database filename> <CC_CEDICT file> <CC_CANTO file> <Cantonese Readings file>"
        )
        print("e.g. python3 script.py eng.db CC-CEDICT.txt CC-CANTO.txt READINGS.txt")
        sys.exit(1)

    entries = {}
    parse_cc_cedict(sys.argv[2], entries)
    parse_cc_canto(sys.argv[3], entries)
    parse_cc_cedict_canto_readings(sys.argv[4], entries)
    assign_frequencies(entries)
    write(entries, sys.argv[1])
