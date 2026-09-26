#include "sqldatabaseutils.h"

#include "logic/database/sqldatabasemanager.h"

#include <QFile>
#include <QUuid>
#include <QtSql>

#include <chrono>
#include <thread>

// Database differences from version 1 to version 2:
// - Added chinese_sentences, nonchinese_sentences, and sentence_links tables
//   to properly support showing sentences in the GUI.
bool SQLDatabaseUtils::migrateDatabaseFromOneToTwo(QSqlDatabase &db)
{
    QSqlQuery query{db};

    query.exec(
        "CREATE TABLE IF NOT EXISTS chinese_sentences( "
        "  chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
        "  traditional TEXT, "
        "  simplified TEXT, "
        "  pinyin TEXT, "
        "  jyutping TEXT, "
        "  language TEXT, "
        "  UNIQUE(traditional, simplified, pinyin, jyutping) ON CONFLICT IGNORE"
        ")");

    query.exec(
        "CREATE TABLE IF NOT EXISTS nonchinese_sentences( "
        "  example_translation_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
        "  sentence TEXT, "
        "  language TEXT, "
        "  UNIQUE(example_translation_id, sentence) ON CONFLICT IGNORE"
        ")");

    query.exec("CREATE TABLE IF NOT EXISTS sentence_links( "
               "  fk_example_id INTEGER, "
               "  fk_example_translation_id INTEGER, "
               "  fk_source_id INTEGER, "
               "  direct BOOLEAN, "
               "  FOREIGN KEY(fk_example_id) REFERENCES "
               "    chinese_sentences(chinese_sentence_id), "
               "  FOREIGN KEY(fk_example_translation_id) REFERENCES "
               "    nonchinese_sentences(example_translation_id), "
               "  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON "
               "    DELETE CASCADE "
               ")");

    return true;
}

// Database differences from version 2 to version 3:
// - Added definitions_chinese_sentence_links to support linking between
//   definitions and example sentences
// - Added label to definitions table, to display parts of speech or other label
// - Added fk_entry_id to definitions_fts for faster lookup
// - Added unique constraint on sentence links
bool SQLDatabaseUtils::migrateDatabaseFromTwoToThree(QSqlDatabase &db)
{
    QSqlQuery query{db};

    // Add new definitions->chinese_sentence link table
    query.exec(
        "CREATE TABLE definitions_examples_links( "
        "  fk_definition_id INTEGER, "
        "  fk_example_id INTEGER, "
        "  FOREIGN KEY(fk_definition_id) REFERENCES definitions(definition_id) "
        "    ON DELETE CASCADE, "
        "  FOREIGN KEY(fk_example_id) REFERENCES "
        "    chinese_sentences(chinese_sentence_id) "
        "  UNIQUE(fk_definition_id, fk_example_id) ON CONFLICT IGNORE "
        ") ");
    if (query.lastError().isValid()) {
        return false;
    }

    // Drop the old definitions_fts table (to be re-created later)
    query.exec("DROP TABLE definitions_fts");
    if (query.lastError().isValid()) {
        return false;
    }

    // Delete and recreate the definitions table with new "label" column
    query.exec(
        "CREATE TABLE definitions_new( "
        "  definition_id INTEGER PRIMARY KEY, "
        "  definition TEXT, "
        "  label TEXT, "
        "  fk_entry_id INTEGER, "
        "  fk_source_id INTEGER, "
        "  FOREIGN KEY(fk_entry_id) REFERENCES entries(entry_id) ON UPDATE "
        "    CASCADE, "
        "  FOREIGN KEY(fk_source_id) REFERENCES sources(source_id) ON DELETE "
        "    CASCADE, "
        "  UNIQUE(definition, label, fk_entry_id, fk_source_id) ON CONFLICT IGNORE "
        ")");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("INSERT INTO definitions_new(definition_id, definition, "
               "  fk_entry_id, fk_source_id) "
               "SELECT definition_id, definition, fk_entry_id, fk_source_id "
               "FROM definitions ");
    if (query.lastError().isValid()) {
        return false;
    }
    // This statement also drops the fk_entry_id_index
    query.exec("DROP TABLE definitions");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE definitions_new RENAME TO definitions");
    if (query.lastError().isValid()) {
        return false;
    }

    // Re-populate definitions_fts table, adding new "fk_entry_id" column
    query.exec("CREATE VIRTUAL TABLE definitions_fts using fts5( "
               "  fk_entry_id UNINDEXED, definition "
               ")");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("INSERT INTO definitions_fts (rowid, fk_entry_id, definition) "
               "SELECT rowid, fk_entry_id, definition FROM definitions");
    if (query.lastError().isValid()) {
        return false;
    }
    // Re-add index
    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");
    if (query.lastError().isValid()) {
        return false;
    }

    // Delete and recreate the chinese_sentences table to change UNIQUE constraint
    query.exec("CREATE TABLE chinese_sentences_new( "
               "  chinese_sentence_id INTEGER PRIMARY KEY ON CONFLICT IGNORE, "
               "  traditional TEXT, "
               "  simplified TEXT, "
               "  pinyin TEXT, "
               "  jyutping TEXT, "
               "  language TEXT, "
               "  UNIQUE( "
               "    traditional, simplified, pinyin, jyutping, language "
               "  ) ON CONFLICT IGNORE "
               ")");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("INSERT INTO chinese_sentences_new(chinese_sentence_id, "
               "  traditional, simplified, pinyin, jyutping, language) "
               "SELECT chinese_sentence_id, traditional, simplified, pinyin, "
               "  jyutping, language "
               "FROM chinese_sentences");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("DROP TABLE chinese_sentences");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE chinese_sentences_new RENAME TO chinese_sentences");
    if (query.lastError().isValid()) {
        return false;
    }

    // Delete and recreate the sentence links table to add new UNIQUE constraint
    query.exec("CREATE TABLE sentence_links_new( "
               "  fk_example_id INTEGER, "
               "  fk_example_translation_id INTEGER, "
               "  fk_source_id INTEGER, "
               "  direct BOOLEAN, "
               "  FOREIGN KEY(fk_example_id) "
               "    REFERENCES chinese_sentences(chinese_sentence_id), "
               "  FOREIGN KEY(fk_example_translation_id) "
               "    REFERENCES nonchinese_sentences(example_translation_id), "
               "  FOREIGN KEY(fk_source_id) "
               "    REFERENCES sources(source_id) ON DELETE CASCADE "
               "  UNIQUE( "
               "    fk_example_id, fk_example_translation_id "
               "  ) ON CONFLICT IGNORE "
               ")");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("INSERT INTO sentence_links_new(fk_example_id, "
               "  fk_example_translation_id, fk_source_id, direct) "
               "SELECT fk_example_id, fk_example_translation_id, "
               "  fk_source_id, direct "
               "FROM sentence_links ");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("DROP TABLE sentence_links");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE sentence_links_new RENAME TO sentence_links");
    if (query.lastError().isValid()) {
        return false;
    }

    return true;
}

// Database differences from version 3 to version 4:
// - Added indexes on simplified, jyutping, and pinyin
// - Added indexes on sentence links
// - Renamed sentence -> example
bool SQLDatabaseUtils::migrateDatabaseFromThreeToFour(QSqlDatabase &db)
{
    QSqlQuery query{db};

    query.exec("CREATE INDEX IF NOT EXISTS entries_simplified_idx ON "
               "entries(simplified);");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("CREATE INDEX IF NOT EXISTS entries_jyutping_idx ON "
               "entries(jyutping);");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec(
        "CREATE INDEX IF NOT EXISTS entries_pinyin_idx ON entries(pinyin);");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("ALTER TABLE 'chinese_sentences' RENAME TO 'examples'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE 'examples' RENAME COLUMN 'chinese_sentence_id' TO "
               "'example_id'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec(
        "ALTER TABLE 'nonchinese_sentences' RENAME TO 'example_translations'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE 'example_translations' RENAME COLUMN "
               "'non_chinese_sentence_id' TO 'example_translation_id'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE 'example_translations' RENAME COLUMN 'sentence' TO "
               "'translation'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE 'sentence_links' RENAME TO 'example_links'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE 'example_links' RENAME COLUMN "
               "'fk_chinese_sentence_id' TO 'fk_example_id'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE 'example_links' RENAME COLUMN "
               "'fk_non_chinese_sentence_id' TO 'fk_example_translation_id'");
    if (query.lastError().isValid()) {
        qDebug() << query.lastError();
        return false;
    }
    query.exec("ALTER TABLE 'definitions_chinese_sentences_links' RENAME TO "
               "'definitions_examples_links'");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("ALTER TABLE 'definitions_examples_links' RENAME COLUMN "
               "'fk_chinese_sentence_id' TO 'fk_example_id'");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS del_fk_example_idx ON "
               "definitions_examples_links(fk_example_id);");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("CREATE INDEX IF NOT EXISTS example_links_fk_example_translation_idx "
               "ON example_links(fk_example_translation_id);");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec(
        "UPDATE sources SET other = REPLACE(other, 'sentences', 'examples') "
        "WHERE other LIKE '%sentences%';");
    if (query.lastError().isValid()) {
        return false;
    }

    return true;
}

// Update the database to whatever the current version is.
bool SQLDatabaseUtils::updateDatabase(QSqlDatabase &db)
{
    QSqlQuery query{db};

    query.exec("PRAGMA user_version");
    int version = -1;
    while (query.next()) {
        version = query.value(0).toInt();
    }

    if (version != CURRENT_DATABASE_VERSION) {
        query.exec("SAVEPOINT database_update");
        emit migratingDatabase();
        bool success = true;
        switch (version) {
        case -1:
            [[fallthrough]];
        case 1:
            if (!migrateDatabaseFromOneToTwo(db)) {
                success = false;
                break;
            }
            [[fallthrough]];
        case 2:
            if (!migrateDatabaseFromTwoToThree(db)) {
                success = false;
                break;
            }
            [[fallthrough]];
        case 3:
            if (!migrateDatabaseFromThreeToFour(db)) {
                success = false;
                break;
            }
            [[fallthrough]];
        default:
            break;
        }

        if (success) {
            query.exec("RELEASE database_update");
            // For some reason, PRAGMA user_version=? doesn't work; so just use a QString
            QString queryString = "PRAGMA user_version=%1";
            query.prepare(queryString.arg(CURRENT_DATABASE_VERSION));
            query.exec();
        } else {
            query.exec("ROLLBACK");
        }

        emit finishedMigratingDatabase(success);
    }

    return true;
}

// Reads a mapping of sourcename / sourceshortname from the database
// so they can later be converted between the two.
bool SQLDatabaseUtils::readSources(
    QSqlDatabase &db, std::vector<std::pair<std::string, std::string>> &sources)
{
    QSqlQuery query{db};
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

// Reads all the metadata about the sources.
bool SQLDatabaseUtils::readSources(QSqlDatabase &db,
                                   std::vector<SourceMetadata> &sources)
{
    QSqlQuery query{db};
    query.setForwardOnly(true);
    query.exec("SELECT sourcename, version, description, legal, link, "
               "  update_url, other "
               "FROM sources");

    if (query.lastError().isValid()) {
        return false;
    }

    int sourcenameIndex = query.record().indexOf("sourcename");
    int versionIndex = query.record().indexOf("version");
    int descriptionIndex = query.record().indexOf("description");
    int legalIndex = query.record().indexOf("legal");
    int linkIndex = query.record().indexOf("link");
    int updateURLIndex = query.record().indexOf("update_url");
    int otherIndex = query.record().indexOf("other");

    while (query.next()) {
        std::string source
            = query.value(sourcenameIndex).toString().toStdString();
        std::string version = query.value(versionIndex).toString().toStdString();
        std::string description
            = query.value(descriptionIndex).toString().toStdString();
        std::string legal = query.value(legalIndex).toString().toStdString();
        std::string link = query.value(linkIndex).toString().toStdString();
        std::string updateURL
            = query.value(updateURLIndex).toString().toStdString();
        std::string other = query.value(otherIndex).toString().toStdString();

        SourceMetadata dictionary{source,
                                  version,
                                  description,
                                  legal,
                                  link,
                                  updateURL,
                                  other};

        sources.push_back(dictionary);
    }

    return true;
}

bool SQLDatabaseUtils::dropIndices(QSqlDatabase &db)
{
    QSqlQuery query{db};
    query.exec("DROP INDEX fk_entry_id_index");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("DROP INDEX entries_simplified_idx");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("DROP INDEX entries_jyutping_idx");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("DROP INDEX entries_pinyin_idx");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("DROP INDEX del_fk_example_idx");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("DROP INDEX example_links_fk_example_translation_idx");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("DELETE FROM definitions_fts");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("DELETE FROM entries_fts");
    return !query.lastError().isValid();
}

bool SQLDatabaseUtils::rebuildIndices(QSqlDatabase &db)
{
    QSqlQuery query{db};
    emit rebuildingIndexes();

    query.exec("INSERT INTO entries_fts (rowid, pinyin, jyutping) "
               "SELECT rowid, pinyin, jyutping FROM entries");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("INSERT INTO definitions_fts (rowid, fk_entry_id, definition) "
               "SELECT rowid, fk_entry_id, definition FROM definitions");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("CREATE INDEX fk_entry_id_index ON definitions(fk_entry_id)");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("CREATE INDEX IF NOT EXISTS entries_simplified_idx ON "
               "entries(simplified);");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("CREATE INDEX IF NOT EXISTS entries_jyutping_idx ON "
               "entries(jyutping);");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec(
        "CREATE INDEX IF NOT EXISTS entries_pinyin_idx ON entries(pinyin);");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec("CREATE INDEX IF NOT EXISTS del_fk_example_idx ON "
               "definitions_examples_links(fk_example_id);");
    if (query.lastError().isValid()) {
        return false;
    }
    query.exec(
        "CREATE INDEX IF NOT EXISTS example_links_fk_example_translation_idx "
        "ON example_links(fk_example_translation_id);");
    if (query.lastError().isValid()) {
        return false;
    }

    return true;
}

bool SQLDatabaseUtils::deleteSourceFromDatabase(QSqlDatabase &db,
                                                const std::string &source)
{
    QSqlQuery query{db};

    query.prepare("DELETE FROM sources WHERE sourcename = ?");
    query.addBindValue(source.c_str());
    query.exec();

    return !query.lastError().isValid();
}

// Removing definitions from the database involves the following steps:
// - Deleting all the FTS5 tables (they will no longer be valid)
// - Finding how many entries will be deleted
// - Deleting those entries
// - Re-creating the FTS5 tables
// - Re-creating indices that were previously invalidated.
bool SQLDatabaseUtils::removeDefinitionsFromDatabase(QSqlDatabase &db)
{
    QSqlQuery query{db};

    emit deletingDefinitions();

    // The table that is built up by this query looks like this:
    //
    // entry_id         definitions.fk_entry_id
    // 1                1                       // Definition references entry
    // 2                NULL                    // Entry is not referenced
    //
    // Then, we can count all the number of rows where definitions.fk_entry_id
    // is NULL to find the number of entries that must be deleted.
    query.exec("SELECT COUNT(entries.entry_id) AS count "
               "FROM entries "
               "WHERE entries.entry_id IN "
               "  (SELECT entries.entry_id FROM entries "
               "    LEFT JOIN definitions "
               "    ON definitions.fk_entry_id=entries.entry_id "
               "    WHERE definitions.fk_entry_id IS NULL)");
    int numberToDelete = 0;
    while (query.next()) {
        numberToDelete = query.value(0).toInt();
        emit totalToDelete(numberToDelete);
    }

    for (int i = 0; i < numberToDelete; i += 1000) {
        query.exec("DELETE FROM entries WHERE entry_id IN "
                   "  (SELECT entries.entry_id FROM entries "
                   "    LEFT JOIN definitions "
                   "    ON definitions.fk_entry_id=entries.entry_id "
                   "    WHERE definitions.fk_entry_id IS NULL LIMIT 1000)");
        emit deletionProgress(i, numberToDelete);
    }
    emit deletionProgress(numberToDelete, numberToDelete);

    return true;
}

// Removing examples from the database involves the following steps:
// - Assuming the source has been deleted, because of foreign key constraints,
//   the example_links from that source should have also been deleted.
// - Use the LEFT JOIN table to find examples that are no longer
//   linked to any other examples, and delete them.
// - Use the same LEFT JOIN (but on example_translations) to delete
//   translations that are also no longer linked to any examples.
bool SQLDatabaseUtils::removeExamplesFromDatabase(QSqlDatabase &db)
{
    QSqlQuery query{db};

    emit deletingExamples();

    // Remove examples that are no longer linked to any definitions
    // or translations
    //
    // The result of the LEFT JOIN is:
    // example_id               fk_example_id
    // 12345                    12345                   // Linked sentence
    // 12346                    NULL                    // Unlinked sentence
    //
    // Then, we filter with WHERE to find all example_id with no
    // fk_example_id referencing it, and DELETE FROM examples.

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    query.exec("DELETE FROM examples "
               "WHERE examples.example_id IN "
               "(SELECT examples.example_id "
               "   FROM examples "
               "   LEFT JOIN example_links "
               "     ON examples.example_id = "
               "       example_links.fk_example_id "
               "   LEFT JOIN definitions_examples_links "
               "     ON examples.example_id = "
               "       definitions_examples_links.fk_example_id "
               "   WHERE example_links.fk_example_id IS NULL "
               "     AND "
               "definitions_examples_links.fk_example_id IS "
               "NULL "
               ")");

    // Remove example translations that are no longer linked with the same
    // principles as above
    query.exec("DELETE FROM example_translations "
               "WHERE example_translations.example_translation_id IN "
               "(SELECT example_translations.example_translation_id "
               "   FROM example_translations LEFT JOIN example_links "
               "   ON example_translations.example_translation_id = "
               "     example_links.fk_example_translation_id "
               "   WHERE example_links.fk_example_translation_id IS NULL)");

    return true;
}

// Method to remove a source from the database, based on the name of the source.
bool SQLDatabaseUtils::removeSource(
    QSqlDatabase &db,
    const std::string &source,
    const std::shared_ptr<SQLDatabaseManager> &manager,
    bool skipCleanup)
{
    if (manager) {
        manager->backupDictionaryDatabase();
    }
    std::vector<std::string> sources{source};
    return removeSources(db, sources, skipCleanup);
}

// Method to remove multiple sources from the database, based on the name of the sources.
bool SQLDatabaseUtils::removeSources(QSqlDatabase &db,
                                     std::span<const std::string> sources,
                                     bool skipCleanup)
{
    QSqlQuery query{db};

    // Foreign keys cannnot be turned on inside a SAVEPOINT!
    // Turn on before this savepoint.
    query.exec("PRAGMA foreign_keys = ON");

    // Design note: I wish I could make this entire function inside a single
    // transaction (instead of splitting it into source_deletion and
    // source_removal). Unfortunately, removeDefinitionsFromDatabase()
    // is slow as molasses if foreign keys are turned on. I sacrifice a bit
    // of correctness in exchange for much more speed.
    query.exec("SAVEPOINT source_deletion");
    std::string type;
    for (const auto &source : sources) {
        // Determine what type of source it is based on the "other" field
        query.prepare("SELECT other FROM sources WHERE sourcename = ?");
        query.addBindValue(source.c_str());
        query.exec();

        // Assume that the "other" field in the database contains metadata about
        // what type of data this source contains, in the format
        // type,type,type (types of data separated by the character ',')
        int otherIndex = query.record().indexOf("other");
        while (query.next()) {
            type += query.value(otherIndex).toString().toStdString() + ",";
        }

        if (!deleteSourceFromDatabase(db, source)) {
            emit finishedDeletion(
                false, tr("Failed to delete source from database..."));
            query.exec("ROLLBACK");
            query.exec("PRAGMA foreign_keys = OFF");
            return false;
        }
    }
    query.exec("RELEASE source_deletion");
    query.exec("PRAGMA foreign_keys = OFF");

    query.exec("SAVEPOINT source_removal");
    // In the metadata field, no type is a definition source.
    // examples is an examples source.
    bool success = false;
    try {
        dropIndices(db);

        if (type.length() == 0 || type.find("words") != std::string::npos) {
            if (!removeDefinitionsFromDatabase(db)) {
                throw std::runtime_error(
                    tr("Failed to remove definitions...").toStdString());
            }
        }
        if (type.find("examples") != std::string::npos) {
            if (!removeExamplesFromDatabase(db)) {
                throw std::runtime_error(
                    tr("Failed to remove examples...").toStdString());
            }
        }

        if (!skipCleanup) {
            rebuildIndices(db);
        }

        query.exec("RELEASE source_removal");

        if (!skipCleanup) {
            emit cleaningUp();
            query.exec("VACUUM");
        }

        success = true;
        emit finishedDeletion(success);
    } catch (const std::exception &e) {
        emit finishedDeletion(success, e.what());
        query.exec("ROLLBACK");
    }

    return success;
}

// Inserting into the database loops through all the sources in the attached
// database and attempts to insert each one into the database.
std::pair<bool, std::string> SQLDatabaseUtils::insertSourcesIntoDatabase(
    QSqlDatabase &db,
    std::unordered_map<std::string, std::string> old_source_ids)
{
    QSqlQuery query{db};

    query.exec(
        "SELECT sourcename, sourceshortname, version, description, legal, "
        "  link, update_url, other "
        "FROM db.sources");
    int sourcenameIndex = query.record().indexOf("sourcename");
    int sourceshortnameIndex = query.record().indexOf("sourceshortname");
    int versionIndex = query.record().indexOf("version");
    int descriptionIndex = query.record().indexOf("description");
    int legalIndex = query.record().indexOf("legal");
    int linkIndex = query.record().indexOf("link");
    int updateURLIndex = query.record().indexOf("update_url");
    int otherIndex = query.record().indexOf("other");

    while (query.next()) {
        QSqlQuery insertQuery{db};

        QString sourcename{query.value(sourcenameIndex).toString()};
        QString sourceshortname{query.value(sourceshortnameIndex).toString()};
        QString version{query.value(versionIndex).toString()};
        QString description{query.value(descriptionIndex).toString()};
        QString legal{query.value(legalIndex).toString()};
        QString link{query.value(linkIndex).toString()};
        QString updateURL{query.value(updateURLIndex).toString()};
        QString other{query.value(otherIndex).toString()};

        if (old_source_ids.find(sourcename.toStdString())
            != old_source_ids.end()) {
            // If the sourcename is in the old_source_ids map, it means that it
            // already existed in the database. Since we want to preserve
            // dictionary order, we will need to insert at the old dictionary ID
            // which is given by the map.
            QString sourceid = QString::fromStdString(
                old_source_ids[sourcename.toStdString()]);

            insertQuery.prepare("INSERT INTO sources "
                                "  (source_id, sourcename, sourceshortname, "
                                "   version, description, legal, link, "
                                "   update_url, other) "
                                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
            insertQuery.addBindValue(sourceid);
            insertQuery.addBindValue(sourcename);
            insertQuery.addBindValue(sourceshortname);
            insertQuery.addBindValue(version);
            insertQuery.addBindValue(description);
            insertQuery.addBindValue(legal);
            insertQuery.addBindValue(link);
            insertQuery.addBindValue(updateURL);
            insertQuery.addBindValue(other);
        } else {
            insertQuery.prepare("INSERT INTO sources "
                                "  (sourcename, sourceshortname, version, "
                                "   description, legal, link, update_url, "
                                "   other) "
                                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
            insertQuery.addBindValue(sourcename);
            insertQuery.addBindValue(sourceshortname);
            insertQuery.addBindValue(version);
            insertQuery.addBindValue(description);
            insertQuery.addBindValue(legal);
            insertQuery.addBindValue(link);
            insertQuery.addBindValue(updateURL);
            insertQuery.addBindValue(other);
        }

        insertQuery.exec();
        if (insertQuery.lastError().isValid()) {
            QString error = insertQuery.lastError().text();
            return {false, error.toStdString()};
        }
    }

    return {true, ""};
}

// To add a definition source, the steps are the following:
// - Insert into the entries table all the entries from the attached db file.
// - Use a CTE that:
//     - Matches the fk_source_id (from the attached db) to the new source_id
//       in the main database.
//     - Matches the fk_entry_id (from the attached db) to the new entry_id
//       in the main database.
// - Insert all the entries from the CTE into the main table.
bool SQLDatabaseUtils::addDefinitionSource(QSqlDatabase &db)
{
    QSqlQuery query{db};

    emit insertingEntries();

    query.exec("INSERT INTO entries(traditional, simplified, pinyin, "
               "  jyutping, frequency)"
               "SELECT traditional, simplified, pinyin, jyutping, frequency "
               "FROM db.entries");
    if (query.lastError().isValid()) {
        return false;
    }

    emit insertingDefinitions();

    // A diagram of what the various databases look like:
    //
    // ATTACHED DB
    // entries                      | definitions                         | sources
    // trad simp jp   py entry_id<----fk_entry_id definition fk_source_id-->source_id sourcename
    // 麼   么    mo1  me 0129<--------0129        what       1------------->1         CC-CEDICT
    //
    // MAIN DB
    // (entry_id and source_id are auto-incremented in the insertion done
    //  a few lines higher; the fk_entry_id and fk_source_id from the attached
    //  are database no longer valid)
    // entries                      | definitions               | sources
    // trad simp jp   py entry_id<----fk_entry_id  fk_source_id-->source_id sourcename
    // 麼   么    mo1  me 6130<--------?            ?------------->6         CC-CEDICT
    //
    // definitions_tmp
    // trad  simp jp   py   sourcename  definition
    // 麼    么    mo1  me   CC-CEDICT   what
    //
    // Based on that CTE, we try to match the new entry against
    // what exists in the database.
    // entries                      | definitions               | sources
    // trad simp jp   py entry_id<----fk_entry_id  fk_source_id-->source_id sourcename
    // 麼   么    mo1  me 6130<--------?            ?------------->6         CC-CEDICT
    // ^    ^     ^    ^                                                    ^
    // |    |     |    |    ┍----------------------------------------------┛
    // |    |     |    |    ┃
    // trad  simp jp   py   sourcename  definition
    // 麼    么    mo1  me   CC-CEDICT   what
    //
    // Because we can match trad/simp/jp/py, fk_entry_id is assigned 6130
    // and because we can match CC-CEDICT, fk_source_id is assigned 6.

    query.exec("WITH definitions_tmp AS ( "
               "  SELECT entries.traditional AS traditional, "
               "    entries.simplified AS simplified, "
               "    entries.pinyin AS pinyin, entries.jyutping AS jyutping, "
               "    sources.sourcename AS sourcename, "
               "    definitions.definition AS definition, "
               "    definitions.label AS label "
               "  FROM db.entries, db.definitions, db.sources "
               "  WHERE db.definitions.fk_entry_id = db.entries.entry_id "
               "  AND db.definitions.fk_source_id = db.sources.source_id "
               ") "
               " "
               "INSERT INTO definitions(definition, label, fk_entry_id, "
               "fk_source_id) "
               "  SELECT d.definition, d.label, e.entry_id, s.source_id "
               "  FROM definitions_tmp AS d, sources AS s, entries AS e "
               "  WHERE d.sourcename = s.sourcename "
               "    AND d.traditional = e.traditional "
               "    AND d.simplified = e.simplified "
               "    AND d.pinyin = e.pinyin "
               "    AND d.jyutping = e.jyutping");

    return !query.lastError().isValid();
}

// To add an example source, the steps are the following:
// - Insert into the examples table all examples from the
//   examples table of the attached database
// - Insert into the example_translations table all the translations from the
//   example_translations table of the attached database
// - Use a CTE that:
//     - Contains the example_links from the attached database
//     - Matches example IDs from the attached database to the
//       example IDs that are in the main database
//     - Matches the fk_source_id (from the attached db) to the new source_id
//       in the main database.
// - Insert all the links from the CTE into the main table
//
// Then, insert the definitions/examples links:
// - entry_and_definitions: Uniquely identify each definition from the attached
//   database by joining the definition with the entry it belongs to, and the
//   source it comes from
// - defs_s_links_tmp: Uniquely identify each definition -> Chinese sentence
//   link in the attached database by selecting from the
//   definitions_examples_links table of the attached database
// - new_entry_and_examples: Uniquely identify each definition from the
//   current database, as in entry_and_definitions
// - Then, since new_entry_and_definitions contains all the data that uniquely
//   identifies corresponding defnitions (aka entry / definition / label / source), join
//   the definition -> example links from defs_s_links_tmp using that
//   information
bool SQLDatabaseUtils::addExampleSource(QSqlDatabase &db)
{
    QSqlQuery query{db};

    query.exec("INSERT INTO examples "
               "  (example_id, traditional, simplified, pinyin, "
               "jyutping, "
               "    language) "
               "SELECT example_id, traditional, simplified, "
               "pinyin, jyutping,"
               "   language "
               "FROM db.examples");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("INSERT INTO examples_translations( "
               "  example_translation_id, translation, language) "
               "SELECT example_translation_id, translation, language "
               "FROM db.example_translations");
    if (query.lastError().isValid()) {
        return false;
    }

    // A diagram of what the various databases look like:
    //
    // ATTACHED DB
    // sentence_link                                             | sources
    // example_id          example_translation_id fk_source_id     source_id    sourcename
    // 55                  104725                 1--------------->1            Tatoeba—Cantonese-English
    //
    // CTE: links_tmp
    // example_id          example_translation_id sourcename
    // 55                  104725                 Tatoeba—Cantonese-English
    //
    // Based on that CTE, we try to match the new entry against
    // what exists in the database.
    // sentence_link                                             | sources
    // example_id          example_translation_id fk_source_id     source_id    sourcename
    // 55                  104725                 ?                6            Tatoeba—Cantonese-English
    //                                                                          ^
    //                                                                          │
    //                                            ┍━━━━━━━━━━━━━━━━━┛
    //                                            ┃
    // example_id          example_translation_id sourcename
    // 55                  104725                 Tatoeba—Cantonese-English
    //
    // Because we can match Tatoeba—Cantonese-English, fk_source_id is assigned 6.

    query.exec("WITH example_links_with_source AS ( "
               "  SELECT example_links.fk_example_id as fk_csi, "
               "    example_links.fk_example_translation_id as fk_eti, "
               "    sources.sourcename AS sourcename, "
               "    example_links.direct as direct "
               "  FROM db.example_links, db.sources "
               "  WHERE db.example_links.fk_source_id = db.sources.source_id "
               "), "
               " "
               "example_links_with_foreign_key AS ( "
               "  SELECT traditional, simplified, pinyin, jyutping, language, "
               "    fk_eti, direct, sourcename "
               "  FROM example_links_with_source as elws, "
               "    db.examples AS e "
               "  WHERE elws.fk_csi = e.example_id "
               ") "
               " "
               "INSERT INTO example_links( "
               "  fk_example_id, fk_example_translation_id, "
               "  fk_source_id, direct) "
               "SELECT e.example_id, elwfk.fk_eti, "
               "  s.source_id, elwfk.direct "
               "FROM example_links_with_foreign_key AS elwfk, sources as s, "
               "  examples AS e "
               "WHERE s.sourcename = elwfk.sourcename "
               "  AND e.traditional = elwfk.traditional "
               "  AND e.simplified = elwfk.simplified "
               "  AND e.pinyin = elwfk.pinyin "
               "  AND e.jyutping = elwfk.jyutping "
               "  AND e.language = elwfk.language ");
    if (query.lastError().isValid()) {
        return false;
    }

    query.exec("WITH entry_and_definitions AS ( "
               "  SELECT entries.traditional AS traditional, "
               "    entries.simplified AS simplified, "
               "    entries.pinyin AS pinyin, "
               "    entries.jyutping AS jyutping, "
               "    definitions.definition AS definition, "
               "    definitions.label AS label, "
               "    definitions.definition_id AS definition_id, "
               "    sources.sourcename AS source "
               "  FROM db.entries, db.definitions, db.sources "
               "  WHERE db.definitions.fk_entry_id = db.entries.entry_id "
               "    AND db.definitions.fk_source_id = db.sources.source_id "
               "), "
               " "
               "defs_e_links_tmp AS ( "
               "  SELECT "
               "    e.traditional AS sentence_traditional, "
               "    e.simplified AS sentence_simplified, "
               "    e.pinyin AS sentence_pinyin, "
               "    e.jyutping AS sentence_jyutping, "
               "    e.language AS sentence_language, "
               "    ed.definition AS definition, "
               "    ed.label AS label, "
               "    ed.traditional AS traditional, "
               "    ed.simplified AS simplified, "
               "    ed.pinyin AS pinyin, "
               "    ed.jyutping AS jyutping, "
               "    ed.source AS source "
               "  FROM db.definitions_examples_links AS dsl, "
               "    db.examples AS e, "
               "    entry_and_definitions AS ed "
               "  WHERE dsl.fk_definition_id = ed.definition_id "
               "    AND dsl.fk_example_id = e.example_id "
               "), "
               " "
               "new_entry_and_definitions AS ( "
               "  SELECT entries.traditional AS traditional, "
               "    entries.simplified AS simplified, "
               "    entries.pinyin AS pinyin, "
               "    entries.jyutping AS jyutping, "
               "    definitions.definition AS definition, "
               "    definitions.label AS label, "
               "    definitions.definition_id AS definition_id, "
               "    sources.sourcename AS source "
               "  FROM entries, definitions, sources "
               "  WHERE definitions.fk_entry_id = entries.entry_id "
               "    AND definitions.fk_source_id = sources.source_id "
               ") "
               " "
               "INSERT INTO definitions_examples_links( "
               "  fk_definition_id, fk_example_id) "
               "SELECT ned.definition_id, e.example_id "
               "FROM defs_e_links_tmp AS dsl, "
               "  new_entry_and_definitions AS ned, "
               "  examples AS e "
               "WHERE dsl.sentence_traditional = e.traditional "
               "  AND dsl.sentence_simplified = e.simplified "
               "  AND dsl.sentence_pinyin = e.pinyin "
               "  AND dsl.sentence_jyutping = e.jyutping "
               "  AND dsl.sentence_language = e.language "
               "  AND dsl.definition = ned.definition "
               "  AND dsl.label = ned.label "
               "  AND dsl.traditional = ned.traditional "
               "  AND dsl.simplified = ned.simplified "
               "  AND dsl.pinyin = ned.pinyin "
               "  AND dsl.jyutping = ned.jyutping "
               "  AND dsl.source = ned.source");

    return !query.lastError().isValid();
}

bool SQLDatabaseUtils::addSource(
    QSqlDatabase &db,
    const std::string &filepath,
    const std::shared_ptr<SQLDatabaseManager> &manager,
    bool overwriteConflictingSource)
{
    if (manager) {
        manager->backupDictionaryDatabase();
    }

    QSqlQuery query{db};

    query.prepare("ATTACH DATABASE ? AS db");
    query.addBindValue(filepath.c_str());
    query.exec();

    // Only databases that match the current version can be added
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

    // Only sources that don't already exist in the database can be added
    std::unordered_map<std::string, std::string> old_source_ids;
    if (overwriteConflictingSource) {
        // If overwrite is set to true, then we remove the conflicting
        // sources before adding the new ones from the filepath.
        // However, we keep around the source_ids belonging to those
        // sources, so that when we insert the ones from the filepath, we
        // can insert them at the same source_id.
        query.exec("SELECT source_id, sourcename "
                   "FROM sources WHERE sourcename IN "
                   "  (SELECT sourcename FROM db.sources)");
        int sourceIdIndex = query.record().indexOf("source_id");
        int sourcenameIndex = query.record().indexOf("sourcename");
        std::vector<std::string> sourcesToRemove;
        while (query.next()) {
            sourcesToRemove.emplace_back(
                query.value(sourcenameIndex).toString().toStdString());
            old_source_ids[query.value(sourcenameIndex).toString().toStdString()]
                = query.value(sourceIdIndex).toString().toStdString();
        }
        if (!removeSources(db,
                           sourcesToRemove,
                           /* skipCleanup */ true)) {
            query.exec("DETACH DATABASE db");
            rebuildIndices(db);
            emit finishedAddition(
                false,
                tr("Could not add new dictionaries. We couldn't remove a "
                   "dictionary that you already had installed with the "
                   "same "
                   "name."),
                tr("Try manually deleting the dictionaries yourself before "
                   "adding the new dictionary."));
            return false;
        }
    } else {
        // If overwrite is set to false, then we return the list of conflicting
        // databases, along with their versions
        query.exec("SELECT s.sourcename, s.version AS in_database_version, "
                   "  dbs.version AS new_file_version "
                   "FROM sources AS s, db.sources AS dbs "
                   "WHERE s.sourcename IN "
                   "  (SELECT sourcename FROM db.sources) "
                   "AND s.sourcename = dbs.sourcename");
        int sourcenameIndex = query.record().indexOf("sourcename");
        int inDatabaseVersionIndex = query.record().indexOf(
            "in_database_version");
        int newFileVersionIndex = query.record().indexOf("new_file_version");
        conflictingSourceMetadata matchingDictionaryNames;
        while (query.next()) {
            matchingDictionaryNames.emplace_back(
                query.value(sourcenameIndex).toString().toStdString(),
                query.value(inDatabaseVersionIndex).toString().toStdString(),
                query.value(newFileVersionIndex).toString().toStdString());
        }
        if (!matchingDictionaryNames.empty()) {
            query.exec("DETACH DATABASE db");
            emit conflictingDictionaryNamesExist(matchingDictionaryNames);
            return false;
        }
    }

    // Since removeSources() causes indices to be dropped, but removeSources()
    // is not always called (it will only be called if there is a conflicting
    // source), indices must always be dropped and rebuilt outside of the
    // source_addition savepoint.
    // If indices were dropped inside the savepoint, then a rollback would cause
    // the index drop to never occur. We would have an inconsistent state -
    // if overwrite is true, indices would still be dropped after the rollback,
    // otherwise, indices would not be dropped. To avoid this state, we always
    // drop indices before the savepoint, so indices are always dropped after
    // a rollback.
    dropIndices(db);

    query.exec("SAVEPOINT source_addition");
    // Insert the sources from the new database file into the database
    emit insertingSource();
    auto [success, errorMessage] = insertSourcesIntoDatabase(db, old_source_ids);
    if (!success) {
        query.exec("DETACH DATABASE db");
        query.exec("ROLLBACK");
        rebuildIndices(db);
        emit finishedAddition(
            false,
            tr("Could not insert source. Could it be a duplicate of a "
               "dictionary you already installed?"),
            errorMessage.c_str());

        return false;
    }

    // Then, insert the definitions and sentences
    success = false;
    try {
        if (!addDefinitionSource(db)) {
            throw std::runtime_error(
                tr("Unable to add definitions...").toStdString());
        }
        if (!addExampleSource(db)) {
            throw std::runtime_error(
                tr("Unable to add examples...").toStdString());
        }

        query.exec("RELEASE source_addition");
        rebuildIndices(db);
        success = true;
        emit finishedAddition(success);
    } catch (std::exception &e) {
        query.exec("ROLLBACK");
        rebuildIndices(db);
        emit finishedAddition(success, e.what());
    }

    query.exec("DETACH DATABASE db");
    return success;
}

bool SQLDatabaseUtils::mergeDatabases(const std::vector<std::string> &paths)
{
    if (paths.empty()) {
        return true;
    }

    const auto &outputDatabasePath = paths[0];
    QFile databaseFile{QString::fromStdString(outputDatabasePath)};
    if (!databaseFile.open(QIODevice::ReadWrite)) {
        return false;
    }

    QString outputConnectionName{
        QUuid::createUuid().toString(QUuid::WithoutBraces)};
    QSqlDatabase::addDatabase("QSQLITE", outputConnectionName);
    QSqlDatabase::database(outputConnectionName)
        .setDatabaseName(QString::fromStdString(outputDatabasePath));
    QSqlDatabase::database(outputConnectionName).open();
    QSqlDatabase outputDatabase = QSqlDatabase::database(outputConnectionName);

    updateDatabase(outputDatabase);
    dropIndices(outputDatabase);

    for (const auto &p : paths) {
        if (p == outputDatabasePath) {
            continue;
        }

        QString tmpConnectionName{
            QUuid::createUuid().toString(QUuid::WithoutBraces)};
        QSqlDatabase::addDatabase("QSQLITE", tmpConnectionName);
        QSqlDatabase::database(tmpConnectionName)
            .setDatabaseName(QString::fromStdString(p));
        QSqlDatabase::database(tmpConnectionName).open();
        QSqlDatabase tmpDatabase = QSqlDatabase::database(tmpConnectionName);
        updateDatabase(tmpDatabase);
        tmpDatabase.close();

        addSource(outputDatabase, p);
    }

    outputDatabase.close();
    return true;
}
