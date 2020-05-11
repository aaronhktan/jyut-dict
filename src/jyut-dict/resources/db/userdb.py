import datetime
import sqlite3

db = sqlite3.connect('user.db')
c = db.cursor()

# Create tables for favourites
c.execute('''CREATE TABLE favourite_words(
      favourite_id INTEGER PRIMARY KEY,
      traditional TEXT,
      simplified TEXT,
      jyutping TEXT,
      pinyin TEXT,
      fk_list_id INTEGER,
      timestamp TEXT,
      FOREIGN KEY (fk_list_id) REFERENCES favourite_lists(list_id) ON DELETE CASCADE
  )''')

c.execute('''CREATE TABLE favourite_lists(
      list_id INTEGER PRIMARY KEY,
      name TEXT,
      timestamp TEXT
  )''')

c.execute('INSERT INTO favourite_lists VALUES(?,?,?)', 
  (1, 'General', datetime.datetime.now()))

# Create tables for search history and view history
c.execute('''CREATE TABLE search_history(
      search_history_id INTEGER PRIMARY KEY,
      search_text TEXT,
      search_options INTEGER,
      timestamp TEXT
  )''')

c.execute('''CREATE TABLE view_history(
      view_history_id INTEGER PRIMARY KEY,
      traditional TEXT,
      simplified TEXT,
      jyutping TEXT,
      pinyin TEXT,
      timestamp TEXT
  )''')

c.execute('PRAGMA user_version=1')

db.commit()
db.close()
