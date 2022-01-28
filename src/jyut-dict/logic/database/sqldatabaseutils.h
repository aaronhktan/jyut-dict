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
typedef std::vector<std::tuple<std::string, std::string, std::string>>
    conflictingDictionaryMetadata;

class SQLDatabaseUtils : public QObject
{
Q_OBJECT
public:
    SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager);

    bool updateDatabase(void) const;

    // Note: when adding or removing sources, make sure to update the map in
    // DictionarySourceUtils!
    bool removeSource(const std::string &source, bool skipCleanup = false) const;
    bool addSource(const std::string &filepath, bool overwriteConflictingSource = false) const;

    bool readSources(std::vector<std::pair<std::string, std::string>> &sources) const;
    bool readSources(std::vector<DictionaryMetadata> &sources) const;

private:
    std::shared_ptr<SQLDatabaseManager> _manager;

    bool backupDatabase(void) const;

    bool migrateDatabaseFromOneToTwo(void) const;
    bool migrateDatabaseFromTwoToThree(void) const;

    bool deleteSourceFromDatabase(const std::string &source) const;
    bool removeDefinitionsFromDatabase(void) const;
    bool removeSentencesFromDatabase(void) const;
    // Note to callers: There CANNOT be a transaction running when this method
    // is called! It does PRAGMA foreign_keys = ON, which is a no-op inside
    // a transaction.
    // If skipCleanup is set to true, the caller MUST call rebuildIndices()
    // after this method returns if indices are desired.
    bool removeSources(const std::vector<std::string> &sources,
                       bool skipCleanup = false) const;

    std::pair<bool, std::string> insertSourcesIntoDatabase(
        std::unordered_map<std::string, std::string> old_source_ids) const;
    bool addDefinitionSource(void) const;
    bool addSentenceSource(void) const;

    bool dropIndices(void) const;
    bool rebuildIndices(void) const;

signals:
    void deletingDefinitions() const;
    void totalToDelete(int number) const;
    void deletionProgress(int deleted, int total) const;
    void rebuildingIndexes() const;
    void cleaningUp() const;
    void finishedDeletion(bool success,
                          QString reason = "",
                          QString description = "") const;

    void deletingSentences() const;

    void conflictingDictionaryNamesExist(
        conflictingDictionaryMetadata dictionaries) const;
    void insertingSource() const;
    void insertingEntries() const;
    void insertingDefinitions() const;
    void finishedAddition(bool success,
                          QString reason = "",
                          QString description = "") const;

    void migratingDatabase() const;
    void finishedMigratingDatabase(bool success) const;
};

Q_DECLARE_METATYPE(conflictingDictionaryMetadata);

#endif // SQLDATABASEUTILS_H
