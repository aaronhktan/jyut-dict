#include "sqluserhistoryutils.h"

#include "logic/utils/utils.h"

#include <QtConcurrent/QtConcurrent>

SQLUserHistoryUtils::SQLUserHistoryUtils(std::shared_ptr<SQLDatabaseManager> manager)
    : _manager{manager}
{
}

SQLUserHistoryUtils::~SQLUserHistoryUtils() {}

void SQLUserHistoryUtils::registerObserver(ISearchObserver *observer)
{
    _observers.push_back(observer);
}

void SQLUserHistoryUtils::deregisterObserver(ISearchObserver *observer)
{
    _observers.remove(observer);
}

void SQLUserHistoryUtils::notifyObservers(const std::vector<std::pair<std::string, int>> &results,
                                          bool emptyQuery)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    std::list<ISearchObserver *>::iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(results, emptyQuery);
        ++it;
    }
}

void SQLUserHistoryUtils::notifyObservers(const std::vector<Entry> &results,
                                          bool emptyQuery)
{
    std::lock_guard<std::mutex> notifyLock{_notifyMutex};
    std::list<ISearchObserver *>::iterator it = _observers.begin();
    while (it != _observers.end()) {
        (static_cast<ISearchObserver *>(*it))->callback(results, emptyQuery);
        ++it;
    }
}

void SQLUserHistoryUtils::searchAllSearchHistory(void)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    QtConcurrent::run(this,
                      &SQLUserHistoryUtils::searchAllSearchHistoryThread);
}

void SQLUserHistoryUtils::clearAllSearchHistory(void)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    QtConcurrent::run(this,
                      &SQLUserHistoryUtils::clearAllSearchHistoryThread);
}

void SQLUserHistoryUtils::searchAllViewHistory(void)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    QtConcurrent::run(this,
                      &SQLUserHistoryUtils::searchAllViewHistoryThread);
}

void SQLUserHistoryUtils::clearAllViewHistory(void)
{
    if (!_manager) {
        std::cout << "No database specified!" << std::endl;
        return;
    }
    QtConcurrent::run(this,
                      &SQLUserHistoryUtils::clearAllViewHistoryThread);
}

void SQLUserHistoryUtils::searchAllSearchHistoryThread(void)
{
    std::vector<Entry> results{};

    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
        QSqlQuery query{_manager->getDatabase()};

        query.exec(
            "SELECT search_text AS text, search_options AS options, timestamp "
            "FROM user.search_history "
            "ORDER BY timestamp DESC");

        results = parseEntries(query);
        _manager->closeDatabase();
    }

    notifyObservers(results, /*emptyQuery=*/false);
}

void SQLUserHistoryUtils::clearAllSearchHistoryThread(void)
{
    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
        QSqlQuery query{_manager->getDatabase()};

        query.exec("DELETE FROM user.search_history");

        _manager->closeDatabase();
    }
}

void SQLUserHistoryUtils::searchAllViewHistoryThread(void)
{
    std::vector<Entry> results{};

    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
        QSqlQuery query{_manager->getDatabase()};

        query.exec(
            "SELECT traditional, simplified, pinyin, jyutping, timestamp "
            "FROM user.view_history "
            "ORDER BY timestamp DESC");

        results = parseEntries(query);
        _manager->closeDatabase();
    }

    notifyObservers(results, /*emptyQuery=*/false);
}

void SQLUserHistoryUtils::clearAllViewHistoryThread(void)
{
    {
        std::lock_guard<std::mutex> databaseLock(_databaseMutex);
        QSqlQuery query{_manager->getDatabase()};

        query.exec("DELETE FROM user.view_history");

        _manager->closeDatabase();
    }
}

std::vector<std::pair<std::string, int>> SQLUserHistoryUtils::parseStrings(
    QSqlQuery &query)
{
    std::vector<std::pair<std::string, int>> results;

    int textIndex = query.record().indexOf("text");
    int optionsIndex = query.record().indexOf("options");

    while (query.next()) {
        std::string text = query.value(textIndex).toString().toStdString();
        int options = query.value(optionsIndex).toInt();

        results.push_back(std::pair<std::string, int>{text, options});
    }

    return results;
}

std::vector<Entry> SQLUserHistoryUtils::parseEntries(QSqlQuery &query)
{
    std::vector<Entry> entries;

    int simplifiedIndex = query.record().indexOf("simplified");
    int traditionalIndex = query.record().indexOf("traditional");
    int jyutpingIndex = query.record().indexOf("jyutping");
    int pinyinIndex = query.record().indexOf("pinyin");

    while (query.next()) {
        // Get fields from table
        std::string simplified
            = query.value(simplifiedIndex).toString().toStdString();
        std::string traditional
            = query.value(traditionalIndex).toString().toStdString();
        std::string jyutping
            = query.value(jyutpingIndex).toString().toStdString();
        std::string pinyin = query.value(pinyinIndex).toString().toStdString();

        entries.push_back(Entry(simplified,
                                traditional,
                                jyutping,
                                pinyin,
                                {}, {}, {}));
    }

    return entries;
}
