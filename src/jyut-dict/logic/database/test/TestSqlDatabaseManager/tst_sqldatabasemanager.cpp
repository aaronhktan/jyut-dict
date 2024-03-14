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
