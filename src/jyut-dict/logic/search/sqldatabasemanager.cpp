#include "sqldatabasemanager.h"

#include <QDir>
#include <QFile>

SQLDatabaseManager::SQLDatabaseManager(const QString& path)
{
    QFile::copy(path, QDir::currentPath() + "/test.db");
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(QDir::currentPath() + "/test.db");
    _db.open();
}

SQLDatabaseManager::~SQLDatabaseManager()
{
    _db.close();
}
