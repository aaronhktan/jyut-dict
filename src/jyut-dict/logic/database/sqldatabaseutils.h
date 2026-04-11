#ifndef SQLDATABASEUTILS_H
#define SQLDATABASEUTILS_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/source/sourcemetadata.h"

#include <QObject>

#include <memory>
#include <span>
#include <string>
#include <unordered_map>

class QSqlDatabase;

// The SQLDatabaseUtils class has functions that read and write from the
// database. This is differentiated from the SQLDatabaseManager class,
// which is only responsible for opening and closing a connection to a database.

constexpr auto CURRENT_DATABASE_VERSION = 4;
using conflictingSourceMetadata
    = std::vector<std::tuple<std::string, std::string, std::string>>;

class SQLDatabaseUtils : public QObject
{
Q_OBJECT
public:
    bool updateDatabase(QSqlDatabase &db);

    // Note: when adding or removing sources, make sure to update the map in
    // DictionarySourceUtils!
    bool removeSource(QSqlDatabase &db,
                      const std::string &source,
                      const std::shared_ptr<SQLDatabaseManager> &manager
                      = nullptr,
                      bool skipCleanup = false);
    bool addSource(QSqlDatabase &db,
                   const std::string &filepath,
                   const std::shared_ptr<SQLDatabaseManager> &manager = nullptr,
                   bool overwriteConflictingSource = false);

    bool readSources(QSqlDatabase &db,
                     std::vector<std::pair<std::string, std::string>> &sources);
    bool readSources(QSqlDatabase &db, std::vector<SourceMetadata> &sources);

    bool mergeDatabases(const std::vector<std::string> &paths);

private:
    bool migrateDatabaseFromOneToTwo(QSqlDatabase &db);
    bool migrateDatabaseFromTwoToThree(QSqlDatabase &db);
    bool migrateDatabaseFromThreeToFour(QSqlDatabase &db);

    bool deleteSourceFromDatabase(QSqlDatabase &db, const std::string &source);
    bool removeDefinitionsFromDatabase(QSqlDatabase &db);
    bool removeSentencesFromDatabase(QSqlDatabase &db);
    // Note to callers: There CANNOT be a transaction running when this method
    // is called! It does PRAGMA foreign_keys = ON, which is a no-op inside
    // a transaction.
    // If skipCleanup is set to true, the caller MUST call rebuildIndices()
    // after this method returns if indices are desired.
    bool removeSources(QSqlDatabase &db,
                       std::span<const std::string> sources,
                       bool skipCleanup = false);

    std::pair<bool, std::string> insertSourcesIntoDatabase(
        QSqlDatabase &db,
        std::unordered_map<std::string, std::string> old_source_ids);
    bool addDefinitionSource(QSqlDatabase &db);
    bool addSentenceSource(QSqlDatabase &db);

    bool dropIndices(QSqlDatabase &db);
    bool rebuildIndices(QSqlDatabase &db);

signals:
    void deletingDefinitions();
    void totalToDelete(int number);
    void deletionProgress(int deleted, int total);
    void rebuildingIndexes();
    void cleaningUp();
    void finishedDeletion(bool success,
                          QString reason = "",
                          QString description = "");

    void deletingSentences();

    void conflictingDictionaryNamesExist(conflictingSourceMetadata dictionaries);
    void insertingSource();
    void insertingEntries();
    void insertingDefinitions();
    void finishedAddition(bool success,
                          QString reason = "",
                          QString description = "");

    void migratingDatabase();
    void finishedMigratingDatabase(bool success);
};

Q_DECLARE_METATYPE(conflictingSourceMetadata);

#endif // SQLDATABASEUTILS_H
