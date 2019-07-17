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
    _EnglishDB.close();
//    _FrenchDB.close();
}

#include <QDebug>
void SQLDatabaseManager::openEnglishDatabase()
{
    if (_EnglishDB.isOpen()) {
        return;
    }

    _EnglishDB = QSqlDatabase::addDatabase("QSQLITE");

#ifdef Q_OS_DARWIN
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "/../Resources/dict.db"};
    QFileInfo localFile{QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
                 + "/Dictionaries/dict.db"};
#elif defined(Q_OS_WIN)
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "./eng.db"};
    QFileInfo localFile{QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
                 + "/Dictionaries/eng.db"};
#else
    QFileInfo bundleFile{QCoreApplication::applicationDirPath() + "/eng.db"};
    QFileInfo localFile{"/usr/share/jyut-dict/dictionaries/eng.db"};
#endif

#ifdef PORTABLE
    qDebug() << bundleFile.absoluteFilePath();
    qDebug() << bundleFile.exists();
    qDebug() << bundleFile.isFile();
    if (bundleFile.exists() && bundleFile.isFile()) {
        _EnglishDB.setDatabaseName(bundleFile.absoluteFilePath());
        qDebug() << bundleFile.absoluteFilePath();
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
        if (!QFile::copy(bundleFile.absoluteFilePath(), localFile.absoluteFilePath())) {
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

    _EnglishDB.setDatabaseName(localFile.absoluteFilePath());
#endif
    _EnglishDB.open();
}

//void SQLDatabaseManager::openOtherDatabase()
//{
//    _FrenchDB = QSqlDatabase::addDatabase("QSQLITE");
//    _FrenchDB.setDatabaseName(QCoreApplication::applicationDirPath() + "/../Resources/fra.db");
//    _FrenchDB.open();
//}

QSqlDatabase SQLDatabaseManager::getEnglishDatabase()
{
    return _EnglishDB;
}

bool SQLDatabaseManager::isEnglishDatabaseOpen()
{
    return _EnglishDB.isOpen();
}

//QSqlDatabase SQLDatabaseManager::getFrenchDatabase()
//{
//    return _FrenchDB;
//}

//bool SQLDatabaseManager::isFrenchDatabaseOpen()
//{
//    return _FrenchDB.isOpen();
//}
