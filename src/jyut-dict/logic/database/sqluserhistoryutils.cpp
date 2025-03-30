#include "sqluserhistoryutils.h"

#include "logic/database/queryparseutils.h"

#include <QtConcurrent/QtConcurrent>

SQLUserHistoryUtils::SQLUserHistoryUtils(std::shared_ptr<SQLDatabaseManager> manager)
    : _manager{manager}
{
}

void SQLUserHistoryUtils::registerObserver(ISearchObserver *observer)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    _observers.push_back(observer);
}

void SQLUserHistoryUtils::deregisterObserver(ISearchObserver *observer)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    _observers.remove(observer);
}

void SQLUserHistoryUtils::notifyObservers(
    const std::vector<searchTermHistoryItem> &results, bool emptyQuery)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    std::list<ISearchObserver *>::const_iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(results, emptyQuery);
        ++it;
    }
}

void SQLUserHistoryUtils::notifyObservers(const std::vector<Entry> &results,
                                          bool emptyQuery)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    std::list<ISearchObserver *>::const_iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(results, emptyQuery);
        ++it;
    }
}

bool SQLUserHistoryUtils::checkForManager(void) const
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return false;
    }
    return true;
}

void SQLUserHistoryUtils::addSearchToHistory(const std::string &search, int options)
{
    if (!checkForManager()) {
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserHistoryUtils::addSearchToHistoryThread,
                                    this,
                                    search,
                                    options);
}

void SQLUserHistoryUtils::addViewToHistory(const Entry &entry)
{
    if (!checkForManager()) {
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserHistoryUtils::addViewToHistoryThread,
                                    this,
                                    entry);
}

void SQLUserHistoryUtils::searchAllSearchHistory(void)
{
    if (!checkForManager()) {
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserHistoryUtils::searchAllSearchHistoryThread,
                                    this);
}

void SQLUserHistoryUtils::clearAllSearchHistory(void)
{
    if (!checkForManager()) {
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserHistoryUtils::clearAllSearchHistoryThread,
                                    this);
}

void SQLUserHistoryUtils::searchAllViewHistory(void)
{
    if (!checkForManager()) {
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserHistoryUtils::searchAllViewHistoryThread,
                                    this);
}

void SQLUserHistoryUtils::clearAllViewHistory(void)
{
    if (!checkForManager()) {
        return;
    }
    std::ignore = QtConcurrent::run(&SQLUserHistoryUtils::clearAllViewHistoryThread,
                                    this);
}

void SQLUserHistoryUtils::addSearchToHistoryThread(const std::string &search,
                                                   int options)
{
    QSqlQuery query{_manager->getDatabase()};

    query.prepare("INSERT INTO user.search_history "
                  " (search_text, search_options, timestamp) "
                  "VALUES "
                  " (?, ?, datetime(\"now\"))");
    query.addBindValue(search.c_str());
    query.addBindValue(QVariant::fromValue(options));
    query.exec();

    _manager->closeAndRemoveDatabaseConnection();
}

void SQLUserHistoryUtils::addViewToHistoryThread(const Entry &entry)
{
    QSqlQuery query{_manager->getDatabase()};

    query.prepare("INSERT INTO user.view_history "
                  " (traditional, simplified, jyutping, pinyin, timestamp) "
                  "VALUES "
                  " (?, ?, ?, ?, datetime(\"now\"))");
    query.addBindValue(entry.getTraditional().c_str());
    query.addBindValue(entry.getSimplified().c_str());
    query.addBindValue(entry.getJyutping().c_str());
    query.addBindValue(entry.getPinyin().c_str());
    query.exec();

    _manager->closeAndRemoveDatabaseConnection();
}

void SQLUserHistoryUtils::searchAllSearchHistoryThread(void)
{
    std::vector<searchTermHistoryItem> results;

    QSqlQuery query{_manager->getDatabase()};

    query.exec(
        "SELECT search_text AS text, search_options AS options, timestamp "
        "FROM user.search_history "
        "ORDER BY timestamp DESC "
        "LIMIT 1000");

    results = QueryParseUtils::parseHistoryItems(query);
    _manager->closeAndRemoveDatabaseConnection();

    notifyObservers(results, /*emptyQuery=*/false);
}

void SQLUserHistoryUtils::clearAllSearchHistoryThread(void)
{
    QSqlQuery query{_manager->getDatabase()};

    query.exec("DELETE FROM user.search_history");

    _manager->closeAndRemoveDatabaseConnection();

    searchAllSearchHistory();
}

void SQLUserHistoryUtils::searchAllViewHistoryThread(void)
{
    std::vector<Entry> results;

    QSqlQuery query{_manager->getDatabase()};

    query.exec(
        "SELECT traditional, simplified, pinyin, jyutping, timestamp "
        "FROM user.view_history "
        "ORDER BY timestamp DESC "
        "LIMIT 1000");

    results = QueryParseUtils::parseEntries(query, /*parseDefinitions=*/false);
    _manager->closeAndRemoveDatabaseConnection();

    notifyObservers(results, /*emptyQuery=*/false);
}

void SQLUserHistoryUtils::clearAllViewHistoryThread(void)
{
    QSqlQuery query{_manager->getDatabase()};

    query.exec("DELETE FROM user.view_history");

    _manager->closeAndRemoveDatabaseConnection();

    searchAllViewHistory();
}
