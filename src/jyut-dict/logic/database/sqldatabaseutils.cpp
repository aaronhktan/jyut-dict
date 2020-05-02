#include "sqldatabaseutils.h"

#include "logic/utils/utils.h"

#include <QtSql>

#include <chrono>
#include <thread>

SQLDatabaseUtils::SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager)
    : QObject()
{
    _manager = manager;
}

bool SQLDatabaseUtils::migrateDatabaseFromOneToTwo(void)
{
    QSqlQuery query{_manager->getDatabase()};

    query.exec("BEGIN TRANSACTION");

    query.exec(
        "CREATE TABLE IF NOT EXISTS chinese_sentences( "
        "chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
        "traditional TEXT, "
        "simplified TEXT, "
        "pinyin TEXT, "
        "jyutping TEXT, "
        "language TEXT, "
        "UNIQUE(traditional, simplified, pinyin, jyutping) ON CONFLICT IGNORE)");

    query.exec(
        "CREATE TABLE IF NOT EXISTS nonchinese_sentences( "
        "non_chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
        "sentence TEXT, "
        "language TEXT, "
        "UNIQUE(non_chinese_sentence_id, sentence) ON CONFLICT IGNORE)");

    query.exec("CREATE TABLE IF NOT EXISTS sentence_links( "
               "fk_chinese_sentence_id INTEGER, "
               "fk_non_chinese_sentence_id INTEGER, "
               "fk_source_id INTEGER, "
               "direct BOOLEAN, "
               "FOREIGN KEY(fk_chinese_sentence_id) REFERENCES "
               "    chinese_sentences(chinese_sentence_id), "
               "FOREIGN KEY(fk_non_chinese_sentence_id) REFERENCES "
               "    nonchinese_sentences(non_chinese_sentence_id), "
               "FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON "
               "    DELETE CASCADE "
               ")");

    query.exec("COMMIT");

    // For some reason, PRAGMA user_version=? doesn't work; so just use a QString
    QString queryString = "PRAGMA user_version=%1";
    query.prepare(queryString.arg(CURRENT_DATABASE_VERSION));
    query.exec();

    query.exec("PRAGMA user_version");
    int version = -1;
    while (query.next()) {
        version = query.value(0).toInt();
    }
    return true;
}

bool SQLDatabaseUtils::updateDatabase(void)
{
    QSqlQuery query{_manager->getDatabase()};

    query.exec("PRAGMA user_version");
    int version = -1;
    while (query.next()) {
        version = query.value(0).toInt();
    }

    if (version != CURRENT_DATABASE_VERSION) {
        switch (version) {
        case -1:
        case 1:
            migrateDatabaseFromOneToTwo();
            [[clang::fallthrough]]; // TODO: remove this!
        default:
            break;
        }
    }

    return true;
}

bool SQLDatabaseUtils::readSources(std::vector<std::pair<std::string, std::string>> &sources)
{
    QSqlQuery query{_manager->getDatabase()};
    query.exec("SELECT sourcename, sourceshortname FROM sources");

    if (query.lastError().isValid()) {
        return false;
    }

    int sourcenameIndex = query.record().indexOf("sourcename");
    int sourceshortnameIndex = query.record().indexOf("sourceshortname");
    while (query.next()) {
        std::string sourcename
            = query.value(sourcenameIndex).toString().toStdString();
        std::string sourceshortname
            = query.value(sourceshortnameIndex).toString().toStdString();

        sources.push_back(
            std::pair<std::string, std::string>(sourcename, sourceshortname));
    }

    return true;
}

bool SQLDatabaseUtils::readSources(std::vector<DictionaryMetadata> &sources)
{
    QSqlQuery query{_manager->getDatabase()};
    query.exec("SELECT sourcename, version, description, legal, link, other "
               "FROM sources");

    if (query.lastError().isValid()) {
        return false;
    }

    int sourcenameIndex = query.record().indexOf("sourcename");
    int versionIndex = query.record().indexOf("version");
    int descriptionIndex = query.record().indexOf("description");
    int legalIndex = query.record().indexOf("legal");
    int linkIndex = query.record().indexOf("link");
    int otherIndex = query.record().indexOf("other");

    while (query.next()) {
        std::string source
            = query.value(sourcenameIndex).toString().toStdString();
        std::string version = query.value(versionIndex).toString().toStdString();
        std::string description
            = query.value(descriptionIndex).toString().toStdString();
        std::string legal = query.value(legalIndex).toString().toStdString();
        std::string link = query.value(linkIndex).toString().toStdString();
        std::string other = query.value(otherIndex).toString().toStdString();

        DictionaryMetadata dictionary{source,
                                      version,
                                      description,
                                      legal,
                                      link,
                                      other};

        sources.push_back(dictionary);
    }

    return true;
}

bool SQLDatabaseUtils::deleteSourceFromDatabase(std::string source)
{
    QSqlQuery query{_manager->getDatabase()};
    query.exec("PRAGMA foreign_keys = ON");

    query.exec("BEGIN TRANSACTION");

    query.exec("DROP INDEX fk_entry_id_index");

    query.prepare("DELETE FROM sources WHERE sourcename = ?");
    query.addBindValue(source.c_str());
    query.exec();

    query.exec("COMMIT");
    query.exec("PRAGMA foreign_keys = OFF");

    return true;
}

bool SQLDatabaseUtils::removeDefinitionsFromDatabase(void)
{
    QSqlQuery query{_manager->getDatabase()};

    emit deletingDefinitions();

    query.exec("DELETE FROM definitions_fts");
    query.exec("DELETE FROM entries_fts");

    query.exec("SELECT COUNT(entries.entry_id) AS count "
               "FROM entries "
               "WHERE entries.entry_id IN "
               " (SELECT entries.entry_id FROM entries "
               " LEFT JOIN definitions "
               " ON definitions.fk_entry_id=entries.entry_id "
               " WHERE definitions.fk_entry_id IS NULL)");
    int numberToDelete = 0;
    while (query.next()) {
        numberToDelete = query.value(0).toInt();
        emit totalToDelete(numberToDelete);
    }

    query.exec("SAVEPOINT row_delection");
    for (int i = 0; i < numberToDelete; i += 1000) {
        query.exec("DELETE FROM entries WHERE entry_id IN "
                   " (SELECT entries.entry_id FROM entries "
                   "  LEFT JOIN definitions "
                   "  ON definitions.fk_entry_id=entries.entry_id "
                   "  WHERE definitions.fk_entry_id IS NULL LIMIT 1000)");
        emit deletionProgress(i, numberToDelete);
    }
    emit deletionProgress(numberToDelete, numberToDelete);
    query.exec("RELEASE row_deletion");

    emit rebuildingIndexes();

    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) "
               "SELECT rowid, pinyin, jyutping FROM entries");
    query.exec("INSERT INTO definitions_fts (rowid, definition) "
               "SELECT rowid, definition FROM definitions");
    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");

    emit cleaningUp();
    query.exec("VACUUM");

    return true;
}

bool SQLDatabaseUtils::removeSentencesFromDatabase(void)
{
    QSqlQuery query{_manager->getDatabase()};

    emit deletingSentences();

    // Remove Chinese sentences that are no longer linked to any definitions
    //
    // The result of the left join is:
    // chinese_sentences_id     fk_chinese_sentence_id
    // 12345                    12345                   // Linked sentence
    // 12346                    NULL                    // Unlinked sentence
    //
    // Then, we filter with WHERE to find all chinese_sentence_id with no
    // fk_chinese_sentence_id referencing it, and DELETE FROM chinese_sentences.

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    query.exec("(SELECT chinese_sentences.chinese_sentence_id "
               "    FROM chinese_sentences LEFT JOIN sentence_links "
               "    ON chinese_sentences.chinese_sentence_id = "
               "     sentence_links.fk_chinese_sentence_id "
               "    WHERE sentence_links.fk_chinese_sentence_id IS NULL)");

    query.exec("DELETE FROM chinese_sentences "
               "WHERE chinese_sentences.chinese_sentence_id IN "
               "(SELECT chinese_sentences.chinese_sentence_id "
               "    FROM chinese_sentences LEFT JOIN sentence_links "
               "    ON chinese_sentences.chinese_sentence_id = "
               "     sentence_links.fk_chinese_sentence_id "
               "    WHERE sentence_links.fk_chinese_sentence_id IS NULL)");

    // Remove non-Chinese sentences that are no longer linked with the same
    // principles as above
    query.exec("DELETE FROM nonchinese_sentences "
               "WHERE nonchinese_sentences.non_chinese_sentence_id IN "
               "(SELECT nonchinese_sentences.non_chinese_sentence_id "
               "    FROM nonchinese_sentences LEFT JOIN sentence_links "
               "    ON nonchinese_sentences.non_chinese_sentence_id = "
               "     sentence_links.fk_non_chinese_sentence_id "
               "    WHERE sentence_links.fk_non_chinese_sentence_id IS NULL)");

    return true;
}

bool SQLDatabaseUtils::removeSource(std::string source)
{
    QSqlQuery query{_manager->getDatabase()};

    // Determine what type of source it is based on the "other" field
    query.prepare("SELECT other FROM sources WHERE sourcename = ?");
    query.addBindValue(source.c_str());
    query.exec();

    int otherIndex = query.record().indexOf("other");
    std::string type = "";
    while (query.next()) {
        std::string other = query.value(otherIndex).toString().toStdString();
        std::vector<std::string> metadata;
        Utils::split(other, "●", metadata);

        if (metadata.size() >= 1) {
            type = metadata[0];
        }
    }

    deleteSourceFromDatabase(source);

    bool success = false;
    query.exec("BEGIN TRANSACTION");
    try {
        if (type.length() == 0) {
            if (!removeDefinitionsFromDatabase()) {
                throw std::runtime_error(
                    tr("Failed to remove definitions...").toStdString());
            }
        }
        if (type.find("sentences") != std::string::npos) {
            if (!removeSentencesFromDatabase()) {
                throw std::runtime_error(
                    tr("Failed to remove sentences").toStdString());
            }
        }
        query.exec("COMMIT");
        success = true;
        emit finishedDeletion(success);
    } catch (const std::exception &e) {
        emit finishedDeletion(success, e.what());
        query.exec("ROLLBACK");
    }

    return success;
}

bool SQLDatabaseUtils::addDefinitionSource(void)
{
    QSqlQuery query{_manager->getDatabase()};
    query.exec("DROP INDEX fk_entry_id_index");
    query.exec("DELETE FROM definitions_fts");
    query.exec("DELETE FROM entries_fts");

    emit insertingSource();

    query.exec("INSERT INTO sources(sourcename, sourceshortname, version, "
               "description, legal, link, update_url, other) "
               "SELECT sourcename, sourceshortname, version, description, "
               "legal, link, update_url, other "
               "FROM db.sources");

    if (query.lastError().isValid()) {
        QString error = query.lastError().text();

        emit finishedAddition(
            false,
            tr("Could not insert source. Could it be a duplicate of a "
               "dictionary you already installed?"),
            error);
        return false;
    }

    emit insertingEntries();

    query.exec("INSERT INTO entries(traditional, simplified, pinyin, "
               " jyutping, frequency)"
               "SELECT traditional, simplified, pinyin, jyutping, frequency "
               "FROM db.entries");

    emit insertingDefinitions();

    query.exec("DROP TABLE IF EXISTS definitions_tmp");
    query.exec("CREATE TEMPORARY TABLE definitions_tmp AS "
               " SELECT entries.traditional AS traditional, "
               "  entries.simplified AS simplified, "
               "  entries.pinyin AS pinyin, entries.jyutping AS jyutping, "
               "  sources.sourcename AS sourcename, "
               "  definitions.definition AS definition "
               "FROM db.entries, db.definitions, db.sources "
               "WHERE db.definitions.fk_entry_id = db.entries.entry_id "
               "AND db.definitions.fk_source_id = db.sources.source_id");
    query.exec("INSERT INTO definitions(definition, fk_entry_id, fk_source_id)"
               " SELECT d.definition, e.entry_id, s.source_id "
               " FROM definitions_tmp AS d, sources AS s, entries AS e "
               " WHERE d.sourcename = s.sourcename "
               "  AND d.traditional = e.traditional "
               "  AND d.simplified = e.simplified "
               "  AND d.pinyin = e.pinyin "
               "  AND d.jyutping = e.jyutping");

    emit rebuildingIndexes();

    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) "
               "SELECT rowid, pinyin, jyutping FROM entries");
    query.exec("INSERT INTO definitions_fts (rowid, definition) "
               "SELECT rowid, definition FROM definitions");
    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");

    return true;
}

bool SQLDatabaseUtils::addSentenceSource(void)
{
    QSqlQuery query{_manager->getDatabase()};

    // Copy the source from the file to database
    query.exec(
        "INSERT INTO sources "
        " (sourcename, sourceshortname, version, description, legal, link, "
        " update_url, other) "
        "SELECT sourcename, sourceshortname, version, description, legal, "
        " link, update_url, other "
        "FROM db.sources");

    // Copy the Chinese sentences
    query.exec(
        "INSERT INTO chinese_sentences "
        " (chinese_sentence_id, traditional, simplified, pinyin, jyutping, "
        "  language) "
        "SELECT chinese_sentence_id, traditional, simplified, pinyin, jyutping,"
        " language "
        "FROM db.chinese_sentences");

    // Copy the non-Chinese sentences
    query.exec("INSERT INTO nonchinese_sentences( "
               " non_chinese_sentence_id, sentence, language) "
               "SELECT non_chinese_sentence_id, sentence, language "
               "FROM db.nonchinese_sentences");

    // Because the source number is different between every database, we need to
    // change the source number for every sentence link.
    // To do this, create a temporary table that mirrors sentence_link table,
    // but replaces the fk_source_id with the name of the source
    query.exec("DROP TABLE IF EXISTS links_tmp");
    query.exec("CREATE TEMPORARY TABLE links_tmp AS "
               " SELECT sentence_links.fk_chinese_sentence_id as "
               "   fk_chinese_sentence_id,"
               "  sentence_links.fk_non_chinese_sentence_id as "
               "   fk_non_chinese_sentence_id,"
               "  sources.sourcename AS sourcename,"
               "  sentence_links.direct as direct "
               "FROM db.sentence_links, db.sources "
               "WHERE db.sentence_links.fk_source_id = db.sources.source_id");

    // Then, use that source name to find its source id in the main database
    // and use that source_id to insert into the main database.
    query.exec("INSERT INTO sentence_links( "
               " fk_chinese_sentence_id, fk_non_chinese_sentence_id, "
               " fk_source_id, direct) "
               "SELECT l.fk_chinese_sentence_id, l.fk_non_chinese_sentence_id, "
               " s.source_id, l.direct "
               "FROM links_tmp as l, sources as s "
               "WHERE l.sourcename = s.sourcename");

    return true;
}

bool SQLDatabaseUtils::addSource(std::string filepath)
{
    QSqlQuery query{_manager->getDatabase()};

    query.prepare("ATTACH DATABASE ? AS db");
    query.addBindValue(filepath.c_str());
    query.exec();

    // This usually happens so quickly that the slot(s) are not yet
    // connected to the finishedAddition() signal. Sleep for 1000ms
    // to make time for all connections to be made.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    query.exec("PRAGMA db.user_version");
    int version = -1;
    while (query.next()) {
        version = query.value(0).toInt();
    }

    if (version != CURRENT_DATABASE_VERSION) {
        query.exec("DETACH DATABASE db");
        emit finishedAddition(false,
                              tr("Database versions do not match. "
                                 "Only dictionaries with the same versions "
                                 "can be added."),
                              tr("Current version is %1, file version is %2.")
                                  .arg(CURRENT_DATABASE_VERSION)
                                  .arg(version));
        return false;
    }

    // Check for presence of "definitions" and "sentence_links" tables; if they
    // exist, then add their contents to the main database
    bool hasDefinitions = false;
    bool hasSentences = false;

    query.prepare("SELECT name FROM db.sqlite_master WHERE type=? AND name=?");
    query.addBindValue("table");
    query.addBindValue("definitions");
    query.exec();
    int nameIndex = query.record().indexOf("name");
    while (query.next()) {
        hasDefinitions = true;
    }

    query.prepare("SELECT name FROM db.sqlite_master WHERE type=? "
                  "AND name=?");
    query.addBindValue("table");
    query.addBindValue("sentence_links");
    query.exec();
    nameIndex = query.record().indexOf("name");
    while (query.next()) {
        hasSentences = true;
    }

    bool success = false;
    try {
        query.exec("BEGIN TRANSACTION");
        if (hasDefinitions) {
            if (!addDefinitionSource()) {
                throw std::runtime_error(
                    tr("Unable to add definitions...").toStdString());
            }
        }
        if (hasSentences) {
            if (!addSentenceSource()) {
                throw std::runtime_error(
                    tr("Unable to add sentences...").toStdString());
            }
        }

        query.exec("COMMIT");
        success = true;
        emit finishedAddition(success);
    } catch (std::exception &e) {
        query.exec("ROLLBACK");
        emit finishedAddition(success, e.what());
    }

    query.exec("DETACH DATABASE db");
    return success;
}
