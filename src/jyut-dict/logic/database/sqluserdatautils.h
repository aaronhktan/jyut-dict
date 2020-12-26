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
    ~SQLUserDataUtils() override;

    void registerObserver(ISearchObserver *observer) override;
    void deregisterObserver(ISearchObserver *observer) override;

    void searchForAllFavouritedWords(void);
    void checkIfEntryHasBeenFavourited(Entry entry);
    void favouriteEntry(Entry entry);
    void unfavouriteEntry(Entry entry);

private:
    void notifyObservers(const std::vector<Entry> &results,
                         bool emptyQuery) override;
    void notifyObservers(bool entryExists, Entry entry) override;

    void setCurrentSearchTerm(const QString &searchTerm);

    void searchForAllFavouritedWordsThread(void);
    void checkIfEntryHasBeenFavouritedThread(Entry entry);
    void favouriteEntryThread(Entry entry);
    void unfavouriteEntryThread(Entry entry);

    std::list<ISearchObserver *> _observers;

    std::shared_ptr<SQLDatabaseManager> _manager;

    Entry _currentEntry;

    std::mutex _databaseMutex;
    std::mutex _currentEntryMutex;
    std::mutex _notifyMutex;
};

#endif // SQLUSERDATAUTILS_H
