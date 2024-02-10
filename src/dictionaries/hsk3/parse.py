import opencc
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict
from itertools import product
import logging
import sqlite3
import sys

lists = (
    "词汇表",
    "汉字表",
    "手写字表",
)

converter = opencc.OpenCC("s2hk.json")


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


def parse_file(filename, entries):
    with open(filename, "r", encoding="utf8") as f:
        level = ""

        for index, line in enumerate(f):
            line = line.strip()

            if len(line) == 0 or line[0] == "#":
                continue

            if line.endswith(lists):
                level = line
                continue

            split = line.split()

            entry_number = split[0]
            try:
                simp = split[1][: split[1].index("（")]
                label = split[1][split[1].index("（") + 1 : split[1].index("）")]
            except:
                simp = split[1]
                label = ""

            trad = converter.convert(simp)
            pin = (
                " ".join(
                    lazy_pinyin(
                        simp,
                        style=Style.TONE3,
                        neutral_tone_with_five=True,
                        v_to_u=True,
                    )
                )
                .lower()
                .replace("ü", "u:")
            )
            jyut = pinyin_jyutping_sentence.jyutping(
                trad, tone_numbers=True, spaces=True
            )

            freq = zipf_frequency(trad, "zh")

            definitions = [(label, f"HSK3 {level} #{entry_number}")]

            entry = objects.Entry(
                trad=trad, simp=simp, pin=pin, jyut=jyut, freq=freq, defs=definitions
            )
            entries[trad].append(entry)


if __name__ == "__main__":
    if len(sys.argv) != 12:
        print(
            (
                "Usage: python3 -m hsk3.parse <database filename> "
                "<HSK3 wordlist.txt filepath> <HSK3 charlist.txt filepath>"
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m hsk3.parse hsk3/developer/hks3.db hsk3/data/wordlist.txt "
                "hsk3/data/charlist.txt HSK3.0 HSK3 2021-07-01 "
                '"本规范规定了中文作为第二语言的学习 者在生活、学习、'
                "工作等领域运用中文完成交际的语言水平等级。本规范适用于国际中文教育的学习、"
                '教学、测试与评估，并为其提供参考。" '
                '"MIT License\n\nCopyright (c) 2021 Pleco Inc.\n\n'
                "Permission is hereby granted, free of charge, to any person obtaining a copy "
                'of this software and associated documentation files (the \\"Software\\"), to deal '
                "in the Software without restriction, including without limitation the rights "
                "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
                "copies of the Software, and to permit persons to whom the Software is "
                "furnished to do so, subject to the following conditions:\n\n"
                "The above copyright notice and this permission notice shall be included in all "
                "copies or substantial portions of the Software.\n\n"
                'THE SOFTWARE IS PROVIDED \\"AS IS\\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR '
                "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
                "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
                "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
                "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
                "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
                'SOFTWARE." '
                '"https://github.com/elkmovie/hsk30" "" "words"'
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

    words = defaultdict(list)
    parse_file(sys.argv[2], words)
    parse_file(sys.argv[3], words)
    write(sys.argv[1], source, words)
