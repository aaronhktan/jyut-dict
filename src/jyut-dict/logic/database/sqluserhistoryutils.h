#ifndef SQLUSERHISTORYUTILS_H
#define SQLUSERHISTORYUTILS_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"
#include "logic/search/isearchobservable.h"

#include <QObject>

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

// The SQLUserHistoryUtils class searches the user database
// for items regarding history (search history and view history).

using searchTermHistoryItem = std::pair<std::string, long>;

class SQLUserHistoryUtils : public QObject,
                            virtual public ISearchObservable
{
public:
    SQLUserHistoryUtils(std::shared_ptr<SQLDatabaseManager> manager);

    void registerObserver(ISearchObserver *observer) override;
    void deregisterObserver(ISearchObserver *observer) override;

    void addSearchToHistory(const std::string &search, int options);
    void addViewToHistory(const Entry &entry);

    void searchAllSearchHistory(void);
    void clearAllSearchHistory(void);

    void searchAllViewHistory(void);
    void clearAllViewHistory(void);

private:
    using ISearchObservable::notifyObservers;
    void notifyObservers(const std::vector<searchTermHistoryItem> &results,
                         bool emptyQuery);
    void notifyObservers(const std::vector<Entry> &results,
                         bool emptyQuery) override;
    bool checkForManager(void) const;

    void addSearchToHistoryThread(const std::string &search, int options);
    void addViewToHistoryThread(const Entry &entry);

    void searchAllSearchHistoryThread(void);
    void clearAllSearchHistoryThread(void);

    void searchAllViewHistoryThread(void);
    void clearAllViewHistoryThread(void);

    std::mutex _notifyMutex;
    std::list<ISearchObserver *> _observers;

    std::shared_ptr<SQLDatabaseManager> _manager;
};

Q_DECLARE_METATYPE(searchTermHistoryItem);

#endif // SQLUSERHISTORYUTILS_H
