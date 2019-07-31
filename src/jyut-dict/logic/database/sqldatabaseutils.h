#ifndef SQLDATABASEUTILS_H
#define SQLDATABASEUTILS_H

#include "logic/database/sqldatabasemanager.h"

#include <QObject>

#include <memory>

class SQLDatabaseUtils : public QObject
{
Q_OBJECT
public:
    SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager);

    bool removeSource(std::string source);
    bool addSource(std::string filepath);

private:
    std::shared_ptr<SQLDatabaseManager> _manager;

signals:
    void deletingDefinitions();
    void totalToDelete(int number);
    void deletionProgress(int deleted, int total);
    void rebuildingIndexes();
    void cleaningUp();
    void finishedDeletion(bool success);

    void insertingSource();
    void insertingEntries();
    void insertingDefinitions();
    void finishedAddition(bool success);
};

#endif // SQLDATABASEUTILS_H
