#ifndef SQLDATABASEUTILS_H
#define SQLDATABASEUTILS_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QObject>

#include <memory>
#include <string>
#include <unordered_map>

// The SQLDatabaseUtils class has functions that read and write from the
// database. This is differentiated from the SQLDatabaseManager class,
// which is only responsible for opening and closing a connection to a database.

constexpr auto CURRENT_DATABASE_VERSION = 3;
using conflictingDictionaryMetadata
    = std::vector<std::tuple<std::string, std::string, std::string>>;

class SQLDatabaseUtils : public QObject
{
Q_OBJECT
public:
    SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager);

    bool updateDatabase(void);

    // Note: when adding or removing sources, make sure to update the map in
    // DictionarySourceUtils!
    bool removeSource(const std::string &source, bool skipCleanup = false);
    bool addSource(const std::string &filepath,
                   bool overwriteConflictingSource = false);

    bool readSources(std::vector<std::pair<std::string, std::string>> &sources);
    bool readSources(std::vector<DictionaryMetadata> &sources);

private:
    std::shared_ptr<SQLDatabaseManager> _manager;

    bool backupDatabase(void);

    bool migrateDatabaseFromOneToTwo(void);
    bool migrateDatabaseFromTwoToThree(void);

    bool deleteSourceFromDatabase(const std::string &source);
    bool removeDefinitionsFromDatabase(void);
    bool removeSentencesFromDatabase(void);
    // Note to callers: There CANNOT be a transaction running when this method
    // is called! It does PRAGMA foreign_keys = ON, which is a no-op inside
    // a transaction.
    // If skipCleanup is set to true, the caller MUST call rebuildIndices()
    // after this method returns if indices are desired.
    bool removeSources(const std::vector<std::string> &sources,
                       bool skipCleanup = false);

    std::pair<bool, std::string> insertSourcesIntoDatabase(
        std::unordered_map<std::string, std::string> old_source_ids);
    bool addDefinitionSource(void);
    bool addSentenceSource(void);

    bool dropIndices(void);
    bool rebuildIndices(void);

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

    void conflictingDictionaryNamesExist(
        conflictingDictionaryMetadata dictionaries);
    void insertingSource();
    void insertingEntries();
    void insertingDefinitions();
    void finishedAddition(bool success,
                          QString reason = "",
                          QString description = "");

    void migratingDatabase();
    void finishedMigratingDatabase(bool success);
};

Q_DECLARE_METATYPE(conflictingDictionaryMetadata);

#endif // SQLDATABASEUTILS_H
