#include "sqldatabasemanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

SQLDatabaseManager::SQLDatabaseManager()
{
    // Delete the old version of the dictionary
#ifdef Q_OS_LINUX
    QDir localDir{"/usr/share/jyut-dict/"};
#ifndef PORTABLE
    if (localDir.exists()) {
        if (!localDir.removeRecursively()) {
            //  std::cerr << "Couldn't remove original dictionary!" << std::endl;
            //  return;
        }
    }
#endif
#else
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/eng.db"};
#ifndef PORTABLE
    if (localFile.exists() && localFile.isFile()) {
        if (!QFile::remove(localFile.absoluteFilePath())) {
            //  std::cerr << "Couldn't remove original file!" << std::endl;
            //  return;
        }
    }
#endif
#endif
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
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "./dict.db"};
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/dict.db"};
#else
#ifdef APPIMAGE
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                + "/../share/jyut-dict/dictionaries/dict.db"};
#elif defined(DEBUG)
    QFileInfo bundleFile{"./dict.db"};
#else
    QFileInfo bundleFile{"/usr/share/jyut-dict/dictionaries/dict.db"};
#endif
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/dictionaries/dict.db"};
#endif

#ifdef PORTABLE
    if (bundleFile.exists() && bundleFile.isFile()) {
        _db.setDatabaseName(bundleFile.absoluteFilePath());
    }
#else
    // Make path for dictionary storage
    if (!localFile.exists()) {
        if (!QDir().mkpath(localFile.absolutePath())) {
            return false;
        }
    }

    // Copy file from bundle to Application Support
    if (!localFile.exists() || !localFile.isFile()) {
        if (!QFile::copy(bundleFile.absoluteFilePath(),
                         localFile.absoluteFilePath())) {
            return false;
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
    try {
        bool rv = _db.open();
        if (!rv) {
            std::runtime_error{"Couldn't open database..."};
        }

        copyUserDatabase();
        attachUserDatabase();
    } catch (std::exception &e) {
        return false;
    }

    return true;
}

QSqlDatabase SQLDatabaseManager::getDatabase()
{
    return _db;
}

bool SQLDatabaseManager::isDatabaseOpen()
{
    return _db.isOpen();
}

bool SQLDatabaseManager::copyUserDatabase()
{
    if (!_db.isOpen()) {
        return false;
    }

#ifdef Q_OS_DARWIN
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../Resources/user.db"};
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/user.db"};
#elif defined(Q_OS_WIN)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "./user.db"};
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/user.db"};
#else
#ifdef APPIMAGE
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/dictionaries/user.db"};
#elif defined(DEBUG)
    QFileInfo bundleFile{"./user.db"};
#else
    QFileInfo bundleFile{"/usr/share/jyut-dict/dictionaries/user.db"};
#endif
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/dictionaries/user.db"};
#endif

#ifdef PORTABLE
    _userDatabasePath = bundleFile.absoluteFilePath();
#else
    // Make path for dictionary storage
    if (!localFile.exists()) {
        if (!QDir().mkpath(localFile.absolutePath())) {
            return false;
        }
    }

    // Copy file from bundle to Application Support
    if (!localFile.exists() || !localFile.isFile()) {
        if (!QFile::copy(bundleFile.absoluteFilePath(),
                         localFile.absoluteFilePath())) {
            return false;
        }
    }

    // Delete file in bundle
    if (bundleFile.exists() && bundleFile.isFile()) {
        if (!QFile::remove(bundleFile.absoluteFilePath())) {
            //            std::cerr << "Couldn't remove original file!" << std::endl;
            //            return;
        }
    }

    _userDatabasePath = localFile.absoluteFilePath();
#endif

    return true;
}

bool SQLDatabaseManager::attachUserDatabase()
{
    QSqlQuery query{_db};

    query.prepare("ATTACH DATABASE ? AS user");
    query.addBindValue(QVariant::fromValue(_userDatabasePath));
    query.exec();

    return true;
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
