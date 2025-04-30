#include <QtTest>

#include "logic/database/sqldatabasemanager.h"

#include <QSqlDatabase>

class TestSqlDatabaseManager : public QObject
{
    Q_OBJECT

public:
    TestSqlDatabaseManager();
    ~TestSqlDatabaseManager();

private slots:
    void getDatabase();
    void removeAllDatabaseConnections();
    void backupAndRestore();
};

TestSqlDatabaseManager::TestSqlDatabaseManager() {}

TestSqlDatabaseManager::~TestSqlDatabaseManager() {}

void TestSqlDatabaseManager::getDatabase()
{
    SQLDatabaseManager manager;
    QCOMPARE(manager.isDatabaseOpen(), false);

    QSqlDatabase database = manager.getDatabase();
    QCOMPARE(manager.isDatabaseOpen(), true);
    QCOMPARE(database.isOpen(), true);
    QCOMPARE(database.isValid(), true);

    manager.closeAndRemoveDatabaseConnection();
    QCOMPARE(manager.isDatabaseOpen(), false);
    QCOMPARE(database.isOpen(), false);
}

void TestSqlDatabaseManager::removeAllDatabaseConnections()
{
#ifdef Q_OS_MAC
    QSKIP("Functionality is broken on macOS");
#endif

    std::shared_ptr<SQLDatabaseManager> manager
        = std::make_shared<SQLDatabaseManager>();
    std::mutex mutex;
    std::condition_variable cv;
    QCOMPARE(manager->isDatabaseOpen(), false);

    QSqlDatabase database = manager->getDatabase();
    QCOMPARE(manager->isDatabaseOpen(), true);
    QCOMPARE(database.isOpen(), true);
    QCOMPARE(database.isValid(), true);
    QString mainThreadConnectionName = database.connectionName();

    auto future = std::async([&]() {
        QSqlDatabase database = manager->getDatabase();
        QCOMPARE(database.isOpen(), true);
        QCOMPARE(database.connectionName() != mainThreadConnectionName, true);

        // Now that we've opened a connection, proceed in the main thread...
        cv.notify_one();

        {
            // Wait until the main thread has killed our connection...
            std::unique_lock lock{mutex};
            cv.wait(lock);

            // ... and verify that it's dead
            QCOMPARE(database.isOpen(), false);
            return;
        }
    });

    {
        std::unique_lock lock{mutex};
        cv.wait(lock);
    }

    // We got notified by the async thread, which means there are two
    // connections open (one from the main thread, one from the async
    // thread). Force-kill all of them.
    manager->removeAllDatabaseConnections();
    QCOMPARE(manager->isDatabaseOpen(), false);
    QCOMPARE(database.isOpen(), false);

    // Signal the async thread so it can check that its database got
    // closed.
    cv.notify_one();
    future.get();
}

void TestSqlDatabaseManager::backupAndRestore()
{
    SQLDatabaseManager manager;

    QFile databaseFile{manager.getDictionaryDatabasePath()};
    QDir databaseDir{QFileInfo{databaseFile.fileName()}.absolutePath()};
    QCOMPARE(databaseDir.mkpath(
                 QFileInfo{databaseFile.fileName()}.absolutePath()),
             true);
    QCOMPARE(databaseFile.open(QIODevice::ReadWrite), true);
    QCOMPARE(databaseFile.exists(), true);

    QCOMPARE(manager.backupDictionaryDatabase(), true);
    QCOMPARE(databaseFile.exists(), true);

    QFile databaseBackupFile{manager.getDictionaryDatabasePath() + "_1"};
    QCOMPARE(databaseBackupFile.open(QIODevice::ReadOnly), true);
    QCOMPARE(databaseBackupFile.exists(), true);

    QCOMPARE(databaseFile.remove(), true);
    QCOMPARE(databaseFile.exists(), false);

    QCOMPARE(manager.restoreBackedUpDictionaryDatabase(), true);

    QCOMPARE(databaseBackupFile.exists(), true);
    QCOMPARE(databaseFile.exists(), true);

#ifdef Q_OS_WINDOWS
    // On Windows, open files cannot be deleted.
    databaseBackupFile.close();
#endif

    QCOMPARE(manager.backupDictionaryDatabase(), true);
    QFile databaseBackupBackupFile{manager.getDictionaryDatabasePath() + "_2"};
    QCOMPARE(databaseBackupBackupFile.exists(), true);
    QCOMPARE(databaseBackupBackupFile.open(QIODevice::ReadOnly), true);

    QCOMPARE(databaseFile.remove(), true);
    QCOMPARE(databaseBackupFile.remove(), true);
    QCOMPARE(databaseBackupBackupFile.remove(), true);
}

QTEST_MAIN(TestSqlDatabaseManager)

#include "tst_sqldatabasemanager.moc"
