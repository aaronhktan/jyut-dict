#ifndef SQLDATABASEUTILS_H
#define SQLDATABASEUTILS_H

#include "logic/database/sqldatabasemanager.h"

#include <memory>

class SQLDatabaseUtils
{
public:
    SQLDatabaseUtils(std::shared_ptr<SQLDatabaseManager> manager);

    bool removeSource(std::string source);
    bool addSource(std::string filepath);

private:
    std::shared_ptr<SQLDatabaseManager> _manager;
};

#endif // SQLDATABASEUTILS_H
