from wordfreq import zipf_frequency

import sqlite3
import sys

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(
            "Usage: python3 script.py <output database name> <database 1 filename> <database 2 filename>"
        )
        print("e.g. python3 script.py dict-merged.db dict.db dict-fr.db")
        sys.exit(1)

    db = sqlite3.connect(sys.argv[1])
    c = db.cursor()

    # Set version of database
    c.execute("PRAGMA user_version=1")

    # Attach new databases
    c.execute("ATTACH DATABASE '{}' AS db1".format(sys.argv[2]))
    c.execute("ATTACH DATABASE '{}' AS db2".format(sys.argv[3]))

    # Delete old tables and indices
    c.execute("DROP TABLE IF EXISTS entries")
    c.execute("DROP TABLE IF EXISTS entries_fts")
    c.execute("DROP TABLE IF EXISTS sources")
    c.execute("DROP TABLE IF EXISTS definitions")
    c.execute("DROP TABLE IF EXISTS definitions_fts")
    c.execute("DROP INDEX IF EXISTS fk_entry_id_index")

    # Create new tables
    c.execute(
        """CREATE TABLE entries(
                    entry_id INTEGER PRIMARY KEY,
                    traditional TEXT,
                    simplified TEXT,
                    pinyin TEXT,
                    jyutping TEXT,
                    frequency REAL,
                    UNIQUE(traditional, simplified, pinyin, jyutping) ON CONFLICT IGNORE
                )"""
    )
    c.execute("CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)")

    c.execute(
        """CREATE TABLE sources(
                    source_id INTEGER PRIMARY KEY,
                    sourcename TEXT UNIQUE ON CONFLICT ABORT,
                    sourceshortname TEXT,
                    version TEXT,
                    description TEXT,
                    legal TEXT,
                    link TEXT,
                    update_url TEXT,
                    other TEXT
                )"""
    )

    c.execute(
        """CREATE TABLE definitions(
                    definition_id INTEGER PRIMARY KEY,
                    definition TEXT,
                    fk_entry_id INTEGER,
                    fk_source_id INTEGER,
                    FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON UPDATE CASCADE,
                    FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE,
                    UNIQUE(definition, fk_entry_id, fk_source_id) ON CONFLICT IGNORE
                )"""
    )
    c.execute("CREATE VIRTUAL TABLE definitions_fts using fts5(definition)")

    # Insert from first database
    c.execute(
        "INSERT INTO entries(traditional, simplified, pinyin, jyutping, frequency) SELECT traditional, simplified, pinyin, jyutping, frequency FROM db1.entries"
    )
    c.execute(
        "INSERT INTO sources(sourcename, sourceshortname, version, description, legal, link, update_url, other) SELECT sourcename, sourceshortname, version, description, legal, link, update_url, other FROM db1.sources"
    )
    c.execute(
        "INSERT INTO definitions(definition, fk_entry_id, fk_source_id) SELECT definition, fk_entry_id, fk_source_id FROM db1.definitions"
    )

    # Insert from second database
    c.execute(
        "INSERT INTO entries(traditional, simplified, pinyin, jyutping, frequency) SELECT traditional, simplified, pinyin, jyutping, frequency FROM db2.entries"
    )
    c.execute(
        "INSERT INTO sources(sourcename, sourceshortname, version, description, legal, link, update_url, other) SELECT sourcename, sourceshortname, version, description, legal, link, update_url, other FROM db2.sources"
    )

    c.execute(
        """CREATE TEMPORARY TABLE definitions_tmp AS
                    SELECT entries.traditional AS traditional, entries.simplified AS simplified, entries.pinyin AS pinyin, entries.jyutping AS jyutping,
                        sources.sourcename AS sourcename, definitions.definition AS definition
                    FROM db2.entries, db2.definitions, db2.sources
                    WHERE db2.definitions.fk_entry_id = db2.entries.entry_id AND db2.definitions.fk_source_id = db2.sources.source_id
              """
    )

    c.execute(
        """INSERT INTO definitions(definition, fk_entry_id, fk_source_id)
                SELECT d.definition, e.entry_id, s.source_id
                FROM definitions_tmp AS d, sources AS s, entries AS e
                WHERE d.sourcename = s.sourcename
                    AND d.traditional = e.traditional
                    AND d.simplified = e.simplified
                    AND d.pinyin = e.pinyin
                    AND d.jyutping = e.jyutping
              """
    )

    # Populate FTS versions of tables
    c.execute(
        "INSERT INTO entries_fts(rowid, pinyin, jyutping) SELECT rowid, pinyin, jyutping FROM entries"
    )
    c.execute(
        "INSERT INTO definitions_fts(rowid, definition) SELECT rowid, definition FROM definitions"
    )

    # Create index
    c.execute("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)")

    db.commit()
    db.close()
