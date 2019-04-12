from wordfreq import zipf_frequency

import sqlite3
import sys

class Entry(object):
    def __init__(self, trad='', simp='', pin='', jyut='', freq=0.0, cedict_eng='', canto_eng=''):
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

    c.execute('drop table if exists entries');
    c.execute('create virtual table entries using fts5(traditional, simplified, pinyin, jyutping, freq, cedict_english, canto_english)')

    def entry_to_tuple(entry):
        return (entry.traditional, entry.simplified, entry.pinyin, entry.jyutping, entry.freq, entry.cedict_english, entry.canto_english)

    entry_tuples = map(entry_to_tuple, entries.values())
    c.executemany('insert into entries values (?,?,?,?,?,?,?)', entry_tuples)

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
            eng = line[line.index('/') + 1 : -2]
            entry = Entry(trad=trad,
                          simp=simp,
                          pin=pin,
                          cedict_eng=eng)

            if trad in entries:
                entries[trad + str(index)] = entry
            else:
                entries[trad] = entry

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
            eng = line[line.index('/') + 1 : -2]
            entry = Entry(trad=trad,
                          simp=simp,
                          pin=pin,
                          jyut=jyut,
                          canto_eng=eng)

            if trad in entries:
                entries[trad].add_jyut_ping(jyut)
                entries[trad].add_canto_eng(eng)
            else:
                entries[trad] = entry

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

            entries[trad].add_jyut_ping(jyut)

def assign_frequencies(entries):
    for entry in entries:
        freq = zipf_frequency(entries[entry].traditional, 'zh')
        entries[entry].add_freq(freq)

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