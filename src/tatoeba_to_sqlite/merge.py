import sqlite3
import sys

def merge(db_name, db_attach):
  db = sqlite3.connect(db_name)
  c = db.cursor()

  c.execute('ATTACH DATABASE "{}" as db'.format(db_attach))

  # Add source to database
  c.execute('BEGIN TRANSACTION')

  print('Adding source')
  c.execute('''INSERT INTO sources(
    sourcename, sourceshortname, version, description, legal, link,
    update_url, other) 
    SELECT sourcename, sourceshortname, version, description, legal, link,
    update_url, other 
    FROM db.sources''')

  print('Adding Chinese sentences')
  c.execute('''INSERT INTO chinese_sentences(
    chinese_sentence_id, traditional, simplified, pinyin, jyutping, language)
    SELECT chinese_sentence_id, traditional, simplified, pinyin, jyutping, language
    FROM db.chinese_sentences''')

  print('Adding non-Chinese sentences')
  c.execute('''INSERT INTO nonchinese_sentences(
    non_chinese_sentence_id, sentence, language)
    SELECT non_chinese_sentence_id, sentence, language
    FROM db.nonchinese_sentences
    ''')

  print('Adding links')
  c.execute('''CREATE TEMPORARY TABLE links_tmp AS
    SELECT sentence_links.fk_chinese_sentence_id as fk_chinese_sentence_id,
    sentence_links.fk_non_chinese_sentence_id as fk_non_chinese_sentence_id,
    sources.sourcename AS sourcename,
    sentence_links.direct as direct
    FROM db.sentence_links, db.sources 
    WHERE db.sentence_links.fk_source_id = db.sources.source_id''')

  c.execute('''INSERT INTO sentence_links(
    fk_chinese_sentence_id, fk_non_chinese_sentence_id, fk_source_id, direct)
    SELECT l.fk_chinese_sentence_id, l.fk_non_chinese_sentence_id,
    s.source_id, l.direct
    FROM links_tmp as l, sources as s
    WHERE l.sourcename = s.sourcename''')

  # c.execute('''DETACH DATABASE db''')
  db.commit()
  db.close()

if __name__ == '__main__':
  merge(sys.argv[1], sys.argv[2])