#include "sqldatabaseutils.h"

#include <QtSql>

SQLDatabaseUtils::SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager)
    : QObject()
{
    _manager = manager;
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

bool SQLDatabaseUtils::removeSource(std::string source)
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

    emit deletingDefinitions();

    query.exec("BEGIN TRANSACTION");
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
    query.exec("COMMIT");

    emit cleaningUp();
    query.exec("VACUUM");

    emit finishedDeletion(true);
    return true;
}

bool SQLDatabaseUtils::addSource(std::string filepath)
{
    QSqlQuery query{_manager->getDatabase()};

    query.prepare("ATTACH DATABASE ? AS db");
    query.addBindValue(filepath.c_str());
    query.exec();

    query.exec("PRAGMA user_version");
    while (query.next()) {
        int version = query.value(0).toInt();

        if (version != CURRENT_DATABASE_VERSION) {
            emit
                finishedAddition(false,
                                 tr("Database versions do not match."),
                                 tr("Current version is %1, file version is %2")
                                     .arg(CURRENT_DATABASE_VERSION)
                                     .arg(version));
            return false;
        }
    }

    query.exec("BEGIN TRANSACTION");

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
        query.exec("DETACH DATABASE db");

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
    query.exec("COMMIT");

    query.exec("DETACH DATABASE db");

    emit finishedAddition(true);

    return true;
}
