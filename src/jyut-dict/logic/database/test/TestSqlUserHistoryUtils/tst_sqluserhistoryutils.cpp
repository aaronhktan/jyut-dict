#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserhistoryutils.h"

#include <QCoreApplication>
#include <QtTest>
#include <QSqlError>
#include <QSqlQuery>

#include <thread>

namespace {
constexpr auto dbCreateConnName = "dbCreateConn";

class TestObserver : public ISearchObserver
{
public:
    void setExpected(const std::vector<searchTermHistoryItem> &results)
    {
        _results = results;
    }
    void setExpected(const std::vector<Entry> &entries) { _entries = entries; }

    void callback(const std::vector<searchTermHistoryItem> &results,
                  bool emptyQuery) override
    {
        if (results != _results) {
            testFailed = true;
        }
        resultsReady.notify_one();
    }
    void callback(const std::vector<Entry> &entries, bool emptyQuery) override
    {
        if (entries != _entries) {
            testFailed = true;
        }
        resultsReady.notify_one();
    }

    std::mutex mutex;
    std::condition_variable resultsReady;
    std::atomic_bool testFailed = false;

private:
    bool _entryExists;
    std::vector<searchTermHistoryItem> _results;
    std::vector<Entry> _entries;
};
} // namespace

class TestSqlUserHistoryUtils : public QObject
{
    Q_OBJECT

public:
    TestSqlUserHistoryUtils();
    ~TestSqlUserHistoryUtils();

private slots:
    void searchHistory();
    void viewHistory();

private:
    void createV3Database(const QString &dbPath);
    void createV1UserDatabase(const QString &dbPath);

    std::shared_ptr<SQLDatabaseManager> _manager;
};

TestSqlUserHistoryUtils::TestSqlUserHistoryUtils() {
    _manager = std::make_shared<SQLDatabaseManager>();

    createV3Database(_manager->getDictionaryDatabasePath());
    createV1UserDatabase(_manager->getUserDatabasePath());
}

TestSqlUserHistoryUtils::~TestSqlUserHistoryUtils() {
    _manager->removeAllDatabaseConnections();
    QFile::remove(_manager->getDictionaryDatabasePath());
    QFile::remove(_manager->getUserDatabasePath());
}

void TestSqlUserHistoryUtils::createV3Database(const QString &dbPath)
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
    query.exec("CREATE TABLE chinese_sentences( "
               "  chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
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
        "CREATE TABLE definitions_chinese_sentences_links( "
        "  fk_definition_id INTEGER, "
        "  fk_chinese_sentence_id INTEGER, "
        "  FOREIGN KEY(fk_definition_id) REFERENCES definitions(definition_id) "
        "ON DELETE CASCADE, "
        "  FOREIGN KEY(fk_chinese_sentence_id) REFERENCES "
        "    chinese_sentences(chinese_sentence_id) "
        "  UNIQUE(fk_definition_id, fk_chinese_sentence_id) ON CONFLICT IGNORE "
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
        "CREATE TABLE nonchinese_sentences( "
        "  non_chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
        "  sentence TEXT, "
        "  language TEXT, "
        "  UNIQUE(non_chinese_sentence_id, sentence) ON CONFLICT IGNORE "
        ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE sentence_links( "
               "  fk_chinese_sentence_id INTEGER, "
               "  fk_non_chinese_sentence_id INTEGER, "
               "  fk_source_id INTEGER, "
               "  direct BOOLEAN, "
               "  FOREIGN KEY(fk_chinese_sentence_id) REFERENCES "
               "    chinese_sentences(chinese_sentence_id), "
               "  FOREIGN KEY(fk_non_chinese_sentence_id) REFERENCES "
               "    nonchinese_sentences(non_chinese_sentence_id), "
               "  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON "
               "    DELETE CASCADE "
               "  UNIQUE(fk_chinese_sentence_id, fk_non_chinese_sentence_id) "
               "    ON CONFLICT IGNORE "
               ") ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE definitions_fts using fts5(fk_entry_id, "
               "  definition)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    // Insert small amounts of data
    query.exec("INSERT INTO entries (traditional, simplified, pinyin, "
               "  jyutping, frequency) "
               "VALUES ('白雲山', '白云山', 'bai2 yun2 shan1', "
               "  'baak6 wan4 saan1', '0.00')");
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
    query.exec("INSERT INTO chinese_sentences (traditional, simplified, "
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
        "INSERT INTO nonchinese_sentences (sentence, language) "
        "VALUES ('How long does it take to walk from here to Yuexiu Park?', "
        "  'eng') ");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO sentence_links (fk_chinese_sentence_id, "
               "  fk_non_chinese_sentence_id, fk_source_id, direct) "
               "VALUES (1, 1, 2, 1)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "INSERT INTO definitions_chinese_sentences_links (fk_definition_id, "
        "  fk_chinese_sentence_id) "
        "VALUES (3, 1)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT "
               "rowid, pinyin, jyutping FROM entries");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions_fts (fk_entry_id, definition) "
               "SELECT rowid, definition FROM definitions");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("PRAGMA user_version=3");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    QSqlDatabase::database(dbCreateConnName).close();
    QSqlDatabase::removeDatabase(dbCreateConnName);
}

void TestSqlUserHistoryUtils::createV1UserDatabase(const QString &dbPath)
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

void TestSqlUserHistoryUtils::searchHistory() {
    TestObserver observer;
    SQLUserHistoryUtils utils{_manager};

    utils.registerObserver(&observer);

    std::vector<searchTermHistoryItem> results = {
        {"hello", static_cast<int>(SearchParameters::ENGLISH)}};
    observer.setExpected(results);
    utils.addSearchToHistory(results.back().first, results.back().second);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    utils.searchAllSearchHistory();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    results.emplace_back("旺角", static_cast<int>(SearchParameters::TRADITIONAL));
    observer.setExpected(results);
    utils.addSearchToHistory(results.back().first, results.back().second);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    utils.searchAllSearchHistory();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    results = {};
    observer.setExpected(results);
    utils.clearAllSearchHistory();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    utils.searchAllSearchHistory();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlUserHistoryUtils::viewHistory()
{
    TestObserver observer;
    SQLUserHistoryUtils utils{_manager};

    utils.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    Entry originalEntry{"白云山",
                        "白雲山",
                        "baak6 wan4 saan1",
                        "bai2 yun2 shan1",
                        definitions};

    // The result we get back from the database should have no definitions.
    Entry expectedEntry{"白云山",
                        "白雲山",
                        "baak6 wan4 saan1",
                        "bai2 yun2 shan1",
                        {}};
    std::vector<Entry> expected = {expectedEntry};
    observer.setExpected(expected);
    utils.addViewToHistory(originalEntry);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    utils.searchAllViewHistory();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    expected.emplace_back(Entry{"更", "更", "gang3", "geng4", {}});
    observer.setExpected(expected);
    utils.addViewToHistory(expected.back());
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    utils.searchAllViewHistory();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    expected = {};
    observer.setExpected(expected);
    utils.clearAllViewHistory();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    utils.searchAllViewHistory();
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

QTEST_MAIN(TestSqlUserHistoryUtils)

#include "tst_sqluserhistoryutils.moc"
