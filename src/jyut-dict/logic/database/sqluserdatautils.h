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

private:
    void notifyObservers(const std::vector<Entry> &results,
                         bool emptyQuery) override;
    void notifyObservers(bool entryExists) override;

    void setCurrentSearchTerm(const QString &searchTerm);

    void searchForAllFavouritedWordsThread(void);
    void checkIfEntryHasBeenFavouritedThread(Entry entry);

    std::vector<Entry> parseEntries(QSqlQuery &query);
    bool parseExistence(QSqlQuery &query);

    std::list<ISearchObserver *> _observers;

    std::shared_ptr<SQLDatabaseManager> _manager;

    Entry _currentEntry;

    std::mutex _databaseMutex;
    std::mutex _currentEntryMutex;
    std::mutex _notifyMutex;
};

#endif // SQLUSERDATAUTILS_H
