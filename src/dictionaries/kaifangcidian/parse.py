import hanzidentifier
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

import ast
import csv
import logging
import sqlite3
import sys


def write(entries, db_name):
    db = sqlite3.connect(db_name)
    c = db.cursor()

    database.write_database_version(c)
    database.drop_tables(c)
    database.create_tables(c)
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

    entry_id = 0
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


def parse_file(filename_traditional, filename_simplified_jyutping, entries):
    simplified = traditional = []
    with open(filename_traditional, "r", encoding="utf8") as f:
        reader = csv.reader(f, delimiter="	")
        traditional = list(reader)

    # The Kaifangcidian data for jyutping is horrible.
    # The entire data is on a single line, printed like a flat Python list.
    # The entry may be a single item in the array, or multiple items.
    # The Jyutping pronunciation is a separate item for each character in the entry.
    # The translations to Mandarin may, or may not follow the Jyutping!
    # And there is no separator between data for different entries :)
    last_line = ""
    with open(filename_simplified_jyutping, "r", encoding="utf8") as f:
        last_line = f.readlines()[-1]
    simplified = ast.literal_eval(last_line)

    index = 0
    for row in range(len(traditional)):
        if row < 9:  # The first nine rows are comments and headers
            continue

        trad = traditional[row][0]

        # Horrible data workaround 1:
        # In KFCD Jyutping data, when the entry has Chinese characters in it,
        # the entry is presented as a single string in the array. (This is sane.)
        # If it does not (e.g. the word 'pat pat'), each series of characters, delineated
        # by a space, is a separate entry in the array ('pat pat' => ["pat", "pat"])
        trad_len = len(trad.split(" "))
        if not hanzidentifier.has_chinese(trad):
            simp = "".join(simplified[index : index + trad_len])
        else:
            simp = simplified[index]

        # Horrible data workaround 2:
        # In KFCD Jyutping data, the Jyutping for each word in an entry
        # is presented as a separate string.
        # To find the indices that correspond to the entry we just extracted,
        # use the data from the KFCD Yale edition (which is formatted as a CSV) to
        # determine how many items comprise the Jyutping pronunciation.
        # One cannot use the string length of the entry, as it may contain punctuation
        # (e.g. '，') that has no corresponding Jyutping syllable, AND the entry
        # may be split up into multiple items (as described in horrible
        # workaround #1).
        jyut_len = len(traditional[row][1].split(" "))
        jyut = " ".join(simplified[index + trad_len : index + trad_len + jyut_len])

        pin = (
            " ".join(lazy_pinyin(trad, style=Style.TONE3, neutral_tone_with_five=True))
            .lower()
            .replace("v", "u:")
        )

        # Horrible data workaround 3:
        # In the KFCD Yale data, all the definitions are listed as a single item, separated
        # by the wide-character '，'. Some entries have definitions, and some do not.
        # In the KFCD Jyutping edition, the definitions are also listed all as a single item.
        # However, many words do not have definitions; if there are no definitions then
        # we do NOT need to advance the index by 1 more item (which would have been
        # the definitions).
        if traditional[row][2]:
            defs_traditional = traditional[row][2].split("，")
            defs_simplified = simplified[index + trad_len + jyut_len].split("，")
            definitions = []
            for (def_traditional, def_simplified) in zip(
                defs_traditional, defs_simplified
            ):
                if def_traditional != def_simplified:
                    definitions.append(def_traditional + " – " + def_simplified)
                else:
                    definitions.append(def_traditional)
            index += trad_len + jyut_len + 1
        else:
            definitions = ["（沒有對應漢語詞彙）"]
            index += trad_len + jyut_len

        entry = objects.Entry(
            trad=trad, simp=simp, pin=pin, jyut=jyut, defs=definitions
        )

        if trad in entries:
            entries[trad].append(entry)
        else:
            entries[trad] = [entry]


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
                "<Kaifangcidian traditional + Yale file> "
                "<Kaifangcidian simplified + Jyutping file> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py kfcd.db cidian_zhyue-ft-kfcd-ylshu-2019623.txt "
                "cidian_zhyue-jt-kfcd-yp-2019623.txt Kaifangcidian KFCD 2019-06-23 "
                '"Kaifangcidian is a dictionary" '
                '"本词典以创作共用“署名 3.0”许可协议授权发布（详见 http://creativecommons.org/licenses/by/3.0/）" '
                '"http://www.kaifangcidian.com/han/yue" "" ""'
            )
        )
        sys.exit(1)

    cc_cedict.load()

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
    parse_file(sys.argv[2], sys.argv[3], entries)
    assign_frequencies(entries)
    write(entries, sys.argv[1])
