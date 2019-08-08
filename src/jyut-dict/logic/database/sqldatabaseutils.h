#ifndef SQLDATABASEUTILS_H
#define SQLDATABASEUTILS_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QObject>

#include <memory>
#include <string>

// The SQLDatabaseUtils class has functions that read and write from the
// database. This is differentiated from the SQLDatabaseManager class,
// which is only responsible for opening and closing a connection to a database.

constexpr auto CURRENT_DATABASE_VERSION = 1;

class SQLDatabaseUtils : public QObject
{
Q_OBJECT
public:
    SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager);

    // Note: when adding or removing sources, make sure to update the map in
    // DictionarySourceUtils!
    bool removeSource(std::string source);
    bool addSource(std::string filepath);

    bool readSources(std::vector<std::pair<std::string, std::string>> &sources);
    bool readSources(std::vector<DictionaryMetadata> &sources);

private:
    std::shared_ptr<SQLDatabaseManager> _manager;

signals:
    void deletingDefinitions();
    void totalToDelete(int number);
    void deletionProgress(int deleted, int total);
    void rebuildingIndexes();
    void cleaningUp();
    void finishedDeletion(bool success, QString reason="", QString description="");

    void insertingSource();
    void insertingEntries();
    void insertingDefinitions();
    void finishedAddition(bool success, QString reason="", QString description="");
};

#endif // SQLDATABASEUTILS_H
