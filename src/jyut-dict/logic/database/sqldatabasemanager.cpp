#include "sqldatabasemanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QThread>
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
    QSqlDatabase::database(getCurrentDatabaseName()).close();
}

QSqlDatabase SQLDatabaseManager::getDatabase()
{
    QString name = getCurrentDatabaseName();
    if (!QSqlDatabase::contains(name)) {
        addDatabase(name);
    }
    if (!(QSqlDatabase::database(name, /*open=*/false).isOpen())) {
        openDatabase(name);
    }
    return QSqlDatabase::database(name);
}

void SQLDatabaseManager::closeDatabase()
{
    QSqlDatabase::database(getCurrentDatabaseName(), /*open=*/false).close();
}

bool SQLDatabaseManager::isDatabaseOpen()
{
    return QSqlDatabase::database(getCurrentDatabaseName(), /*open=*/false)
        .isOpen();
}

void SQLDatabaseManager::addDatabase(QString name)
{
    QSqlDatabase::addDatabase("QSQLITE", name);
}

bool SQLDatabaseManager::openDatabase(QString name)
{
    try {
        copyDictionaryDatabase();
        QSqlDatabase::database(name).setDatabaseName(_dictionaryDatabasePath);
        bool rv = QSqlDatabase::database(name).open();
        if (!rv) {
            throw std::runtime_error{"Couldn't open database..."};
        }

        copyUserDatabase();
        rv = attachUserDatabase();
        if (!rv) {
            throw std::runtime_error{"Couldn't attach user database..."};
        }
    } catch (std::exception &e) {
        (void) (e);
        return false;
    }

    return true;
}

bool SQLDatabaseManager::copyDictionaryDatabase()
{
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
        _dictionaryDatabasePath = bundleFile.absoluteFilePath();
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

    _dictionaryDatabasePath = localFile.absoluteFilePath();
#endif

    return true;
}

bool SQLDatabaseManager::copyUserDatabase()
{
    if (!QSqlDatabase::database(getCurrentDatabaseName()).isOpen()) {
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
    QSqlQuery query{QSqlDatabase::database(getCurrentDatabaseName())};

    query.prepare("ATTACH DATABASE ? AS user");
    query.addBindValue(QVariant::fromValue(_userDatabasePath));
    query.exec();

    return !query.lastError().isValid();
}

QString SQLDatabaseManager::getCurrentDatabaseName()
{
    // Generate a unique name for every thread that needs a connection to the
    // database.
    QString name = "database_"
                   + QString::number(reinterpret_cast<uint64_t>(
                                         QThread::currentThread()),
                                     16);
    return name;
}
