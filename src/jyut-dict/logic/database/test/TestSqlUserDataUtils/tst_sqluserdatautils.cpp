#include <QtTest>

#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserdatautils.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace {
constexpr auto dbCreateConnName = "dbCreateConn";

class TestObserver : public ISearchObserver
{
public:
    void setExpected(bool entryExists, const Entry &entry)
    {
        _entryExists = entryExists;
        _entry = entry;
    }
    void setExpected(const std::vector<Entry> &entries) { _entries = entries; }
    void callback(const std::vector<Entry> &entries, bool emptyQuery) override
    {
        if (entries != _entries) {
            testFailed = true;
        }
        resultsReady.notify_one();
    }
    void callback(bool entryExists, const Entry &entry) override
    {
        if ((entryExists != _entryExists) || (entry != _entry)) {
            testFailed = true;
        }
        resultsReady.notify_one();
    }

    std::mutex mutex;
    std::condition_variable resultsReady;
    std::atomic_bool testFailed = false;

private:
    bool _entryExists;
    Entry _entry;
    std::vector<Entry> _entries;
};
} // namespace

class TestSqlUserDataUtils : public QObject
{
    Q_OBJECT

public:
    TestSqlUserDataUtils();
    ~TestSqlUserDataUtils();

private slots:
    void favouriteUnfavouriteEntry();
    void checkIfEntryHasBeenFavourited();
    void searchForAllFavouritedWords();

private:
    void createV4Database(const QString &dbPath);
    void createV1UserDatabase(const QString &dbPath);

    std::shared_ptr<SQLDatabaseManager> _manager;
};

TestSqlUserDataUtils::TestSqlUserDataUtils()
{
    _manager = std::make_shared<SQLDatabaseManager>();

    createV4Database(_manager->getDictionaryDatabasePath());
    createV1UserDatabase(_manager->getUserDatabasePath());
}

TestSqlUserDataUtils::~TestSqlUserDataUtils()
{
    _manager->removeAllDatabaseConnections();
    QFile::remove(_manager->getDictionaryDatabasePath());
    QFile::remove(_manager->getUserDatabasePath());
}

void TestSqlUserDataUtils::createV4Database(const QString &dbPath)
{
    QFile databaseFile{dbPath};
    QDir databaseDir{QFileInfo{databaseFile.fileName()}.absolutePath()};
    QCOMPARE(databaseDir.mkpath(
                 QFileInfo{databaseFile.fileName()}.absolutePath()),
             true);
    if (databaseFile.exists()) {
        QFile::remove(databaseFile.fileName());
    }
    QCOMPARE(databaseFile.open(QIODevice::ReadWrite), true);
    QCOMPARE(databaseFile.exists(), true);
    databaseFile.close();

    QSqlDatabase::addDatabase("QSQLITE", dbCreateConnName);
    QSqlDatabase::database(dbCreateConnName).setDatabaseName(dbPath);
    QSqlDatabase::database(dbCreateConnName).open();
    QSqlQuery query{QSqlDatabase::database(dbCreateConnName)};
    query.exec("CREATE TABLE examples( "
               "  example_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
               "  traditional TEXT, "
               "  simplified TEXT, "
               "  pinyin TEXT, "
               "  jyutping TEXT, "
               "  language TEXT, "
               "  UNIQUE(traditional, simplified, pinyin, jyutping, language) "
               "    ON CONFLICT IGNORE "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE definitions( "
               "  definition_id INTEGER PRIMARY KEY, "
               "  definition TEXT, "
               "  label TEXT, "
               "  fk_entry_id INTEGER, "
               "  fk_source_id INTEGER, "
               "  FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON "
               "    UPDATE CASCADE, "
               "  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON "
               "    DELETE CASCADE, "
               "  UNIQUE(definition, label, fk_entry_id, fk_source_id) ON "
               "    CONFLICT IGNORE "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "CREATE TABLE definitions_examples_links( "
        "  fk_definition_id INTEGER, "
        "  fk_example_id INTEGER, "
        "  FOREIGN KEY(fk_definition_id) REFERENCES definitions(definition_id) "
        "ON DELETE CASCADE, "
        "  FOREIGN KEY(fk_example_id) REFERENCES "
<<<<<<< HEAD
        "    examples(example_id), "
=======
        "    chinese_sentences(chinese_sentence_id) "
>>>>>>> 1ec70d9b (Rename chinese_sentences, nonchinese_sentences)
        "  UNIQUE(fk_definition_id, fk_example_id) ON CONFLICT IGNORE "
        ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE entries( "
               "  entry_id INTEGER PRIMARY KEY, "
               "  traditional TEXT, "
               "  simplified TEXT, "
               "  pinyin TEXT, "
               "  jyutping TEXT, "
               "  frequency REAL, "
               "  UNIQUE(traditional, simplified, pinyin, jyutping) ON "
               "    CONFLICT IGNORE "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE sources( "
               "  source_id INTEGER PRIMARY KEY, "
               "  sourcename TEXT UNIQUE ON CONFLICT ABORT, "
               "  sourceshortname TEXT, "
               "  version TEXT, "
               "  description TEXT, "
               "  legal TEXT, "
               "  link TEXT, "
               "  update_url TEXT, "
               "  other TEXT "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
<<<<<<< HEAD
        "CREATE TABLE example_translations( "
        "  example_translation_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
        "  translation TEXT, "
        "  language TEXT, "
        "  UNIQUE(example_translation_id, translation) ON CONFLICT IGNORE "
        ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE example_links( "
=======
        "CREATE TABLE nonchinese_sentences( "
        "  example_translation_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
        "  sentence TEXT, "
        "  language TEXT, "
        "  UNIQUE(example_translation_id, sentence) ON CONFLICT IGNORE "
        ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE sentence_links( "
>>>>>>> 1ec70d9b (Rename chinese_sentences, nonchinese_sentences)
               "  fk_example_id INTEGER, "
               "  fk_example_translation_id INTEGER, "
               "  fk_source_id INTEGER, "
               "  direct BOOLEAN, "
               "  FOREIGN KEY(fk_example_id) REFERENCES "
<<<<<<< HEAD
               "    examples(example_id), "
               "  FOREIGN KEY(fk_example_translation_id) REFERENCES "
               "    example_translations(example_translation_id), "
=======
               "    chinese_sentences(chinese_sentence_id), "
               "  FOREIGN KEY(fk_example_translation_id) REFERENCES "
               "    nonchinese_sentences(example_translation_id), "
>>>>>>> 1ec70d9b (Rename chinese_sentences, nonchinese_sentences)
               "  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON "
               "    DELETE CASCADE "
               "  UNIQUE(fk_example_id, fk_example_translation_id) "
               "    ON CONFLICT IGNORE "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE definitions_fts using fts5( "
               "	fk_entry_id UNINDEXED, definition)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    // Insert small amounts of data
    query.exec(
        "INSERT INTO entries (traditional, simplified, pinyin, "
        "  jyutping, frequency) "
        "VALUES ('白雲山', '白云山', 'bai2 yun2 shan1', 'baak6 wan4 saan1', "
        "        '0.00')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO sources (sourcename, sourceshortname, version, "
               "  description, legal, link, update_url, other) "
               "VALUES ('CC-CANTO', 'CCY', '2024-03-13', 'dictionary',"
               "  'CC-BY-SA 3.0', '', '', 'words')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions (definition, label, fk_entry_id, "
               "  fk_source_id) "
               "VALUES ('Baiyun Mountain', 'noun', 1, 1)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO entries (traditional, simplified, pinyin, "
               "  jyutping, frequency) "
               "VALUES ('更', '更', 'geng4', 'gang3', '0.00')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions (definition, label, fk_entry_id, "
               "  fk_source_id) "
               "VALUES ('more', 'adverb', 2, 1)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO entries (traditional, simplified, pinyin, "
               "  jyutping, frequency) "
               "VALUES ('越秀', '越秀', 'yue4 xiu4', "
               "  'jyut6 sau3', '0.00')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO sources (sourcename, sourceshortname, version, "
               "  description, legal, link, update_url, other) "
               "VALUES ('Wiktionary', 'WT', '2024-03-13', 'dictionary',"
               "  'CC-BY-SA 4.0', '', '', 'words')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions (definition, label, fk_entry_id, "
               "  fk_source_id) "
               "VALUES ('Yuexiu (a district)', 'name', 3, 2)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO examples (traditional, simplified, "
               "  pinyin, jyutping, language) "
               "VALUES ('從這裡走路去越秀公園要多久？', "
               "  '从这里走路去越秀公园要多久？', "
               "  'cong2 zhe4 li3 zou3 lu4 qu4 yue4 xiu4 gong1 yuan2 yao4 duo1 "
               "jiu3 ？', "
               "  'cung4 ze2 leoi5 zau2 lou6 heoi3 jyut6 sau3 gung1 jyun2 jiu3 "
               "do1 gau2 ？', "
               "  'cmn')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "INSERT INTO example_translations (sentence, language) "
        "VALUES ('How long does it take to walk from here to Yuexiu Park?', "
        "  'eng') ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
<<<<<<< HEAD
    query.exec("INSERT INTO example_links_links (fk_example_id, "
               "  fk_example_translation_id, fk_source_id, direct) "
               "VALUES (1, 1, 2, 1)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions_examples_links (fk_definition_id, "
               "  fk_example_id) "
               "VALUES (3, 1)");
=======
    query.exec("INSERT INTO sentence_links (fk_example_id, "
               "  fk_example_translation_id, fk_source_id, direct) "
               "VALUES (1, 1, 2, 1)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "INSERT INTO definitions_examples_links (fk_definition_id, "
        "  fk_example_id) "
        "VALUES (3, 1)");
>>>>>>> 1ec70d9b (Rename chinese_sentences, nonchinese_sentences)
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE INDEX IF NOT EXISTS entries_simplified_idx ON "
               "entries(simplified);");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE INDEX IF NOT EXISTS entries_jyutping_idx ON "
               "entries(jyutping);");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "CREATE INDEX IF NOT EXISTS entries_pinyin_idx ON entries(pinyin);");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE INDEX IF NOT EXISTS del_fk_example_idx ON "
               "definitions_examples_links(fk_example_id);");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "CREATE INDEX IF NOT EXISTS example_links_fk_example_translation_idx "
        "ON example_links(fk_example_translation_id);");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT "
               "rowid, pinyin, jyutping FROM entries");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions_fts (rowid, fk_entry_id, definition) "
               "SELECT rowid, fk_entry_id, definition FROM definitions");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("PRAGMA user_version=4");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    QSqlDatabase::database(dbCreateConnName).close();
    QSqlDatabase::removeDatabase(dbCreateConnName);
}

void TestSqlUserDataUtils::createV1UserDatabase(const QString &dbPath)
{
    QFile databaseFile{dbPath};
    QDir databaseDir{QFileInfo{databaseFile.fileName()}.absolutePath()};
    QCOMPARE(databaseDir.mkpath(
                 QFileInfo{databaseFile.fileName()}.absolutePath()),
             true);
    if (databaseFile.exists()) {
        QFile::remove(databaseFile.fileName());
    }
    QCOMPARE(databaseFile.open(QIODevice::ReadWrite), true);
    QCOMPARE(databaseFile.exists(), true);
    databaseFile.close();

    QSqlDatabase::addDatabase("QSQLITE", dbCreateConnName);
    QSqlDatabase::database(dbCreateConnName).setDatabaseName(dbPath);
    QSqlDatabase::database(dbCreateConnName).open();
    QSqlQuery query{QSqlDatabase::database(dbCreateConnName)};
    query.exec("CREATE TABLE favourite_words( "
               "  favourite_id INTEGER PRIMARY KEY, "
               "  traditional TEXT, "
               "  simplified TEXT, "
               "  jyutping TEXT, "
               "  pinyin TEXT, "
               "  fk_list_id INTEGER, "
               "  timestamp TEXT, "
               "  FOREIGN KEY (fk_list_id) REFERENCES favourite_lists(list_id) "
               "    ON DELETE CASCADE "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE favourite_lists( "
               "  list_id INTEGER PRIMARY KEY, "
               "  name TEXT, "
               "  timestamp TEXT "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "INSERT INTO favourite_lists VALUES(1,'General',datetime(\"now\"))");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE search_history( "
               "  search_history_id INTEGER PRIMARY KEY, "
               "  search_text TEXT, "
               "  search_options INTEGER, "
               "  timestamp TEXT "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE view_history( "
               "  view_history_id INTEGER PRIMARY KEY, "
               "  traditional TEXT, "
               "  simplified TEXT, "
               "  jyutping TEXT, "
               "  pinyin TEXT, "
               "  timestamp TEXT "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    QSqlDatabase::database(dbCreateConnName).close();
    QSqlDatabase::removeDatabase(dbCreateConnName);
}

void TestSqlUserDataUtils::favouriteUnfavouriteEntry()
{
    TestObserver observer;
    SQLUserDataUtils utils{_manager};

    utils.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    Entry expected{"白云山",
                   "白雲山",
                   "baak6 wan4 saan1",
                   "bai2 yun2 shan1",
                   definitions};
    observer.setExpected(true, expected);
    observer.setExpected({expected});

    // Technically there could be a race between the two condvar waits,
    // but I don't care enough to check for that in a test.
    utils.favouriteEntry(expected);
    {
        // After favouriting an entry, SQLUserDataUtils will
        // check that it has been favourited.
        // The reason for this behaviour is to refresh the
        // status of the "favourite" button in the GUI.
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    {
        // After checking for favouriting, it'll search for
        // all favourite words. The reason for this search
        // is to refresh the favourited words window in the GUI.
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    // Expect the list of favourited entries to be empty
    // after unfavouriting
    observer.setExpected(false, expected);
    observer.setExpected({});
    utils.unfavouriteEntry(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    // Unfavouriting a word should be idempotent
    utils.unfavouriteEntry(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlUserDataUtils::checkIfEntryHasBeenFavourited()
{
    TestObserver observer;
    SQLUserDataUtils utils{_manager};

    utils.registerObserver(&observer);

    Entry expected{"更", "更", "gang3", "geng4", {}};
    observer.setExpected(false, expected);
    observer.setExpected({});

    // Entry should not be favourited when list is empty
    utils.checkIfEntryHasBeenFavourited(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    // Add to list of favourited words, and it should appear.
    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"more", "adverb", {}}}},
    };
    expected = {"更", "更", "gang3", "geng4", definitions};
    observer.setExpected(true, expected);
    observer.setExpected({expected});

    utils.favouriteEntry(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    utils.checkIfEntryHasBeenFavourited(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    observer.setExpected(false, expected);
    observer.setExpected({});
    utils.unfavouriteEntry(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlUserDataUtils::searchForAllFavouritedWords()
{
    TestObserver observer;
    SQLUserDataUtils utils{_manager};

    utils.registerObserver(&observer);

    // Initially, the list of favourited words should be empty.
    observer.setExpected({});
    utils.searchForAllFavouritedWords();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"more", "adverb", {}}}},
    };
    Entry expected = {"更", "更", "gang3", "geng4", definitions};
    observer.setExpected(true, expected);
    observer.setExpected({expected});

    utils.favouriteEntry(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    utils.searchForAllFavouritedWords();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    observer.setExpected(false, expected);
    observer.setExpected({});
    utils.unfavouriteEntry(expected);
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

QTEST_MAIN(TestSqlUserDataUtils)

#include "tst_sqluserdatautils.moc"
