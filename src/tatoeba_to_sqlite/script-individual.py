import hanzidentifier
from hanziconv import HanziConv
from pypinyin import pinyin, Style

import sqlite3
import sys

source = {
    'name': '',
    'shortname': '',
    'version': '',
    'description': '',
    'legal': '',
    'link': '',
    'update_url': '',
    'other': ''
}

class Sentence(object):
    def __init__(self, trad='', simp='', pin='', jyut='', translations=None, lang=''):
        self.traditional = trad
        self.simplified = simp
        self.pinyin = pin
        self.jyutping = jyut
        self.sentence_translations = translations if translations is not None else []
        self.language = lang

    def add_translation(self, sentence_translation):
        self.sentence_translations.append(sentence_translation)

class SentenceTranslation(object):
    def __init__(self, sentence='', lang=''):
        self.sentence = sentence
        self.lang = lang

def write(sentences, db_name):
    db = sqlite3.connect(db_name)
    c = db.cursor()

    # Set version of database
    c.execute('PRAGMA user_version=2')

    # Delete old tables and indices
    c.execute('DROP TABLE IF EXISTS sentences')
    c.execute('DROP TABLE IF EXISTS sentences_fts')
    c.execute('DROP TABLE IF EXISTS sources')
    c.execute('DROP TABLE IF EXISTS sentence_translations')
    c.execute('DROP TABLE IF EXISTS sentence_translations_fts')
    c.execute('DROP INDEX IF EXISTS fk_sentence_id_index')

    # Create new tables
    c.execute('''CREATE TABLE sentences(
                    sentence_id INTEGER PRIMARY KEY,
                    traditional TEXT,
                    simplified TEXT,
                    pinyin TEXT,
                    jyutping TEXT,
                    language TEXT,
                    UNIQUE(traditional, simplified, pinyin, jyutping) ON CONFLICT IGNORE
                )''')
    c.execute('CREATE VIRTUAL TABLE sentences_fts using fts5(pinyin, jyutping)')

    c.execute('''CREATE TABLE sources(
                    source_id INTEGER PRIMARY KEY,
                    sourcename TEXT UNIQUE ON CONFLICT ABORT,
                    sourceshortname TEXT,
                    version TEXT,
                    description TEXT,
                    legal TEXT,
                    link TEXT,
                    update_url TEXT,
                    other TEXT
                )''')

    c.execute('''CREATE TABLE sentence_translations(
                    sentence_translation_id INTEGER PRIMARY KEY,
                    sentence_translation TEXT,
                    language TEXT,
                    fk_sentence_id INTEGER,
                    fk_source_id INTEGER,
                    FOREIGN KEY(fk_sentence_id) REFERENCES sentences(sentence_id) ON UPDATE CASCADE,
                    FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE,
                    UNIQUE(sentence_translation, fk_sentence_id, fk_source_id) ON CONFLICT IGNORE
                )''')
    c.execute('CREATE VIRTUAL TABLE sentence_translations_fts using fts5(sentence_translation)')

    # SQLITE 3 currently only supports triggers FOR EACH ROW
    # making these extremely slow and time-consuming
    # Disable for now.
    # Delete sentences when no sentence_translations reference them
    # c.execute('''CREATE TRIGGER IF NOT EXISTS sentence_cleanup 
    #                 AFTER DELETE ON sentence_translations
    #             BEGIN
    #                 DELETE FROM sentences WHERE sentence_id NOT IN (SELECT fk_sentence_id FROM sentence_translations);
    #             END
    #             ''')

    # Rebuild sentences FTS after modifying sentences
    # c.execute('''CREATE TRIGGER IF NOT EXISTS sentences_fts_cleanup 
    #                 AFTER DELETE ON sentences
    #             BEGIN
    #                 DELETE FROM sentences_fts WHERE rowid NOT IN (SELECT sentence_id FROM sentences);
    #             END
    #             ''')

    # Rebuild sentence_translation FTS after modifying sentence_translations
    # c.execute('''CREATE TRIGGER IF NOT EXISTS sentence_translations_fts_cleanup 
    #                 AFTER DELETE ON sentence_translations
    #             BEGIN
    #                 DELETE FROM sentence_translations_fts WHERE rowid NOT IN (SELECT sentence_translation_id FROM sentence_translations);
    #             END
    #             ''')

    # Add sources to tables
    c.execute('INSERT INTO sources values(?,?,?,?,?,?,?,?,?)', (None, source['name'], source['shortname'], source['version'], source['description'], source['legal'], source['link'], source['update_url'], source['other']))

    # Add sentences to tables
    def sentence_to_tuple(sentence):
        return (None, sentence.traditional, sentence.simplified, sentence.pinyin, sentence.jyutping, sentence.language)

    def sentence_translation_to_tuple(translation, sentence_id, source_id):
        return (None, translation.sentence, translation.lang, sentence_id, source_id)

    for key in sentences:
        sentence = sentences[key]
        if not sentence.sentence_translations:
            continue
        c.execute('INSERT INTO sentences values (?,?,?,?,?,?)', sentence_to_tuple(sentence))

        c.execute('SELECT last_insert_rowid()')
        sentence_id = c.fetchone()[0]
        sentence_translation_tuples = [sentence_translation_to_tuple(sentence_translation, sentence_id, 1) for sentence_translation in sentence.sentence_translations]
        c.executemany('INSERT INTO sentence_translations values (?,?,?,?,?)', sentence_translation_tuples)

    # Populate FTS versions of tables
    c.execute('INSERT INTO sentences_fts (rowid, pinyin, jyutping) SELECT rowid, pinyin, jyutping FROM sentences')
    c.execute('INSERT INTO sentence_translations_fts (rowid, sentence_translation) SELECT rowid, sentence_translation FROM sentence_translations')

    # Create index
    c.execute('CREATE INDEX fk_sentence_id_index ON sentence_translations(fk_sentence_id)')

    db.commit()
    db.close()

def parse_sentence_file(filename, sentences, sentence_translations):
    with open(filename, 'r', encoding='utf8') as f:
        for index, line in enumerate(f):
            if (len(line) == 0 or line[0] == '#'):
                continue

            split = line.split()
            lang = split[1]
            sentence_start = line.index('\t', line.index('\t')+1) + 1
            sentence = line[sentence_start:]

            # Source sentences are in Mandarin or Cantonese
            if lang == 'cmn' or lang == 'yue':
                if (hanzidentifier.is_simplified(sentence)):
                    trad = HanziConv.toTraditional(sentence)
                    simp = sentence
                else:
                    trad = sentence
                    simp = HanziConv.toSimplified(sentence)
                pin = ' '.join(p[0] for p in pinyin(sentence, style=Style.TONE3)).lower()
                sentence_id = split[0]
                sentence_row = Sentence(trad=trad,
                                    simp=simp,
                                    pin=pin,
                                    lang=lang)

                sentences[sentence_id] = sentence_row
                print('Added ID: {}'.format(sentence_id))

            # Translations are in English or French
            if lang == 'eng' or lang == 'fra':
                sentence_id = split[0]

                sentence = line[sentence_start:].strip()
                sentence_translation = SentenceTranslation(lang=lang,
                                                           sentence=sentence)
                sentence_translations[sentence_id] = sentence_translation
                print('Added ID: {}'.format(sentence_id))

def parse_links_file(filename, sentences, sentence_translations):
    with open(filename, 'r', encoding='utf8') as f:
        for line in f:
            if (len(line) == 0 or line[0] == '#'):
                continue

            split = line.split()
            first_id = split[0]
            second_id = split[1]

            if first_id in sentences and second_id in sentence_translations:
                sentences[first_id].add_translation(sentence_translations[second_id])
                print('Linked IDs: {}, {}'.format(first_id, second_id))

if __name__ == '__main__':
    if len(sys.argv) != 12:
        print('Usage: python3 script.py <database filename> <Tatoeba sentences file> <Tatoeba links file> <source name> <source short name> <source version> <source description> <source legal> <source link> <source update url> <source other>')
        print('e.g. python3 script.py dict.db sentences.csv links.csv Tatoeba TTB 2018-07-09 "Tatoeba is a collection of sentences." "These files are released under CC BY 2.0 FR." "https://tatoeba.org/eng/downloads" "" ""')
        sys.exit(1)

    sentences = {}
    sentence_translations = {}
    source['name'] = sys.argv[4]
    source['shortname'] = sys.argv[5]
    source['version'] = sys.argv[6]
    source['description'] = sys.argv[7]
    source['legal'] = sys.argv[8]
    source['link'] = sys.argv[9]
    source['update_url'] = sys.argv[10]
    source['other'] = sys.argv[11]
    parse_sentence_file(sys.argv[2], sentences, sentence_translations)
    parse_links_file(sys.argv[3], sentences, sentence_translations)
    write(sentences, sys.argv[1])