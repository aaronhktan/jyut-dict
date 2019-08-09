#ifndef SQLDATABASEMANAGER_H
#define SQLDATABASEMANAGER_H

#include <QSqlDatabase>

// SQLDatabaseManager provides one or more connections to databases
// that contain dictionaries and translations

class SQLDatabaseManager
{
public:
    SQLDatabaseManager();
    ~SQLDatabaseManager();

    bool openDatabase();
    QSqlDatabase getDatabase();
    bool isDatabaseOpen();

    void openEnglishDatabase(); // Deprecated, use openDatabase() instead
    QSqlDatabase getEnglishDatabase(); // Deprecated, use getDatabase() instead
    bool isEnglishDatabaseOpen(); // Deprecated, use isDatabaseOpen() instead

private:
    QSqlDatabase _db;
};

#endif // SQLDATABASEMANAGER_H
