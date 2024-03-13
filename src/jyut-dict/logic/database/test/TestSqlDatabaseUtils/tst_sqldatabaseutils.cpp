#include <QtTest>

#include "logic/database/sqldatabaseutils.h"

#include <QSqlError>
#include <QSqlQuery>

#include <unordered_set>

namespace {
constexpr auto dbCreateConnName = "dbCreateConn";
}

class TestSqlDatabaseUtils : public QObject
{
    Q_OBJECT

public:
    TestSqlDatabaseUtils();
    ~TestSqlDatabaseUtils();

private slots:
    void updateDatabaseFromV1();
    void updateDatabaseFromV2();

    void addAndRemoveSources();
    void readSources();

private:
    void createV1Database(const QString &dbPath);
    void createV2Database(const QString &dbPath);
    void createV3Database(const QString &dbPath);
    void removeDatabase();

    std::shared_ptr<SQLDatabaseManager> _manager;
    SQLDatabaseUtils *_utils;
};

TestSqlDatabaseUtils::TestSqlDatabaseUtils()
{
    _manager = std::make_shared<SQLDatabaseManager>();
    _utils = new SQLDatabaseUtils{_manager};
}

TestSqlDatabaseUtils::~TestSqlDatabaseUtils()
{
    removeDatabase(); // Just in case!
}

void TestSqlDatabaseUtils::createV1Database(const QString &dbPath)
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
    query.exec(
        "CREATE TABLE definitions( "
        "  definition_id INTEGER PRIMARY KEY, "
        "  definition TEXT, "
        "  fk_entry_id INTEGER, "
        "  fk_source_id INTEGER, "
        "  FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON UPDATE "
        "    CASCADE, "
        "  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE "
        "    CASCADE, "
        "  UNIQUE(definition, fk_entry_id, fk_source_id) ON CONFLICT IGNORE "
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
    query.exec("CREATE VIRTUAL TABLE definitions_fts using fts5(definition)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT "
               "rowid, pinyin, jyutping FROM entries");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions_fts (rowid, definition) "
               "select rowid, definition FROM definitions");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("PRAGMA user_version=1");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    QSqlDatabase::database(dbCreateConnName).close();
    QSqlDatabase::removeDatabase(dbCreateConnName);
}

void TestSqlDatabaseUtils::createV2Database(const QString &dbPath)
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
    query.exec(
        "CREATE TABLE chinese_sentences( chinese_sentence_id INTEGER PRIMARY "
        "KEY ON CONFLICT IGNORE, traditional TEXT, simplified TEXT, pinyin "
        "TEXT, jyutping TEXT, language TEXT, UNIQUE(traditional, simplified, "
        "pinyin, jyutping) ON CONFLICT IGNORE)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec(
        "CREATE TABLE definitions( "
        "  definition_id INTEGER PRIMARY KEY, "
        "  definition TEXT, "
        "  fk_entry_id INTEGER, "
        "  fk_source_id INTEGER, "
        "  FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON UPDATE "
        "    CASCADE, "
        "  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE "
        "    CASCADE, "
        "  UNIQUE(definition, fk_entry_id, fk_source_id) ON CONFLICT IGNORE "
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
        "CREATE TABLE nonchinese_sentences( non_chinese_sentence_id INTEGER "
        "PRIMARY KEY ON CONFLICT IGNORE, sentence TEXT, language TEXT, "
        "UNIQUE(non_chinese_sentence_id, sentence) ON CONFLICT IGNORE)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE TABLE sentence_links( fk_chinese_sentence_id INTEGER, "
               "fk_non_chinese_sentence_id INTEGER, fk_source_id INTEGER, "
               "direct BOOLEAN, FOREIGN KEY(fk_chinese_sentence_id) REFERENCES "
               "    chinese_sentences(chinese_sentence_id), FOREIGN "
               "KEY(fk_non_chinese_sentence_id) REFERENCES     "
               "nonchinese_sentences(non_chinese_sentence_id), FOREIGN "
               "KEY(fk_source_id) REFERENCES sources(source_id) ON     DELETE "
               "CASCADE )");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE definitions_fts using fts5(definition)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT "
               "rowid, pinyin, jyutping FROM entries");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions_fts (rowid, definition) "
               "select rowid, definition FROM definitions");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("PRAGMA user_version=2");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    QSqlDatabase::database(dbCreateConnName).close();
    QSqlDatabase::removeDatabase(dbCreateConnName);
}

void TestSqlDatabaseUtils::createV3Database(const QString &dbPath)
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
    query.exec("CREATE VIRTUAL TABLE definitions_fts using fts5(definition)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("CREATE VIRTUAL TABLE entries_fts using fts5(pinyin, jyutping)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    // Insert small amounts of data
    query.exec("INSERT INTO entries (traditional, simplified, pinyin, "
               "  jyutping, frequency) "
               "VALUES ('白雲山', '白云山', 'bai2 yun2 shan1', 'baak6 wan4 "
               "  saan1', '0.00')");
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

    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) SELECT "
               "rowid, pinyin, jyutping FROM entries");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions_fts (rowid, definition) "
               "SELECT rowid, definition FROM definitions");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("PRAGMA user_version=3");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    QSqlDatabase::database(dbCreateConnName).close();
    QSqlDatabase::removeDatabase(dbCreateConnName);
}

void TestSqlDatabaseUtils::removeDatabase()
{
    _manager->closeDatabase();
    QFile::remove(_manager->getDictionaryDatabasePath());
}

void TestSqlDatabaseUtils::updateDatabaseFromV1()
{
    removeDatabase();
    createV1Database(_manager->getDictionaryDatabasePath());
    _utils->updateDatabase();

    QSqlQuery query{_manager->getDatabase()};
    query.exec("PRAGMA user_version");
    int version = -1;
    while (query.next()) {
        version = query.value(0).toInt();
    }
    QCOMPARE(version, CURRENT_DATABASE_VERSION);

    // Check stuff added in v2
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND "
               "name='nonchinese_sentences'");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "nonchinese_sentences");

    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND "
               "name='chinese_sentences'");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "chinese_sentences");

    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND "
               "name='sentence_links'");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "sentence_links");

    // Check stuff added in v3
    query.exec(
        "SELECT * FROM pragma_table_info('definitions_fts') AS table_info");
    std::unordered_set<QString> expected_columns{"fk_entry_id", "definition"};
    while (query.next()) {
        QCOMPARE(expected_columns.find(query.value(1).toString())
                     == expected_columns.end(),
                 false);
        expected_columns.erase(query.value(1).toString());
    }
    QCOMPARE(expected_columns.empty(), true);

    query.exec("SELECT * FROM pragma_table_info('definitions') AS table_info");
    expected_columns = {"definition_id",
                        "definition",
                        "label",
                        "fk_entry_id",
                        "fk_source_id"};
    while (query.next()) {
        QCOMPARE(expected_columns.find(query.value(1).toString())
                     == expected_columns.end(),
                 false);
        expected_columns.erase(query.value(1).toString());
    }
    QCOMPARE(expected_columns.empty(), true);

    removeDatabase();
}

void TestSqlDatabaseUtils::updateDatabaseFromV2()
{
    removeDatabase();
    createV2Database(_manager->getDictionaryDatabasePath());
    _utils->updateDatabase();

    QSqlQuery query{_manager->getDatabase()};
    query.exec("PRAGMA user_version");
    int version = -1;
    while (query.next()) {
        version = query.value(0).toInt();
    }
    QCOMPARE(version, CURRENT_DATABASE_VERSION);

    query.exec(
        "SELECT * FROM pragma_table_info('definitions_fts') AS table_info");
    std::unordered_set<QString> expected_columns{"fk_entry_id", "definition"};
    while (query.next()) {
        QCOMPARE(expected_columns.find(query.value(1).toString())
                     == expected_columns.end(),
                 false);
        expected_columns.erase(query.value(1).toString());
    }
    QCOMPARE(expected_columns.empty(), true);

    query.exec("SELECT * FROM pragma_table_info('definitions') AS table_info");
    expected_columns = {"definition_id",
                        "definition",
                        "label",
                        "fk_entry_id",
                        "fk_source_id"};
    while (query.next()) {
        QCOMPARE(expected_columns.find(query.value(1).toString())
                     == expected_columns.end(),
                 false);
        expected_columns.erase(query.value(1).toString());
    }
    QCOMPARE(expected_columns.empty(), true);

    removeDatabase();
}

void TestSqlDatabaseUtils::addAndRemoveSources()
{
    removeDatabase();
    createV3Database(_manager->getDictionaryDatabasePath());

    QSqlQuery query{_manager->getDatabase()};
    query.exec("PRAGMA user_version");
    int version = -1;
    while (query.next()) {
        version = query.value(0).toInt();
    }
    QCOMPARE(version, CURRENT_DATABASE_VERSION);

    query.exec("SELECT sourcename FROM sources");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "CC-CANTO");

    query.exec("SELECT traditional FROM entries");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "白雲山");

    query.exec("SELECT definition FROM definitions");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "Baiyun Mountain");

    _utils->removeSource("CC-CANTO");

    query.exec("SELECT COUNT(*) FROM sources");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toInt(), 0);

    query.exec("SELECT COUNT(*) FROM entries");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toInt(), 0);

    query.exec("SELECT COUNT(*) FROM entries");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toInt(), 0);

    // Create a second database to attach
    QDir databaseDir{
        QFileInfo{_manager->getDictionaryDatabasePath()}.absolutePath()};
    QCOMPARE(databaseDir.mkpath(QFileInfo{_manager->getDictionaryDatabasePath()}
                                    .absolutePath()),
             true);
    QFile secondDatabaseFile{databaseDir.absolutePath() + "/dict_2.db"};
    if (secondDatabaseFile.exists()) {
        QFile::remove(secondDatabaseFile.fileName());
    }
    QCOMPARE(secondDatabaseFile.open(QIODevice::ReadWrite), true);
    QCOMPARE(secondDatabaseFile.exists(), true);
    secondDatabaseFile.close();

    query.exec("INSERT INTO entries (traditional, simplified, pinyin, "
               "  jyutping, frequency) "
               "VALUES ('越秀', '越秀', 'jyut6 sau3', "
               "  'yue4 xiu4', '0.00')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO sources (sourcename, sourceshortname, version, "
               "  description, legal, link, update_url, other) "
               "VALUES ('Wiktionary', 'WT', '2024-03-13', 'dictionary',"
               "  'CC-BY-SA 4.0', '', '', 'words')");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);
    query.exec("INSERT INTO definitions (definition, label, fk_entry_id, "
               "  fk_source_id) "
               "VALUES ('Yuexiu (a district)', 'name', 1, 1)");
    QCOMPARE(query.lastError().type(), QSqlError::NoError);

    // Add source to original database
    _utils->addSource(secondDatabaseFile.fileName().toStdString());

    // Contents of the original database should now match the second database
    query.exec("SELECT sourcename FROM sources");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "Wiktionary");

    query.exec("SELECT traditional FROM entries");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "越秀");

    query.exec("SELECT definition FROM definitions");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toString(), "Yuexiu (a district)");

    query.exec("SELECT COUNT(*) FROM sources");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toInt(), 1);

    query.exec("SELECT COUNT(*) FROM entries");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toInt(), 1);

    query.exec("SELECT COUNT(*) FROM entries");
    QCOMPARE(query.first(), true);
    QCOMPARE(query.value(0).toInt(), 1);

    QCOMPARE(secondDatabaseFile.remove(), true);
    removeDatabase();
}

void TestSqlDatabaseUtils::readSources()
{
    removeDatabase();
    createV3Database(_manager->getDictionaryDatabasePath());

    std::vector<std::pair<std::string, std::string>> sources;
    _utils->readSources(sources);

    QCOMPARE(sources.size(), 1);
    QCOMPARE(QString::fromStdString(sources.at(0).first), "CC-CANTO");
    QCOMPARE(QString::fromStdString(sources.at(0).second), "CCY");

    std::vector<DictionaryMetadata> metadata;
    _utils->readSources(metadata);
    QCOMPARE(metadata.size(), 1);
    QCOMPARE(QString::fromStdString(metadata.at(0).getName()), "CC-CANTO");
    QCOMPARE(QString::fromStdString(metadata.at(0).getVersion()), "2024-03-13");

    removeDatabase();
}

QTEST_MAIN(TestSqlDatabaseUtils)

#include "tst_sqldatabaseutils.moc"
