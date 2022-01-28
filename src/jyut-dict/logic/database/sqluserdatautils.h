#ifndef SQLUSERDATAUTILS_H
#define SQLUSERDATAUTILS_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"
#include "logic/search/isearchobservable.h"

#include <QObject>
#include <QtSql>

#include <list>
#include <memory>
#include <mutex>
#include <vector>

// The SQLUserDataUtils class searches the user database
// It offers convenient functions like searching for all saved words,
// whether a particular word was saved, and more.

class SQLUserDataUtils : public QObject,
                         virtual public ISearchObservable
{
public:
    SQLUserDataUtils(std::shared_ptr<SQLDatabaseManager> manager);

    void registerObserver(ISearchObserver *observer) override;
    void deregisterObserver(ISearchObserver *observer) override;

    void searchForAllFavouritedWords(void);
    void checkIfEntryHasBeenFavourited(const Entry &entry);
    void favouriteEntry(const Entry &entry);
    void unfavouriteEntry(const Entry &entry);

private:
    void notifyObservers(const std::vector<Entry> &results,
                         bool emptyQuery) override;
    void notifyObservers(bool entryExists, const Entry &entry) override;

    void searchForAllFavouritedWordsThread(void);
    void checkIfEntryHasBeenFavouritedThread(const Entry &entry);
    void favouriteEntryThread(const Entry &entry);
    void unfavouriteEntryThread(const Entry &entry);

    std::mutex _notifyMutex;
    std::list<ISearchObserver *> _observers;

    std::shared_ptr<SQLDatabaseManager> _manager;
};

#endif // SQLUSERDATAUTILS_H
