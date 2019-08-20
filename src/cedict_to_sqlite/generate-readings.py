import sys

class Entry(object):
    def __init__(self, trad='', simp='', pin='', jyut=''):
        self.traditional = trad
        self.simplified = simp
        self.pinyin = pin
        self.jyutping = jyut

    def add_jyut_ping(self, jyut):
        self.jyutping = jyut

def write(entries, output_name):
    output_file = open(output_name, 'w+', encoding='utf8')

    output_file.write("# Cantonese Readings\n"
        "# Version 2019-08-20\n"
        "# \n"
        "# Copyright (c) 2015 Pleco Software Incorporated\n"
        "# Distributed under a Creative Commons Attribution-Share Alike 3.0 license (http://creativecommons.org/licenses/by-sa/3.0/)\n"
        "# \n"
        "# For more information + to submit feedback, see our website at cccanto.org\n"
        "# \n")

    for trad in entries:
        for entry in entries[trad]:
            output_file.write("{} {} [{}] {{{}}}\n".format(entry.traditional, entry.simplified, entry.pinyin, entry.jyutping))

    output_file.close()

def parse_cc_canto(filename, entries):
    with open(filename, 'r', encoding='utf8') as f:
        for line in f:
            if (len(line) == 0 or line[0] == '#'):
                continue

            split = line.split() # Splits by whitespace
            trad = split[0]
            simp = split[1]
            pin = line[line.index('[') + 1 : line.index(']')].lower()
            jyut = line[line.index('{') + 1 : line.index('}')].lower()
            entry = Entry(trad=trad,
                          simp=simp,
                          pin=pin,
                          jyut=jyut)

            # Check if entry is already in dictionary
            if trad in entries:
                # If entry is in dictionary, then
                # make sure is new entry before adding
                # to list
                new_entry = True
                for existing_entry in entries[trad]:
                    if existing_entry.simplified == simp and existing_entry.pinyin == pin and existing_entry.jyutping == jyut:
                        new_entry = False
                        break
                if new_entry:
                    entries[trad].append(entry)
            else:
                entries[trad] = [entry]

def parse_cc_cedict_canto_readings(filename, entries):
    with open(filename, 'r', encoding='utf8') as f:
        for line in f:
            if (len(line) == 0 or line[0] == '#'):
                continue

            split = line.split()
            trad = split[0]
            simp = split[1]
            pin = line[line.index('[') + 1 : line.index(']')].lower()
            jyut = line[line.index('{') + 1 : line.index('}')].lower()

            entry = Entry(trad=trad,
                          simp=simp,
                          pin=pin,
                          jyut=jyut)

            if trad in entries:
                new_entry = True
                for existing_entry in entries[trad]:
                    if existing_entry.simplified == simp and existing_entry.pinyin == pin and existing_entry.jyutping == jyut:
                        new_entry = False
                        break
                if new_entry:
                    entries[trad].append(entry)
            else:
                entries[trad] = [entry]

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('Usage: python3 script.py <output filename> <CC_CANTO file> <Cantonese Readings file>')
        print('e.g. python3 script.py FULLREADINGS.txt CC-CANTO.txt READINGS.txt')
        sys.exit(1)

    entries = {}
    parse_cc_canto(sys.argv[2], entries)
    parse_cc_cedict_canto_readings(sys.argv[3], entries)
    write(entries, sys.argv[1])