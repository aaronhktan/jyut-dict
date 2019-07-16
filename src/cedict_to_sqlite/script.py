from wordfreq import zipf_frequency

import sqlite3
import sys

sources = ("CC_CEDICT", "CC_CANTO")

class Entry(object):
    def __init__(self, trad='', simp='', pin='', jyut='', freq=0.0, cedict_eng=[], canto_eng=[]):
        self.traditional = trad
        self.simplified = simp
        self.pinyin = pin
        self.jyutping = jyut
        self.freq = freq
        self.cedict_english = cedict_eng
        self.canto_english = canto_eng

    def add_jyut_ping(self, jyut):
        self.jyutping = jyut

    def add_canto_eng(self, canto_eng):
        self.canto_english = canto_eng

    def add_freq(self, freq):
        self.freq = freq

def write(entries, db_name):
    db = sqlite3.connect(db_name)
    c = db.cursor()

    # Create tables
    c.execute('DROP TABLE IF EXISTS entries')
    c.execute('DROP TABLE IF EXISTS entries_fts')
    c.execute('DROP TABLE IF EXISTS sources')
    c.execute('DROP TABLE IF EXISTS definitions')
    c.execute('DROP TABLE IF EXISTS definitions_fts')

    c.execute('''CREATE TABLE entries(
                    entry_id INTEGER PRIMARY KEY,
                    traditional TEXT,
                    simplified TEXT,
                    pinyin TEXT,
                    jyutping TEXT,
                    frequency REAL
                )''')
    c.execute('CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)')

    c.execute('''CREATE TABLE sources(
                    source_id INTEGER PRIMARY KEY,
                    sourcename TEXT
                )''')

    c.execute('''CREATE TABLE definitions(
                    definition_id INTEGER PRIMARY KEY,
                    definition TEXT,
                    fk_entry_id INTEGER,
                    fk_source_id INTEGER,
                    FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON UPDATE CASCADE,
                    FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE
                )''')
    c.execute('CREATE VIRTUAL TABLE definitions_fts using fts5(definition)')

    # SQLITE 3 currently only supports triggers FOR EACH ROW
    # making these extremely slow and time-consuming
    # Disable for now.
    # Delete entries when no definitions reference them
    # c.execute('''CREATE TRIGGER IF NOT EXISTS entry_cleanup 
    #                 AFTER DELETE ON definitions
    #             BEGIN
    #                 DELETE FROM entries WHERE entry_id NOT IN (SELECT fk_entry_id FROM definitions);
    #             END
    #             ''')

    # Rebuild entries FTS after modifying entries
    # c.execute('''CREATE TRIGGER IF NOT EXISTS entries_fts_cleanup 
    #                 AFTER DELETE ON entries
    #             BEGIN
    #                 DELETE FROM entries_fts WHERE rowid NOT IN (SELECT entry_id FROM entries);
    #             END
    #             ''')

    # Rebuild definition FTS after modifying definitions
    # c.execute('''CREATE TRIGGER IF NOT EXISTS definitions_fts_cleanup 
    #                 AFTER DELETE ON definitions
    #             BEGIN
    #                 DELETE FROM definitions_fts WHERE rowid NOT IN (SELECT definition_id FROM definitions);
    #             END
    #             ''')

    # Add sources to tables
    [c.execute('INSERT INTO sources values(?,?)', (None, value)) for value in sources]

    # Add entries to tables
    def entry_to_tuple(entry):
        return (None, entry.traditional, entry.simplified, entry.pinyin, entry.jyutping, entry.freq)

    def definition_to_tuple(definition, entry_id, source_id):
        return (None, definition, entry_id, source_id)

    for key in entries:
        for entry in entries[key]:
            c.execute('INSERT INTO entries values (?,?,?,?,?,?)', entry_to_tuple(entry))

            c.execute('SELECT last_insert_rowid()')
            entry_id = c.fetchone()[0]
            definition_tuples = [definition_to_tuple(definition, entry_id, sources.index('CC_CEDICT')+1) for definition in entry.cedict_english]
            definition_tuples += [definition_to_tuple(definition, entry_id, sources.index('CC_CANTO')+1) for definition in entry.canto_english]
            c.executemany('INSERT INTO definitions values (?,?,?,?)', definition_tuples)

    # Populate FTS versions of tables
    c.execute('INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT rowid, pinyin, jyutping FROM entries')
    c.execute('INSERT INTO definitions_fts (rowid, definition) SELECT rowid, definition FROM definitions')

    db.commit()
    db.close()

def parse_cc_cedict(filename, entries):
    with open(filename, 'r') as f:
        for index, line in enumerate(f):
            if (len(line) == 0 or line[0] == '#'):
                continue

            split = line.split()
            trad = split[0]
            simp = split[1]
            pin = line[line.index('[') + 1 : line.index(']')]
            eng = line[line.index('/') + 1 : -2].split('/')
            entry = Entry(trad=trad,
                          simp=simp,
                          pin=pin,
                          cedict_eng=eng)

            if trad in entries:
                entries[trad].append(entry)
            else:
                entries[trad] = [entry]

def parse_cc_canto(filename, entries):
    with open(filename, 'r') as f:
        for line in f:
            if (len(line) == 0 or line[0] == '#'):
                continue

            split = line.split() # Splits by whitespace
            trad = split[0]
            simp = split[1]
            pin = line[line.index('[') + 1 : line.index(']')]
            jyut = line[line.index('{') + 1 : line.index('}')]
            eng = [line[line.index('/') + 1 : -2]]
            entry = Entry(trad=trad,
                          simp=simp,
                          pin=pin,
                          jyut=jyut,
                          canto_eng=eng)

            if trad in entries:
                for entry in entries[trad]:
                    entry.add_jyut_ping(jyut)
                entries[trad][0].add_canto_eng(eng)
            else:
                entries[trad] = [entry]

def parse_cc_cedict_canto_readings(filename, entries):
    with open(filename, 'r') as f:
        for line in f:
            if (len(line) == 0 or line[0] == '#'):
                continue

            split = line.split()
            trad = split[0]
            jyut = line[line.index('{') + 1 : -2]
            if trad not in entries:
                continue

            for entry in entries[trad]:
                entry.add_jyut_ping(jyut)

def assign_frequencies(entries):
    for key in entries:
        for entry in entries[key]:
            freq = zipf_frequency(entry.traditional, 'zh')
            entry.add_freq(freq)

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print('Usage: python3 script.py <database filename> <CC_CEDICT file> <CC_CANTO file> <Cantonese Readings file>')
        print('e.g. python3 script.py eng.db CC-CEDICT.txt CC-CANTO.txt READINGS.txt')
        sys.exit(1)

    entries = {}
    parse_cc_cedict(sys.argv[2], entries)
    parse_cc_canto(sys.argv[3], entries)
    parse_cc_cedict_canto_readings(sys.argv[4], entries)
    assign_frequencies(entries)
    write(entries, sys.argv[1])