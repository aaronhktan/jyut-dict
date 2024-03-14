#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/entry/entry.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtTest>

namespace {
constexpr auto dbCreateConnName = "dbCreateConn";

class TestObserver : public ISearchObserver
{
public:
    void setExpected(const std::vector<Entry> &entries) { _entries = entries; }
    void setExpected(const std::vector<SourceSentence> &sentences)
    {
        _sentences = sentences;
    }

    void callback(const std::vector<Entry> &entries, bool emptyQuery) override
    {
        if (entries != _entries) {
            testFailed = true;
        }
        resultsReady.notify_one();
    }
    void callback(const std::vector<SourceSentence> &sentences,
                  bool emptyQuery) override
    {
        if (sentences != _sentences) {
            testFailed = true;
        }
        resultsReady.notify_one();
    }

    std::mutex mutex;
    std::condition_variable resultsReady;
    std::atomic_bool testFailed = false;

private:
    std::vector<Entry> _entries;
    std::vector<SourceSentence> _sentences;
};
} // namespace

class TestSqlSearch : public QObject
{
    Q_OBJECT

public:
    TestSqlSearch();
    ~TestSqlSearch();

private slots:
    void searchSimplified();
    void searchSimplifiedNoResults();

    void searchTraditional();
    void searchTraditionalNoResults();

    void searchJyutping();
    void searchJyutpingNoResults();

    void searchPinyin();
    void searchPinyinNoResults();

    void searchEnglish();
    void searchEnglishNoResults();

    void searchAutoDetect();
    void searchAutoDetectNoResults();

    void searchUnique();
    void searchTraditionalSentences();

private:
    void createV3Database(const QString &dbPath);
    void removeDatabase();

    std::shared_ptr<SQLDatabaseManager> _manager;
    SQLDatabaseUtils *_utils;
};

TestSqlSearch::TestSqlSearch()
{
    _manager = std::make_shared<SQLDatabaseManager>();
    _utils = new SQLDatabaseUtils{_manager};

    createV3Database(_manager->getDictionaryDatabasePath());
}

TestSqlSearch::~TestSqlSearch()
{
    _manager->removeAllDatabaseConnections();
    QFile::remove(_manager->getDictionaryDatabasePath());
}

void TestSqlSearch::createV3Database(const QString &dbPath)
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

void TestSqlSearch::searchSimplified()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    std::vector<Entry> expected = {
        {"白云山", "白雲山", "baak6 wan4 saan1", "bai2 yun2 shan1", definitions},
    };
    observer.setExpected(expected);

    search.searchSimplified("白云山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("白?山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("*山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("白*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    definitions = {
        {"CC-CANTO", {{"more", "adverb", {}}}},
    };
    expected = {
        {"更", "更", "gang3", "geng4", definitions},
    };
    observer.setExpected(expected);
    // Test for Unicode normalization: the string being searched
    // is U+F901, but the one in the dictionary is U+66F4
    search.searchSimplified("更");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    std::vector<Sentence::TargetSentence> translations = {
        {"How long does it take to walk from here to Yuexiu Park?", "eng", true},
    };
    std::vector<SentenceSet> translationSets = {
        {"Wiktionary", translations},
    };
    std::vector<SourceSentence> sentences = {
        {"cmn",
         "从这里走路去越秀公园要多久？",
         "從這裡走路去越秀公園要多久？",
         "cung4 ze2 leoi5 zau2 lou6 heoi3 jyut6 sau3 gung1 jyun2 jiu3 do1 "
         "gau2 ？",
         "cong2 zhe4 li3 zou3 lu4 qu4 yue4 xiu4 gong1 yuan2 yao4 duo1 jiu3 ？",
         translationSets},
    };
    definitions = {
        {"Wiktionary", {{"Yuexiu (a district)", "name", sentences}}},
    };
    expected = {
        {"越秀", "越秀", "jyut6 sau3", "yue4 xiu4", definitions},
    };
    observer.setExpected(expected);
    search.searchSimplified("越秀");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("越?");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("*秀");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("越*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchSimplifiedNoResults()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<Entry> expected;
    observer.setExpected(expected);

    search.searchSimplified("白雲山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("白???");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("*白白白白山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchSimplified("白雲*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchTraditional()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    std::vector<Entry> expected = {
        {"白云山", "白雲山", "baak6 wan4 saan1", "bai2 yun2 shan1", definitions},
    };
    observer.setExpected(expected);

    search.searchTraditional("白雲山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("白?山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("*山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("白*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    definitions = {
        {"CC-CANTO", {{"more", "adverb", {}}}},
    };
    expected = {
        {"更", "更", "gang3", "geng4", definitions},
    };
    observer.setExpected(expected);
    // Test for Unicode normalization: the string being searched
    // is U+F901, but the one in the dictionary is U+66F4
    search.searchTraditional("更");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    std::vector<Sentence::TargetSentence> translations = {
        {"How long does it take to walk from here to Yuexiu Park?", "eng", true},
    };
    std::vector<SentenceSet> translationSets = {
        {"Wiktionary", translations},
    };
    std::vector<SourceSentence> sentences = {
        {"cmn",
         "从这里走路去越秀公园要多久？",
         "從這裡走路去越秀公園要多久？",
         "cung4 ze2 leoi5 zau2 lou6 heoi3 jyut6 sau3 gung1 jyun2 jiu3 do1 "
         "gau2 ？",
         "cong2 zhe4 li3 zou3 lu4 qu4 yue4 xiu4 gong1 yuan2 yao4 duo1 jiu3 ？",
         translationSets},
    };
    definitions = {
        {"Wiktionary", {{"Yuexiu (a district)", "name", sentences}}},
    };
    expected = {
        {"越秀", "越秀", "jyut6 sau3", "yue4 xiu4", definitions},
    };
    observer.setExpected(expected);
    search.searchTraditional("越秀");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("越?");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("*秀");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("越*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchTraditionalNoResults()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<Entry> expected;
    observer.setExpected(expected);

    search.searchTraditional("白云山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("白???");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("*白白白白山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchTraditional("白云*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchJyutping()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    std::vector<Entry> expected = {
        {"白云山", "白雲山", "baak6 wan4 saan1", "bai2 yun2 shan1", definitions},
    };
    observer.setExpected(expected);

    search.searchJyutping("baak6 wan4 saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("baak6 wan4 saan1$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("\"baak6 wan4 saan1\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("baak6'wan4saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("baakwansaan");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("baakwan");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("baak6*saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("*saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("baak6*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    std::vector<Sentence::TargetSentence> translations = {
        {"How long does it take to walk from here to Yuexiu Park?", "eng", true},
    };
    std::vector<SentenceSet> translationSets = {
        {"Wiktionary", translations},
    };
    std::vector<SourceSentence> sentences = {
        {"cmn",
         "从这里走路去越秀公园要多久？",
         "從這裡走路去越秀公園要多久？",
         "cung4 ze2 leoi5 zau2 lou6 heoi3 jyut6 sau3 gung1 jyun2 jiu3 do1 "
         "gau2 ？",
         "cong2 zhe4 li3 zou3 lu4 qu4 yue4 xiu4 gong1 yuan2 yao4 duo1 jiu3 ？",
         translationSets},
    };
    definitions = {
        {"Wiktionary", {{"Yuexiu (a district)", "name", sentences}}},
    };
    expected = {
        {"越秀", "越秀", "jyut6 sau3", "yue4 xiu4", definitions},
    };
    observer.setExpected(expected);
    search.searchJyutping("jyut6 sau3");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("jyut? sau?");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("*sau?");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("jyut?*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("jyut? ????");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("????? ????$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchJyutpingNoResults()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<Entry> expected;
    observer.setExpected(expected);

    search.searchJyutping("白云山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("白???");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("*白白白白山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("白云*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("\"jyut6\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("jyut6$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchPinyin()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    std::vector<Entry> expected = {
        {"白云山", "白雲山", "baak6 wan4 saan1", "bai2 yun2 shan1", definitions},
    };
    observer.setExpected(expected);

    search.searchPinyin("bai2 yun2 shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("bai2 yun2 shan1$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("\"bai2 yun2 shan1\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("bai2'yun2shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("baiyunshan");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("baiyun");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("bai2*shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("*shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("bai2*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    std::vector<Sentence::TargetSentence> translations = {
        {"How long does it take to walk from here to Yuexiu Park?", "eng", true},
    };
    std::vector<SentenceSet> translationSets = {
        {"Wiktionary", translations},
    };
    std::vector<SourceSentence> sentences = {
        {"cmn",
         "从这里走路去越秀公园要多久？",
         "從這裡走路去越秀公園要多久？",
         "cung4 ze2 leoi5 zau2 lou6 heoi3 jyut6 sau3 gung1 jyun2 jiu3 do1 "
         "gau2 ？",
         "cong2 zhe4 li3 zou3 lu4 qu4 yue4 xiu4 gong1 yuan2 yao4 duo1 jiu3 ？",
         translationSets},
    };
    definitions = {
        {"Wiktionary", {{"Yuexiu (a district)", "name", sentences}}},
    };
    expected = {
        {"越秀", "越秀", "jyut6 sau3", "yue4 xiu4", definitions},
    };
    observer.setExpected(expected);
    search.searchPinyin("yue4 xiu4");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("yue? xiu?");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("*xiu?");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("yue?*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("yue? ????");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("???? ????$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchPinyinNoResults()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<Entry> expected;
    observer.setExpected(expected);

    search.searchPinyin("白云山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("白???");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("*白白白白山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("白云*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("\"yue4\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("yue4$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchEnglish()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    std::vector<Entry> expected = {
        {"白云山", "白雲山", "baak6 wan4 saan1", "bai2 yun2 shan1", definitions},
    };
    observer.setExpected(expected);

    search.searchEnglish("Baiyun");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchEnglish("mountain");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchEnglish("baiyun");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchEnglish("\"baiyun mountain\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    std::vector<Sentence::TargetSentence> translations = {
        {"How long does it take to walk from here to Yuexiu Park?", "eng", true},
    };
    std::vector<SentenceSet> translationSets = {
        {"Wiktionary", translations},
    };
    std::vector<SourceSentence> sentences = {
        {"cmn",
         "从这里走路去越秀公园要多久？",
         "從這裡走路去越秀公園要多久？",
         "cung4 ze2 leoi5 zau2 lou6 heoi3 jyut6 sau3 gung1 jyun2 jiu3 do1 "
         "gau2 ？",
         "cong2 zhe4 li3 zou3 lu4 qu4 yue4 xiu4 gong1 yuan2 yao4 duo1 jiu3 ？",
         translationSets},
    };
    definitions = {
        {"Wiktionary", {{"Yuexiu (a district)", "name", sentences}}},
    };
    expected = {
        {"越秀", "越秀", "jyut6 sau3", "yue4 xiu4", definitions},
    };
    observer.setExpected(expected);
    search.searchEnglish("yuexiu");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchEnglish("a");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchEnglish("district");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchEnglishNoResults()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<Entry> expected;
    observer.setExpected(expected);

    search.searchJyutping("snoofles");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchJyutping("\"baiyun\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchAutoDetect()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    std::vector<Entry> expected = {
        {"白云山", "白雲山", "baak6 wan4 saan1", "bai2 yun2 shan1", definitions},
    };
    observer.setExpected(expected);

    search.searchAutoDetect("白雲山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("白云山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("白?山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("*山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("白*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baak6 wan4 saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baak6 wan4 saan1$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("\"baak6 wan4 saan1\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baak6'wan4saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baakwansaan");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baakwan");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baak6*saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("*saan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baak6*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("bai2 yun2 shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("bai2 yun2 shan1$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("\"bai2 yun2 shan1\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("bai2'yun2shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baiyunshan");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("baiyun");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("bai2*shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("*shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("bai2*");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("mountain");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("\"baiyun mountain\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    definitions = {
        {"CC-CANTO", {{"more", "adverb", {}}}},
    };
    expected = {
        {"更", "更", "gang3", "geng4", definitions},
    };
    observer.setExpected(expected);
    // Test for Unicode normalization: the string being searched
    // is U+F901, but the one in the dictionary is U+66F4
    search.searchAutoDetect("更");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchAutoDetectNoResults()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    search.searchAutoDetect("白???");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("*白白白白山");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("\"jyut6\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchAutoDetect("jyut6$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("\"yue4\"");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
    search.searchPinyin("yue4$");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchUnique()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<DefinitionsSet> definitions = {
        {"CC-CANTO", {{"Baiyun Mountain", "noun", {}}}},
    };
    std::vector<Entry> expected = {
        {"白云山", "白雲山", "baak6 wan4 saan1", "bai2 yun2 shan1", definitions},
    };
    observer.setExpected(expected);
    search.searchByUnique("白云山",
                          "白雲山",
                          "baak6 wan4 saan1",
                          "bai2 yun2 shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    expected = {};
    observer.setExpected(expected);
    search.searchByUnique("白雲山",
                          "白雲山",
                          "baak6 wan4 saan1",
                          "bai2 yun2 shan1");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    definitions = {
        {"CC-CANTO", {{"more", "adverb", {}}}},
    };
    expected = {
        {"更", "更", "gang3", "geng4", definitions},
    };
    observer.setExpected(expected);
    // Test for Unicode normalization: the string being searched
    // is U+F901, but the one in the dictionary is U+66F4
    search.searchByUnique("更", "更", "gang3", "geng4");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

void TestSqlSearch::searchTraditionalSentences()
{
    TestObserver observer;
    SQLSearch search{_manager};

    search.registerObserver(&observer);

    std::vector<Sentence::TargetSentence> translations = {
        {"How long does it take to walk from here to Yuexiu Park?", "eng", true},
    };
    std::vector<SentenceSet> translationSets = {
        {"Wiktionary", translations},
    };
    std::vector<SourceSentence> sentences = {
        {"cmn",
         "从这里走路去越秀公园要多久？",
         "從這裡走路去越秀公園要多久？",
         "cung4 ze2 leoi5 zau2 lou6 heoi3 jyut6 sau3 gung1 jyun2 jiu3 do1 "
         "gau2 ？",
         "cong2 zhe4 li3 zou3 lu4 qu4 yue4 xiu4 gong1 yuan2 yao4 duo1 jiu3 ？",
         translationSets},
    };
    observer.setExpected(sentences);
    search.searchTraditionalSentences("公園");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }

    // Test for Unicode normalization: the string being searched
    // is U+F937, but the one in the dictionary is U+8DEF
    search.searchTraditionalSentences("路");
    {
        std::unique_lock lock{observer.mutex};
        observer.resultsReady.wait(lock);
        QCOMPARE(observer.testFailed, false);
    }
}

QTEST_MAIN(TestSqlSearch)

#include "tst_sqlsearch.moc"
