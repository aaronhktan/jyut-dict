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

// Watch out!
// On Linux, define DEBUG and PORTABLE to find the database in the same directory as executable,
// FLATPAK or APPIMAGE for the image one directory above,
// or nothing for the .deb

namespace {
constexpr auto DICTIONARY_DATABASE_NAME = "dict.db";
constexpr auto USER_DATABASE_NAME = "user.db";
} // namespace

SQLDatabaseManager::SQLDatabaseManager()
{
    // Delete the old version of the dictionary
#ifndef PORTABLE
#ifdef Q_OS_LINUX
    QDir localDir{"/usr/share/jyut-dict/"};
    if (localDir.exists()) {
        if (!localDir.removeRecursively()) {
            //  std::cerr << "Couldn't remove original dictionary!" << std::endl;
            //  return;
        }
    }
#else
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/eng.db"};
    if (localFile.exists() && localFile.isFile()) {
        if (!QFile::remove(localFile.absoluteFilePath())) {
            //  std::cerr << "Couldn't remove original file!" << std::endl;
            //  return;
        }
    }
#endif
#endif
}

QSqlDatabase SQLDatabaseManager::getDatabase()
{
    QString name = getConnectionName();
    if (!QSqlDatabase::contains(name)) {
        addDatabaseConnection(name);
    }
    if (!(QSqlDatabase::database(name, /* open= */ false).isOpen())) {
        openDatabaseConnection(name);
    }
    return QSqlDatabase::database(name);
}

bool SQLDatabaseManager::isDatabaseOpen() const
{
    return QSqlDatabase::database(getConnectionName(), /* open= */ false)
        .isOpen();
}

void SQLDatabaseManager::closeAndRemoveDatabaseConnection()
{
    QSqlDatabase::database(getConnectionName(), /* open= */ false).close();
    QSqlDatabase::removeDatabase(getConnectionName());
    {
        std::lock_guard lock{_mutex};
        _openConnectionNames.extract(getConnectionName().toStdString());
    }
}

bool SQLDatabaseManager::removeAllDatabaseConnections()
{
    std::lock_guard lock{_mutex};
    std::vector<std::string> connectionNames;
    connectionNames.insert(connectionNames.end(),
                           _openConnectionNames.begin(),
                           _openConnectionNames.end());
    for (const auto &connectionName : connectionNames) {
        QSqlDatabase::removeDatabase(QString::fromStdString(connectionName));
        if (!QSqlDatabase::database(QString::fromStdString(connectionName),
                                    /* open = */ false)
                 .isOpen()) {
            _openConnectionNames.erase(connectionName);
        }
    }

    return _openConnectionNames.empty();
}

QString SQLDatabaseManager::getDictionaryDatabasePath()
{
#ifdef PORTABLE
    return getBundleDictionaryDatabasePath();
#else
    return getLocalDictionaryDatabasePath();
#endif
}

QString SQLDatabaseManager::getUserDatabasePath()
{
#ifdef PORTABLE
    return getBundleUserDatabasePath();
#else
    return getLocalUserDatabasePath();
#endif
}

bool SQLDatabaseManager::backupDictionaryDatabase()
{
    QString dir = QFileInfo{getDictionaryDatabasePath()}.absolutePath() + "/";

    // Rename backup 2 -> backup 3; backup 1 -> backup 2
    for (int i = 3; i > 1; i--) {
        QString oldFilePath = dir + DICTIONARY_DATABASE_NAME + "_"
                              + QString::number(i - 1);

        QString backupFilePath = dir + DICTIONARY_DATABASE_NAME + "_"
                                 + QString::number(i);
        if (QFile::exists(backupFilePath)) {
            QFile::remove(backupFilePath);
        }
        if (QFile::exists(oldFilePath)) {
            QFile::rename(oldFilePath, backupFilePath);
        }
    }

    // Create backup 1
    QString dictFilePath = dir + DICTIONARY_DATABASE_NAME;
    QString backupFilePath = dir + DICTIONARY_DATABASE_NAME + "_"
                             + QString::number(1);
    return QFile::copy(dictFilePath, backupFilePath);
}

bool SQLDatabaseManager::restoreBackedUpDictionaryDatabase()
{
    // Note: this function does not work in Windows. The OS prevents
    // the program from deleting the dictionary database file, since the file
    // is still in use by other instances of SQLDatabaseManager.
    QString dir = QFileInfo{getDictionaryDatabasePath()}.absolutePath() + "/";
    QString dictFilePath = dir + DICTIONARY_DATABASE_NAME;
    QString backupFilePath = dir + DICTIONARY_DATABASE_NAME + "_"
                             + QString::number(1);
    if (QFile::exists(backupFilePath)) {
        QFile::remove(dictFilePath);
        return QFile::copy(backupFilePath, dictFilePath);
    } else {
        return false;
    }
}

void SQLDatabaseManager::addDatabaseConnection(const QString &connectionName) const
{
    QSqlDatabase::addDatabase("QSQLITE", connectionName);
}

bool SQLDatabaseManager::openDatabaseConnection(const QString &connectionName)
{
    try {
        copyDictionaryDatabase();

        QSqlDatabase db = QSqlDatabase::database(connectionName);
        db.setDatabaseName(_dictionaryDatabasePath);
        db.setConnectOptions("QSQLITE_ENABLE_REGEXP");
        bool rv = db.open();
        if (!rv) {
            throw std::runtime_error{"Couldn't open database..."};
        }

        copyUserDatabase();
        rv = attachUserDatabase();
        if (!rv) {
            throw std::runtime_error{"Couldn't attach user database..."};
        }

        {
            std::lock_guard lock{_mutex};
            _openConnectionNames.emplace(connectionName.toStdString());
        }
    } catch (std::exception &e) {
        (void) (e);
        return false;
    }

    return true;
}

QString SQLDatabaseManager::getLocalDictionaryDatabasePath()
{
#ifdef Q_OS_DARWIN
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/" + DICTIONARY_DATABASE_NAME};
#elif defined(Q_OS_WIN)
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/" + DICTIONARY_DATABASE_NAME};
#else
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/dictionaries/" + DICTIONARY_DATABASE_NAME};
#endif
    return localFile.absoluteFilePath();
}

QString SQLDatabaseManager::getBundleDictionaryDatabasePath()
{
#ifdef Q_OS_DARWIN
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../Resources/" + DICTIONARY_DATABASE_NAME};
#elif defined(Q_OS_WIN)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "/"
                         + DICTIONARY_DATABASE_NAME};
#else // Q_OS_LINUX
#ifdef APPIMAGE
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/dictionaries/"
                         + DICTIONARY_DATABASE_NAME};
#elif defined(DEBUG)
    QFileInfo bundleFile{"./" + QString{DICTIONARY_DATABASE_NAME}};
#elif defined(FLATPAK)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "/../share/jyut-dict/dictionaries/"
                         + DICTIONARY_DATABASE_NAME};
#else
    QFileInfo bundleFile{"/usr/share/jyut-dict/dictionaries/"
                         + QString{DICTIONARY_DATABASE_NAME}};
#endif
#endif
    return bundleFile.absoluteFilePath();
}

QString SQLDatabaseManager::getLocalUserDatabasePath()
{
#ifdef Q_OS_DARWIN
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/" + USER_DATABASE_NAME};
#elif defined(Q_OS_WIN)
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Dictionaries/" + USER_DATABASE_NAME};
#else
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/dictionaries/" + USER_DATABASE_NAME};
#endif
    return localFile.absoluteFilePath();
}

QString SQLDatabaseManager::getBundleUserDatabasePath()
{
#ifdef Q_OS_DARWIN
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../Resources/" + USER_DATABASE_NAME};
#elif defined(Q_OS_WIN)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() +  "./"
                         + USER_DATABASE_NAME};
#else
#ifdef APPIMAGE
    QFileInfo bundleFile{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/dictionaries/" + USER_DATABASE_NAME};
#elif defined(DEBUG)
    QFileInfo bundleFile{"./" + QString{USER_DATABASE_NAME}};
#elif defined(FLATPAK)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "/../share/jyut-dict/dictionaries/" + USER_DATABASE_NAME};
#else
    QFileInfo bundleFile{"/usr/share/jyut-dict/dictionaries/" + QString{USER_DATABASE_NAME}};
#endif
#endif
    return bundleFile.absoluteFilePath();
}

bool SQLDatabaseManager::copyDictionaryDatabase()
{
#ifdef PORTABLE
    QFileInfo file{getDictionaryDatabasePath()};
    if (file.exists() && file.isFile()) {
        _dictionaryDatabasePath = file.absoluteFilePath();
    }
    return true;
#else
    QFileInfo localFile{getLocalDictionaryDatabasePath()};
    QFileInfo bundleFile{getBundleDictionaryDatabasePath()};

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
            // This is non-fatal, just ignore it.
        }
    }

    _dictionaryDatabasePath = localFile.absoluteFilePath();

    return true;
#endif
}

bool SQLDatabaseManager::copyUserDatabase()
{
    if (!QSqlDatabase::database(getConnectionName()).isOpen()) {
        return false;
    }

#ifdef PORTABLE
    QFileInfo file{getUserDatabasePath()};
    _userDatabasePath = file.absoluteFilePath();
#else
    QFileInfo localFile{getLocalUserDatabasePath()};
    QFileInfo bundleFile{getBundleUserDatabasePath()};

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
            // This is non-fatal, just ignore it.
        }
    }

    _userDatabasePath = localFile.absoluteFilePath();
#endif

    return true;
}

bool SQLDatabaseManager::attachUserDatabase()
{
    QSqlQuery query{QSqlDatabase::database(getConnectionName())};

    query.prepare("ATTACH DATABASE ? AS user");
    query.addBindValue(QVariant::fromValue(_userDatabasePath));
    query.exec();

    return !query.lastError().isValid();
}

QString SQLDatabaseManager::getConnectionName() const
{
    // Generate a unique name for every thread that needs a connection to the
    // database.
    QString name = "database_"
                   + QString::number(reinterpret_cast<uint64_t>(
                                         QThread::currentThread()),
                                     16);
    return name;
}
