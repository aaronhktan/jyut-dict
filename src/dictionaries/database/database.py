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
                  UNIQUE(definition, fk_entry_id, fk_source_id) ON CONFLICT IGNORE
            )"""
    )
    c.execute(
        "CREATE VIRTUAL TABLE definitions_fts using fts5(fk_entry_id UNINDEXED, definition)"
    )

    c.execute(
        """CREATE TABLE chinese_sentences(
                  chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
                  traditional TEXT,
                  simplified TEXT,
                  pinyin TEXT,
                  jyutping TEXT,
                  language TEXT,
                  UNIQUE(traditional, simplified, pinyin, jyutping) ON CONFLICT IGNORE
            )"""
    )

    c.execute(
        """CREATE TABLE nonchinese_sentences(
                  non_chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
                  sentence TEXT,
                  language TEXT,
                  UNIQUE(non_chinese_sentence_id, sentence) ON CONFLICT IGNORE
            )"""
    )

    c.execute(
        """CREATE TABLE sentence_links(
                  fk_chinese_sentence_id INTEGER,
                  fk_non_chinese_sentence_id INTEGER,
                  fk_source_id INTEGER,
                  direct BOOLEAN,
                  FOREIGN KEY(fk_chinese_sentence_id) REFERENCES chinese_sentences(chinese_sentence_id),
                  FOREIGN KEY(fk_non_chinese_sentence_id) REFERENCES nonchinese_sentences(non_chinese_sentence_id),
                  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE
                  UNIQUE(fk_chinese_sentence_id, fk_non_chinese_sentence_id) ON CONFLICT IGNORE
            )"""
    )

    c.execute(
        """CREATE TABLE definitions_chinese_sentences_links(
                  fk_definition_id INTEGER,
                  fk_chinese_sentence_id INTEGER,
                  FOREIGN KEY(fk_definition_id) REFERENCES definitions(definition_id),
                  FOREIGN KEY(fk_chinese_sentence_id) REFERENCES chinese_sentences(chinese_sentence_id)
                  UNIQUE(fk_definition_id, fk_chinese_sentence_id) ON CONFLICT IGNORE
            )"""
    )


def drop_tables(c):
    c.execute("DROP TABLE IF EXISTS entries")
    c.execute("DROP TABLE IF EXISTS entries_fts")
    c.execute("DROP TABLE IF EXISTS sources")
    c.execute("DROP TABLE IF EXISTS definitions")
    c.execute("DROP TABLE IF EXISTS definitions_fts")
    c.execute("DROP INDEX IF EXISTS fk_entry_id_index")

    c.execute("DROP TABLE IF EXISTS chinese_sentences")
    c.execute("DROP TABLE IF EXISTS nonchinese_sentences")
    c.execute("DROP TABLE IF EXISTS sentence_links")
    c.execute("DROP INDEX IF EXISTS fk_chinese_sentence_id_index")
    c.execute("DROP INDEX IF EXISTS fk_non_chinese_sentence_id_index")

    c.execute("DROP TABLE IF EXISTS definitions_chinese_sentences_links")


def write_database_version(c):
    c.execute("PRAGMA user_version=3")


def generate_indices(c):
    c.execute(
        "INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT rowid, pinyin, jyutping FROM entries"
    )
    c.execute(
        "INSERT INTO definitions_fts (rowid, fk_entry_id, definition) select rowid, fk_entry_id, definition FROM definitions"
    )

    c.execute("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)")
    c.execute("CREATE INDEX chinese_sentences_index ON chinese_sentences(chinese_sentence_id, traditional, simplified, pinyin, jyutping, language)")
    c.execute("CREATE INDEX definition_index ON definitions(definition_id, definition, label, fk_entry_id, fk_source_id)")


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


def insert_definition_chinese_sentence_link(c, definition_id, chinese_sentence_id):
    c.execute("SELECT max(rowid) FROM definitions_chinese_sentences_links")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO definitions_chinese_sentences_links values (?,?)",
        (definition_id, chinese_sentence_id),
    )

    c.execute("SELECT max(rowid) FROM definitions_chinese_sentences_links")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted a definition<->sentence link
    if after_id != before_id:
        return after_id
    return -1


def insert_chinese_sentence(c, trad, simp, pin, jyut, lang, id_=None):
    c.execute("SELECT max(rowid) FROM chinese_sentences")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO chinese_sentences values (?,?,?,?,?,?)",
        (id_, trad, simp, pin, jyut, lang),
    )

    c.execute("SELECT max(rowid) FROM chinese_sentences")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted a Chinese sentence
    if after_id != before_id:
        return after_id
    return -1


def insert_nonchinese_sentence(c, sentence, lang, id_=None):
    c.execute("SELECT max(rowid) FROM nonchinese_sentences")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO nonchinese_sentences values (?,?,?)",
        (id_, sentence, lang),
    )

    c.execute("SELECT max(rowid) FROM nonchinese_sentences")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted a non-Chinese sentence
    if after_id != before_id:
        return after_id
    return -1


def insert_sentence_link(c, sentence_id, translation_id, source_id, direct):
    c.execute("SELECT max(rowid) FROM sentence_links")
    before_id = -1
    result = c.fetchone()
    if result:
        before_id = result[0]

    c.execute(
        "INSERT INTO sentence_links values (?,?,?,?)",
        (sentence_id, translation_id, source_id, direct),
    )

    c.execute("SELECT max(rowid) FROM sentence_links")
    result = c.fetchone()
    if result:
        after_id = result[0]

    # Compare before and after id to see if we sucessfully inserted a sentence<->translation link
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


def get_chinese_sentence_id(c, trad, simp, pin, jyut, lang):
    c.execute(
        """SELECT rowid FROM chinese_sentences WHERE traditional=?
            AND simplified=? AND pinyin=? AND jyutping=? AND language=?""",
        (trad, simp, pin, jyut, lang),
    )
    row = c.fetchone()
    if row is None:
        return -1
    return row[0]


def get_nonchinese_sentence_id(c, sentence, lang):
    c.execute(
        "SELECT rowid FROM nonchinese_sentences WHERE sentence=? AND language=?",
        (sentence, lang),
    )
    translation_row = c.fetchone()

    if translation_row:
        return translation_row[0]
    return -1


def get_sentence_link(c, sentence_id, translation_id):
    c.execute(
        "SELECT rowid FROM sentence_links WHERE fk_chinese_sentence_id=? AND fk_non_chinese_sentence_id=?",
        (sentence_id, translation_id),
    )
    link_row = c.fetchone()

    if link_row:
        return link_row[0]
    return -1
