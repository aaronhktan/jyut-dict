#include "sqldatabasemanager.h"

#include <QDir>
#include <QFile>

SQLDatabaseManager::SQLDatabaseManager()
{
}

void SQLDatabaseManager::openEnglishDatabase(const QString &path)
{
    QFile::copy(path, QDir::currentPath() + "/eng.db");
    if (!_EnglishDB.isOpen()) {
        _EnglishDB = QSqlDatabase::addDatabase("QSQLITE");
        _EnglishDB.setDatabaseName(QDir::currentPath() + "/eng.db");
        _EnglishDB.open();
    }
}

//void SQLDatabaseManager::openOtherDatabase(const QString &path)
//{
//    QFile::copy(path, QDir::currentPath() + "/fra.db");
//    _FrenchDB = QSqlDatabase::addDatabase("QSQLITE");
//    _FrenchDB.setDatabaseName(QDir::currentPath() + "/fra.db");
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

SQLDatabaseManager::~SQLDatabaseManager()
{
    _EnglishDB.close();
//    _FrenchDB.close();
}
