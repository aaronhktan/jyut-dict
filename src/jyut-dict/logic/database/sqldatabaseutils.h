#ifndef SQLDATABASEUTILS_H
#define SQLDATABASEUTILS_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/dictionary/dictionarymetadata.h"

#include <QObject>

#include <memory>
#include <string>

constexpr auto CURRENT_DATABASE_VERSION = 1;

class SQLDatabaseUtils : public QObject
{
Q_OBJECT
public:
    SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager);

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
