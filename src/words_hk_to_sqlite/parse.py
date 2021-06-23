from bs4 import BeautifulSoup
from hanziconv import HanziConv
import hanzidentifier
import jieba
from pypinyin import lazy_pinyin, Style
from wordfreq import zipf_frequency

from collections import defaultdict, namedtuple
import logging
import os
import sqlite3
import sys

# Structure of entries: a WORD contains multiple MEANINGS
# A MEANING contains multiple DEFINITIONS in different languages, as well
# as EXAMPLES
ExampleTuple = namedtuple('ExampleTuple', ['lang', 'pron', 'content'])
DefinitionTuple = namedtuple('DefinitionTuple', ['lang', 'content'])
MeaningTuple = namedtuple(
    'Meaning', [
        'label', 'definitions', 'examplephrases', 'examplesentences'])
WordTuple = namedtuple('Word', ['word', 'pronunciation', 'meanings'])

SourceTuple = namedtuple('Source',
                         ['name', 'shortname', 'version', 'description',
                          'legal', 'link', 'update_url', 'other'])

# Things that would be nice that are currently not handled:
#   - Alternate pronunciations (currently just take the first pronunciation on the page)
#   - POS tagging (database structure doesn't integrate this for now)

# Useful test pages:
#   - 脂粉客 for malformed entry (or old entry)
#   - 了 for multiple meanings, multiple examples
#   - 一戙都冇 for multiple header words
#   - 是 for item with label and POS
#   - 印 for item with different POS for different meanings
#   - 使勁 for broken sentence
#   - 鬼靈精怪 for multiple different pronunciations
#   - 掌 and 撻 for different definitions (腳掌, 撻沙) with the same English translation (sole)


def drop_tables(c):
    # Drop existing tables
    c.execute('DROP TABLE IF EXISTS entries')
    c.execute('DROP TABLE IF EXISTS entries_fts')
    c.execute('DROP TABLE IF EXISTS sources')
    c.execute('DROP TABLE IF EXISTS definitions')
    c.execute('DROP TABLE IF EXISTS definitions_fts')
    c.execute('DROP INDEX IF EXISTS fk_entry_id_index')

    c.execute('DROP TABLE IF EXISTS chinese_sentences')
    c.execute('DROP TABLE IF EXISTS nonchinese_sentences')
    c.execute('DROP TABLE IF EXISTS sentence_links')
    c.execute('DROP INDEX IF EXISTS fk_chinese_sentence_id_index')
    c.execute('DROP INDEX IF EXISTS fk_non_chinese_sentence_id_index')

    c.execute('DROP TABLE IF EXISTS definitions_chinese_sentences_links')


def create_tables(c):
    # Create new tables
    c.execute('''CREATE TABLE entries(
                  entry_id INTEGER PRIMARY KEY,
                  traditional TEXT,
                  simplified TEXT,
                  pinyin TEXT,
                  jyutping TEXT,
                  frequency REAL,
                  UNIQUE(traditional, simplified, pinyin, jyutping) ON CONFLICT IGNORE
            )''')
    c.execute('CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)')

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

    c.execute('''CREATE TABLE definitions(
                  definition_id INTEGER PRIMARY KEY,
                  definition TEXT,
                  label TEXT,
                  fk_entry_id INTEGER,
                  fk_source_id INTEGER,
                  FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON UPDATE CASCADE,
                  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE,
                  UNIQUE(definition, fk_entry_id, fk_source_id) ON CONFLICT IGNORE
            )''')
    c.execute(
        'CREATE VIRTUAL TABLE definitions_fts using fts5(fk_entry_id UNINDEXED, definition)')

    c.execute('''CREATE TABLE chinese_sentences(
                  chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
                  traditional TEXT,
                  simplified TEXT,
                  pinyin TEXT,
                  jyutping TEXT,
                  language TEXT,
                  UNIQUE(traditional, simplified, pinyin, jyutping) ON CONFLICT IGNORE
            )''')

    c.execute('''CREATE TABLE nonchinese_sentences(
                  non_chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE,
                  sentence TEXT,
                  language TEXT,
                  UNIQUE(non_chinese_sentence_id, sentence) ON CONFLICT IGNORE
            )''')

    c.execute('''CREATE TABLE sentence_links(
                  fk_chinese_sentence_id INTEGER,
                  fk_non_chinese_sentence_id INTEGER,
                  fk_source_id INTEGER,
                  direct BOOLEAN,
                  FOREIGN KEY(fk_chinese_sentence_id) REFERENCES chinese_sentences(chinese_sentence_id),
                  FOREIGN KEY(fk_non_chinese_sentence_id) REFERENCES nonchinese_sentences(non_chinese_sentence_id),
                  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE CASCADE
            )''')

    c.execute('''CREATE TABLE definitions_chinese_sentences_links(
                  fk_definition_id INTEGER,
                  fk_chinese_sentence_id INTEGER,
                  FOREIGN KEY(fk_definition_id) REFERENCES definitions(definition_id),
                  FOREIGN KEY(fk_chinese_sentence_id) REFERENCES chinese_sentences(chinese_sentence_id)
                  UNIQUE(fk_definition_id, fk_chinese_sentence_id) ON CONFLICT IGNORE
            )''')


def insert_example(c, definition_id, starting_example_id, example):
    # The example should be a list of ExampleTuples, such that
    # the first item is the 'source', and all subsequent ones are the
    # translations
    examples_inserted = 0

    trad = example[0].content
    simp = HanziConv.toSimplified(trad)
    jyut = example[0].pron
    pin = ' '.join(lazy_pinyin(
        trad,
        style=Style.TONE3,
        neutral_tone_with_five=True)
    ).lower().replace('v', 'u:')
    lang = example[0].lang

    c.execute('SELECT max(rowid) FROM chinese_sentences')
    before_example_id = c.fetchone()[0]
    c.execute('INSERT INTO chinese_sentences values (?,?,?,?,?,?)',
              (starting_example_id, trad, simp, pin, jyut, lang))
    c.execute('SELECT max(rowid) FROM chinese_sentences')
    after_example_id = c.fetchone()[0]

    # Check if example insertion was successful
    if before_example_id == after_example_id:
        if trad == 'X' or trad == 'x':
            # Ignore examples that are just 'x'
            return 0
        else:
            # If insertion failed, it's probably because the example already exists
            # Get its rowid, so we can link it to this definition
            c.execute(
                '''SELECT rowid FROM chinese_sentences WHERE traditional=?
                    AND simplified=? AND pinyin=? AND jyutping=? AND language=?''',
                (trad,
                 simp,
                 pin,
                 jyut,
                 lang))
            row = c.fetchone()
            if row is None:
                logging.warning(
                    f'Unexpected failure on insertion for example: {definition_id} {trad}')
                return 0
            example_id = row[0]
    else:
        example_id = after_example_id
        examples_inserted += 1

    c.execute(
        'INSERT INTO definitions_chinese_sentences_links values (?,?)',
        (definition_id,
         example_id))

    for translation in example[1:]:
        trad = translation.content
        lang = translation.lang

        # Check if translation already exists before trying to insert
        # Insert a translation only if the translation doesn't already exist in the database
        c.execute(
            'SELECT rowid FROM nonchinese_sentences WHERE sentence=? AND language=?',
            (trad,
             lang))
        translation_row = c.fetchone()

        if translation_row is None:
            translation_id = starting_example_id + examples_inserted
            c.execute(
                'INSERT INTO nonchinese_sentences values (?,?,?)', (translation_id, trad, lang))
            examples_inserted += 1
        else:
            translation_id = translation_row[0]

        # Then, link the translation to the example only if the link doesn't already exist
        c.execute(
            'SELECT rowid FROM sentence_links WHERE fk_chinese_sentence_id=? AND fk_non_chinese_sentence_id=?',
            (example_id,
             translation_id))
        link_row = c.fetchone()

        if link_row is None:
            c.execute('INSERT INTO sentence_links values (?,?,?,?)',
                      (example_id, translation_id, 1, True))



    return examples_inserted


def generate_indices(c):
    # Generate fts5 indices for entries and definitions
    c.execute(
        'INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT rowid, pinyin, jyutping FROM entries')
    c.execute('INSERT INTO definitions_fts (rowid, fk_entry_id, definition) select rowid, fk_entry_id, definition FROM definitions')

    c.execute('CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)')


def insert_words(c, words):
    # Because the sentence id is presumed to be unique by Tatoeba, we will give it
    # a namespace of 999999999 potential sentences. Thus, words.hk sentences will start
    # at rowid 1000000000.
    example_id = 1000000000

    for key in words:
        for entry in words[key]:
            trad = entry.word
            simp = HanziConv.toSimplified(trad)
            jyut = entry.pronunciation
            pin = ' '.join(lazy_pinyin(
                trad,
                style=Style.TONE3,
                neutral_tone_with_five=True)
            ).lower().replace('v', 'u:')
            freq = zipf_frequency(trad, 'zh')

            c.execute('INSERT INTO entries values (?,?,?,?,?,?)',
                      (None, trad, simp, pin, jyut, freq))
            c.execute('SELECT max(rowid) FROM entries')
            entry_id = c.fetchone()[0]

            # Insert each meaning for the entry
            for meaning in entry.meanings:
                definitions = []
                for definition in meaning.definitions:
                    # Insert a zero-width space between Chinese words so that
                    # fts5 properly indexes them
                    is_chinese = (
                        hanzidentifier.is_simplified(
                            definition.content) or hanzidentifier.is_traditional(
                            definition.content))
                    if is_chinese:
                        definitions.append(
                            '​'.join(jieba.cut(definition.content)))
                    else:
                        definitions.append(definition.content)
                definition = '\r\n'.join(definitions)

                c.execute('INSERT INTO definitions values (?,?,?,?,?)',
                          (None, definition, meaning.label, entry_id, 1))
                c.execute('SELECT max(rowid) FROM definitions')
                definition_id = c.fetchone()[0]

                # Insert examples for each meaning
                for sentence in meaning.examplesentences:
                    examples_inserted = insert_example(c, definition_id, example_id, sentence)
                    example_id += examples_inserted

                for phrase in meaning.examplephrases:
                    examples_inserted = insert_example(c, definition_id, example_id, phrase)
                    example_id += examples_inserted


def write(db_name, source, words):
    print('Writing to database file')

    db = sqlite3.connect(db_name)
    c = db.cursor()

    c.execute('PRAGMA user_version=3')

    drop_tables(c)
    create_tables(c)

    # Add source information to table
    c.execute('INSERT INTO sources values(?,?,?,?,?,?,?,?,?)',
              (None, source.name, source.shortname, source.version,
               source.description, source.legal, source.link,
               source.update_url, source.other))

    insert_words(c, words)

    generate_indices(c)

    db.commit()
    db.close()


def parse_definitions(def_elem):
    try:
        # Extract the language of this definition, and then remove it from the
        # tree
        lang_tag = def_elem.find(
            'span', class_='zi-item-lang', recursive=False)
        lang = lang_tag.text.strip('()')
        lang_tag.decompose()
    except BaseException:
        lang = 'yue'  # Assume Cantonese definition if no lang tag found

    # Get the textual content of the definition
    content = def_elem.text.strip()

    return DefinitionTuple(lang, content)


def parse_examples(ex_elem, classname):
    ex = []
    ex_divs = ex_elem.find_all('div', class_=None, recursive=False)
    for ex_div in ex_divs:
        lang_tag = ex_div.find('span', class_='zi-item-lang')
        lang = lang_tag.text.strip('()')
        lang_tag.decompose()

        pronunciation_tag = ex_div.find('span', class_=classname)
        if pronunciation_tag:
            pronunciation = pronunciation_tag.text.strip('()')
            pronunciation_tag.decompose()
        else:
            pronunciation = ''

        content = ex_div.text.strip()
        ex.append(ExampleTuple(lang, pronunciation, content))
    return ex


def log_word(word):
    # Logging for debug purposes
    logging.debug(' '.join([word.word, word.pronunciation]))
    for index, meaning in enumerate(word.meanings):
        logging.debug(f'Meaning {index}:')
        for definition in meaning.definitions:
            logging.debug(
                ' '.join(['\tDefinition:', definition.lang, definition.content]))
        for i, phrases in enumerate(meaning.examplephrases):
            logging.debug(f'\t\tPhrase {i}:')
            for phrase in phrases:
                logging.debug(
                    ' '.join(['\t\t\tPhrase: ', phrase.lang, phrase.content, phrase.pron]))
        for i, sentences in enumerate(meaning.examplesentences):
            logging.debug(f'\t\tSentence {i}:')
            for sentence in sentences:
                logging.debug(
                    ' '.join(['\t\t\tSentence:', sentence.lang, sentence.content, sentence.pron]))


def parse_file(file_name, words):
    with open(file_name, 'r') as file:
        soup = BeautifulSoup(file, 'html.parser')
        drafts = soup.find_all('div', class_='draft-version')

        # Each separate entry for a particular word is contained in a div with
        # a class called "draft-version"
        for draft in drafts:
            word, _ = os.path.splitext(os.path.basename(file_name))

            # In words with multiple written forms or pronunciations,
            # try to parse the correct one
            word_pronunciation = ''
            written_forms = draft.find('tr', class_='zi-written-forms')
            word_pronunciations = written_forms.find_all('li', class_=None)
            if word_pronunciations:
                for item in word_pronunciations:
                    corresponding_word = ''.join(item.find_all(
                        text=True, recursive=False)).strip()
                    if corresponding_word == word:
                        word_pronunciation = item.find(
                            'span', class_='zi-pronunciation').text
                        break

            # If not able to find a corresponding pronunciation or there is only one,
            # take the first one that exists in this entry
            if not word_pronunciation:
                word_pronunciation = draft.find(
                    'span', class_='zi-pronunciation').text

            # The POS tag is in a class labelled zidin-pos
            # Each draft has definitions for only one POS label, so if an entry like 印 is
            # both a noun and a verb, words.hk separates it into two drafts.
            pos_elem = draft.find('tr', class_='zidin-pos')
            if pos_elem:
                pos = pos_elem.find('span', class_=None).get_text()
            else:
                pos = ''

            meanings = []
            # This will find the table row containing meaning if there are multiple meanings
            # If there are multiple meanings, they will be in an ordered list,
            # so extract every item in the list.
            try:
                list_items = draft.find(
                    'tr', class_='zidin-explanation').find_all('li')
                # This will find the table row containing meanings if there is
                # a single meaning
                if not list_items:
                    list_items = [
                        draft.find(
                            'tr',
                            class_='zidin-explanation').find_all('td')[1]]
            except BaseException:
                # If there is no zidin-explanation class, it might be an old page
                # These do not contain good formatting, so just stick it into
                # the list and call it a day
                try:
                    text = draft.find('li', class_=None).text
                    definition = DefinitionTuple(
                        'yue', text)  # Assume definition is in Cantonese
                    meanings.append(MeaningTuple('', [definition], [], []))
                    words[word].append(
                        WordTuple(
                            word,
                            word_pronunciation,
                            meanings))
                    continue
                except BaseException:
                    # Malformed page, give up
                    logging.warning(f'Failed to parse for item {word}')
                    continue

            logging.info(f'Parsing item {word}')
            for list_item in list_items:
                meaning = MeaningTuple(pos, [], [], [])

                # Each definition for one meaning is contained in a classless
                # div
                def_divs = list_item.find_all(
                    'div', class_=None, recursive=False)
                for def_div in def_divs:
                    meaning.definitions.append(parse_definitions(def_div))

                # Each example phrase for the definition is contained in a div
                # with class zi-details-phrase-item
                exphr_elems = list_item.find_all(
                    'div', class_='zi-details-phrase-item', recursive=False)
                for exphr_elem in exphr_elems:
                    meaning.examplephrases.append(parse_examples(
                        exphr_elem, 'zi-item-phrase-pronunciation'))

                # Each example sentence for the definition is contained in a
                # div with class zi-details-example-item
                exsen_elems = list_item.find_all(
                    'div', class_='zi-details-example-item', recursive=False)
                for exsen_elem in exsen_elems:
                    meaning.examplesentences.append(parse_examples(
                        exsen_elem, 'zi-item-example-pronunciation'))

                meanings.append(meaning)

            word_tuple = WordTuple(word, word_pronunciation, meanings)
            log_word(word_tuple)

            words[word].append(word_tuple)


def parse_folder(folder_name, words):
    for index, entry in enumerate(os.scandir(folder_name)):
        if not index % 100:
            print(f'Parsed word #{index}')
        if entry.is_file() and entry.path.endswith('.html'):
            parse_file(entry.path, words)


if __name__ == '__main__':
    if len(sys.argv) != 11:
        print(('Usage: python3 script.py <database filename> '
               '<HTML folder> <source name> <source short name> '
               '<source version> <source description> <source legal> '
               '<source link> <source update url> <source other>'))
        sys.exit(1)

    source = SourceTuple(sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6],
                         sys.argv[7], sys.argv[8], sys.argv[9], sys.argv[10])
    # logging.basicConfig(level='DEBUG') # Uncomment to enable debug logging
    parsed_words = defaultdict(list)
    parse_folder(sys.argv[2], parsed_words)
    write(sys.argv[1], source, parsed_words)
