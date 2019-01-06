#ifndef SQLDATABASEMANAGER_H
#define SQLDATABASEMANAGER_H

#include <QSqlDatabase>

class SQLDatabaseManager
{
public:
    SQLDatabaseManager(const QString& path);
    ~SQLDatabaseManager();

private:
    QSqlDatabase _db;
};

#endif // SQLDATABASEMANAGER_H
