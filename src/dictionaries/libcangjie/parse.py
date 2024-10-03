import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict
import csv
import logging
import sqlite3
import sys


def read_csv(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile, delimiter=" ")
        for row in reader:
            yield row


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


def parse_cangjie(filename, words):
    for line in read_csv(filename):
        if len(line) != 15 or line[0].startswith("#"):
            continue

        traditional = line[0]
        simplified = traditional if line[1] == "NA" else line[1]
        pinyin = (
            " ".join(
                lazy_pinyin(
                    traditional,
                    style=Style.TONE3,
                    neutral_tone_with_five=True,
                    v_to_u=True,
                )
            )
            .lower()
            .replace("ü", "u:")
        )
        jyutping = pinyin_jyutping_sentence.jyutping(
            traditional, tone_numbers=True, spaces=True
        )
        freq = zipf_frequency(traditional, "zh")

        cangjie_v3 = line[11].split(",")
        cangjie_v5 = line[12].split(",")

        entry = objects.Entry(traditional, simplified, pinyin, jyutping, freq)

        for input_code in cangjie_v3:
            if input_code != "NA":
                entry.append_to_defs(("Cangjie v3 Input", input_code))
        for input_code in cangjie_v5:
            if input_code != "NA":
                entry.append_to_defs(("Cangjie v5 Input", input_code))

        words[entry.traditional].append(entry)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 -m unihan.parse <database filename> "
                "<Cangjie data filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m libcangjie.parse libcangjie/developer/cangjie.db "
                "libcangjie/data/table.txt "
                "libcangjie LCJ 2024-07-05 "
                '"libcangjie is a library implementing the Cangjie method. '
                "It handles all the mappings between the input codes and the desired "
                'characters, and provides some filtering options." '
                '"The data in the table.txt file is hereby placed under the '
                'Public Domain, in the hope that is is useful." '
                '"https://cangjie.pages.freedesktop.org/" "" "words"'
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

    logging.getLogger().setLevel(logging.INFO)

    words = defaultdict(list)
    parse_cangjie(sys.argv[2], words)
    write(sys.argv[1], source, words)
