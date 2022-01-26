#ifndef SQLDATABASEMANAGER_H
#define SQLDATABASEMANAGER_H

#include <QSqlDatabase>

// SQLDatabaseManager provides one or more connections to databases
// that contain dictionaries and translations

// The userDatabase is the database that contains user data (e.g. favourites,
// search history, etc.)

constexpr auto DICTIONARY_DATABASE_NAME = "dict.db";
constexpr auto USER_DATABASE_NAME = "user.db";

class SQLDatabaseManager
{
public:
    SQLDatabaseManager();
    ~SQLDatabaseManager();

    QSqlDatabase getDatabase();
    bool isDatabaseOpen() const;
    void closeDatabase();

    QString getDictionaryDatabasePath();
    QString getUserDatabasePath();

    bool backupDictionaryDatabase();
    bool restoreBackedUpDictionaryDatabase();

private:
    void addDatabase(const QString &name) const;
    bool openDatabase(const QString &name);

    QString getLocalDictionaryDatabasePath();
    QString getBundleDictionaryDatabasePath();

    QString getLocalUserDatabasePath();
    QString getBundleUserDatabasePath();

    bool copyDictionaryDatabase();

    bool copyUserDatabase();
    bool attachUserDatabase();

    QString getCurrentDatabaseName() const;

    QString _dictionaryDatabasePath;
    QString _userDatabasePath;
};

#endif // SQLDATABASEMANAGER_H
