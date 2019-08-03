#include "sqldatabasemanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

SQLDatabaseManager::SQLDatabaseManager()
{

}

SQLDatabaseManager::~SQLDatabaseManager()
{
    _db.close();
}

bool SQLDatabaseManager::openDatabase()
{
    if (_db.isOpen()) {
        return true;
    }

    _db = QSqlDatabase::addDatabase("QSQLITE");

#ifdef Q_OS_DARWIN
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../Resources/dict.db"};
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/dict.db"};
#elif defined(Q_OS_WIN)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "./eng.db"};
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/eng.db"};
#else
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "/eng.db"};
    QFileInfo localFile{"/usr/share/jyut-dict/dictionaries/eng.db"};
#endif

#ifdef PORTABLE
    if (bundleFile.exists() && bundleFile.isFile()) {
        _db.setDatabaseName(bundleFile.absoluteFilePath());
    }
#else
    // Make path for dictionary storage
    if (!localFile.exists()) {
        if (!QDir().mkpath(localFile.absolutePath())) {
            return;
        }
    }

    // Copy file from bundle to Application Support
    if (!localFile.exists() || !localFile.isFile()) {
        if (!QFile::copy(bundleFile.absoluteFilePath(),
                         localFile.absoluteFilePath())) {
            return;
        }
    }

    // Delete file in bundle
    if (bundleFile.exists() && bundleFile.isFile()) {
        if (!QFile::remove(bundleFile.absoluteFilePath())) {
            //            std::cerr << "Couldn't remove original file!" << std::endl;
            //            return;
        }
    }

    _db.setDatabaseName(localFile.absoluteFilePath());
#endif
    return _db.open();
}

QSqlDatabase SQLDatabaseManager::getDatabase()
{
    return _db;
}

bool SQLDatabaseManager::isDatabaseOpen()
{
    return _db.isOpen();
}

void SQLDatabaseManager::openEnglishDatabase()
{
    openDatabase();
}

QSqlDatabase SQLDatabaseManager::getEnglishDatabase()
{
    return getDatabase();
}

bool SQLDatabaseManager::isEnglishDatabaseOpen()
{
    return isDatabaseOpen();
}
