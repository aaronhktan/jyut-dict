def create_tables(c):
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
                  label TEXT,
                  fk_entry_id INTEGER,
                  fk_source_id INTEGER,
                  FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON UPDATE CASCADE,
                  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE,
                  UNIQUE(definition, label, fk_entry_id, fk_source_id) ON CONFLICT IGNORE
            )"""
    )
    c.execute(
        "CREATE VIRTUAL TABLE definitions_fts using fts5(fk_entry_id UNINDEXED, definition)"
    )

    c.execute(
        """CREATE TABLE examples(
                  example_id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
                  traditional TEXT,
                  simplified TEXT,
                  pinyin TEXT,
                  jyutping TEXT,
                  language TEXT,
                  UNIQUE(traditional, simplified, pinyin, jyutping, language) ON CONFLICT IGNORE
            )"""
    )

    c.execute(
        """CREATE TABLE example_translations(
                  example_translation_id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
                  translation TEXT,
                  language TEXT,
                  UNIQUE(example_translation_id, translation) ON CONFLICT IGNORE
            )"""
    )

    c.execute(
        """CREATE TABLE example_links(
                  fk_example_id INTEGER,
                  fk_example_translation_id INTEGER,
                  fk_source_id INTEGER,
                  direct BOOLEAN,
                  FOREIGN KEY(fk_example_id) REFERENCES examples(example_id),
                  FOREIGN KEY(fk_example_translation_id) REFERENCES example_translations(example_translation_id),
                  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE
                  UNIQUE(fk_example_id, fk_example_translation_id) ON CONFLICT IGNORE
            )"""
    )

    c.execute(
        """CREATE TABLE definitions_examples_links(
                  fk_definition_id INTEGER,
                  fk_example_id INTEGER,
                  FOREIGN KEY(fk_definition_id) REFERENCES definitions(definition_id) ON DELETE CASCADE,
                  FOREIGN KEY(fk_example_id) REFERENCES examples(example_id)
                  UNIQUE(fk_definition_id, fk_example_id) ON CONFLICT IGNORE
            )"""
    )


def drop_tables(c):
    c.execute("DROP TABLE IF EXISTS entries")
    c.execute("DROP TABLE IF EXISTS entries_fts")
    c.execute("DROP TABLE IF EXISTS sources")
    c.execute("DROP TABLE IF EXISTS definitions")
    c.execute("DROP TABLE IF EXISTS definitions_fts")
    c.execute("DROP INDEX IF EXISTS fk_entry_id_index")

    c.execute("DROP TABLE IF EXISTS examples")
    c.execute("DROP TABLE IF EXISTS example_translations")
    c.execute("DROP TABLE IF EXISTS example_links")
    c.execute("DROP INDEX IF EXISTS fk_example_id_index")
    c.execute("DROP INDEX IF EXISTS fk_example_translation_id_index")

    c.execute("DROP TABLE IF EXISTS definitions_examples_links")


def write_database_version(c):
    c.execute("PRAGMA user_version=4")


def generate_indices(c):
    c.execute(
        "INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT rowid, pinyin, jyutping FROM entries"
    )
    c.execute(
        "INSERT INTO definitions_fts (rowid, fk_entry_id, definition) select rowid, fk_entry_id, definition FROM definitions"
    )

    c.execute("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)")
    c.execute("CREATE INDEX entries_simplified_idx ON entries(simplified)")
    c.execute("CREATE INDEX entries_jyutping_idx ON entries(jyutping)")
    c.execute("CREATE INDEX entries_pinyin_idx ON entries(pinyin)")
    c.execute(
        "CREATE INDEX del_fk_example_idx ON definitions_examples_links(fk_example_id);"
    )
    c.execute(
        "CREATE INDEX example_links_fk_example_translation_idx ON example_links(fk_example_translation_id);"
    )


def insert_source(
    c, name, shortname, version, description, legal, link, update_url, other, id_=None
):
    c.execute(
        "INSERT INTO sources values(?,?,?,?,?,?,?,?,?)",
        (id_, name, shortname, version, description, legal, link, update_url, other),
    )


def insert_entry(c, trad, simp, pin, jyut, freq, id_=None):
    c.execute("SELECT max(rowid) FROM entries")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO entries values (?,?,?,?,?,?)", (id_, trad, simp, pin, jyut, freq)
    )

    c.execute("SELECT max(rowid) FROM entries")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted an entry
    if after_id != before_id:
        return after_id
    return -1


def insert_definition(c, definition, label, entry_id, source_id, id_=None):
    c.execute("SELECT max(rowid) FROM definitions")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO definitions values (?,?,?,?,?)",
        (id_, definition, label, entry_id, source_id),
    )

    c.execute("SELECT max(rowid) FROM definitions")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted a definition
    if after_id != before_id:
        return after_id
    return -1


def insert_definition_example_link(c, definition_id, example_id):
    c.execute("SELECT max(rowid) FROM definitions_examples_links")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO definitions_examples_links values (?,?)",
        (definition_id, example_id),
    )

    c.execute("SELECT max(rowid) FROM definitions_examples_links")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted a definition<->example link
    if after_id != before_id:
        return after_id
    return -1


def insert_example(c, trad, simp, pin, jyut, lang, id_=None):
    c.execute("SELECT max(rowid) FROM examples")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO examples values (?,?,?,?,?,?)",
        (id_, trad, simp, pin, jyut, lang),
    )

    c.execute("SELECT max(rowid) FROM examples")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted an example
    if after_id != before_id:
        return after_id
    return -1


def insert_translation(c, translation, lang, id_=None):
    c.execute("SELECT max(rowid) FROM example_translations")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO example_translations values (?,?,?)",
        (id_, translation, lang),
    )

    c.execute("SELECT max(rowid) FROM example_translations")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted a translation
    if after_id != before_id:
        return after_id
    return -1


def insert_example_link(c, example_id, translation_id, source_id, direct):
    c.execute("SELECT max(rowid) FROM example_links")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO example_links values (?,?,?,?)",
        (example_id, translation_id, source_id, direct),
    )

    c.execute("SELECT max(rowid) FROM example_links")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted an example<->translation link
    if after_id != before_id:
        return after_id
    return -1


def get_source_id(sourcename):
    c.execute(
        """SELECT rowid FROM sources WHERE sourcename=?)""",
        (sourcename),
    )
    row = c.fetchone()
    if row is None:
        return -1
    return row[0]


def get_entry_id(c, trad, simp, pin, jyut, freq):
    c.execute(
        """SELECT rowid FROM entries WHERE traditional=?
            AND simplified=? AND pinyin=? AND jyutping=? AND frequency=?""",
        (trad, simp, pin, jyut, freq),
    )
    row = c.fetchone()
    if row is None:
        return -1
    return row[0]


def get_definition_id(c, definition, label, entry_id, source_id):
    c.execute(
        """SELECT rowid FROM definitions WHERE definition=?
            AND label=? AND fk_entry_id=? AND fk_source_id=?""",
        (definition, label, entry_id, source_id),
    )
    row = c.fetchone()
    if row is None:
        return -1
    return row[0]


def get_example_id(c, trad, simp, pin, jyut, lang):
    c.execute(
        """SELECT rowid FROM examples WHERE traditional=?
            AND simplified=? AND pinyin=? AND jyutping=? AND language=?""",
        (trad, simp, pin, jyut, lang),
    )
    row = c.fetchone()
    if row is None:
        return -1
    return row[0]


def get_translation_id(c, translation, lang):
    c.execute(
        "SELECT rowid FROM example_translations WHERE translation=? AND language=?",
        (translation, lang),
    )
    translation_row = c.fetchone()

    if translation_row:
        return translation_row[0]
    return -1


def get_example_link(c, example_id, translation_id):
    c.execute(
        "SELECT rowid FROM example_links WHERE fk_example_id=? AND fk_example_translation_id=?",
        (example_id, translation_id),
    )
    link_row = c.fetchone()

    if link_row:
        return link_row[0]
    return -1
