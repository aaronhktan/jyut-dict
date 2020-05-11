#ifndef SQLDATABASEMANAGER_H
#define SQLDATABASEMANAGER_H

#include <QSqlDatabase>

// SQLDatabaseManager provides one or more connections to databases
// that contain dictionaries and translations

// The userDatabase is the database that contains user data (e.g. favourites,
// search history, etc.)

class SQLDatabaseManager
{
public:
    SQLDatabaseManager();
    ~SQLDatabaseManager();

    QSqlDatabase getDatabase();
    bool isDatabaseOpen();
    void closeDatabase();

    QString getDictionaryDatabasePath();
    QString getUserDatabasePath();

private:
    void addDatabase(QString name);
    bool openDatabase(QString name);

    QString getLocalDictionaryDatabasePath();
    QString getBundleDictionaryDatabasePath();

    QString getLocalUserDatabasePath();
    QString getBundleUserDatabasePath();

    bool copyDictionaryDatabase();

    bool copyUserDatabase();
    bool attachUserDatabase();

    QString getCurrentDatabaseName();

    QString _dictionaryDatabasePath;
    QString _userDatabasePath;
};

#endif // SQLDATABASEMANAGER_H
