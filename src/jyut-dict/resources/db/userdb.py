import datetime
import sqlite3

db = sqlite3.connect('user.db')
c = db.cursor()

# Add source to database
c.execute('BEGIN TRANSACTION')

c.execute('''CREATE TABLE favourite_words(
      favourite_id INTEGER PRIMARY KEY,
      traditional TEXT,
      simplified TEXT,
      jyutping TEXT,
      pinyin TEXT,
      fk_list_id INTEGER,
      timestamp TEXT,
      FOREIGN KEY (fk_list_id) REFERENCES favourite_lists(list_id) ON DELETE CASCADE)
  ''')

c.execute('''CREATE TABLE favourite_lists(
      list_id INTEGER PRIMARY KEY,
      name TEXT,
      timestamp TEXT
  )''')

c.execute('INSERT INTO favourite_lists VALUES(?,?,?)', 
  (1, 'General', datetime.datetime.now()))

db.commit()
db.close()
