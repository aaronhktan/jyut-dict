#ifndef SQLDATABASEMANAGER_H
#define SQLDATABASEMANAGER_H

#include <QSqlDatabase>

#include <shared_mutex>
#include <unordered_set>

// SQLDatabaseManager provides one or more connections to databases
// that contain dictionaries and translations

// The userDatabase is the database that contains user data (e.g. favourites,
// search history, etc.)

class SQLDatabaseManager
{
public:
    SQLDatabaseManager();

    QSqlDatabase getDatabase();
    bool isDatabaseOpen() const;
    void closeAndRemoveDatabaseConnection();

    // This function is dangerous! Make sure that no queries are
    // being run when this is called. Since database connections
    // may be opened from other threads, it is not possible
    // to close them from whichever thread this function is called
    // from.
    bool removeAllDatabaseConnections();

    QString getDictionaryDatabasePath();
    QString getUserDatabasePath();

    bool backupDictionaryDatabase();
    bool restoreBackedUpDictionaryDatabase();

private:
    void addDatabaseConnection(const QString &connectionName) const;
    bool openDatabaseConnection(const QString &connectionName);

    QString getLocalDictionaryDatabasePath();
    QString getBundleDictionaryDatabasePath();

    QString getLocalUserDatabasePath();
    QString getBundleUserDatabasePath();

    bool copyDictionaryDatabase();

    bool copyUserDatabase();
    bool attachUserDatabase();

    QString getConnectionName() const;

    std::unordered_set<std::string> _openConnectionNames;
    std::shared_mutex _mutex;

    QString _dictionaryDatabasePath;
    QString _userDatabasePath;
};

#endif // SQLDATABASEMANAGER_H
