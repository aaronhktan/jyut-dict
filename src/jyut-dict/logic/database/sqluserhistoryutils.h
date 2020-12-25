#ifndef SQLUSERHISTORYUTILS_H
#define SQLUSERHISTORYUTILS_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"
#include "logic/search/isearchobservable.h"

#include <QObject>
#include <QtSql>

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

// The SQLUserHistoryUtils class searches the user database
// for items regarding history (search history and view history).

typedef std::pair<std::string, int> searchTermHistoryItem;

class SQLUserHistoryUtils : public QObject,
                            virtual public ISearchObservable
{
public:
    SQLUserHistoryUtils(std::shared_ptr<SQLDatabaseManager> manager);
    ~SQLUserHistoryUtils() override;

    void registerObserver(ISearchObserver *observer) override;
    void deregisterObserver(ISearchObserver *observer) override;

    void addSearchToHistory(std::string search, int options);
    void addViewToHistory(Entry entry);

    void searchAllSearchHistory(void);
    void clearAllSearchHistory(void);

    void searchAllViewHistory(void);
    void clearAllViewHistory(void);

private:
    void notifyObservers(const std::vector<searchTermHistoryItem> &results,
                         bool emptyQuery) override;
    void notifyObservers(const std::vector<Entry> &results,
                         bool emptyQuery) override;
    bool checkForManager(void);

    void addSearchToHistoryThread(std::string search, int options);
    void addViewToHistoryThread(Entry entry);

    void searchAllSearchHistoryThread(void);
    void clearAllSearchHistoryThread(void);

    void searchAllViewHistoryThread(void);
    void clearAllViewHistoryThread(void);

    std::list<ISearchObserver *> _observers;

    std::shared_ptr<SQLDatabaseManager> _manager;

    std::mutex _databaseMutex;
    std::mutex _notifyMutex;
};

Q_DECLARE_METATYPE(searchTermHistoryItem);

#endif // SQLUSERHISTORYUTILS_H
