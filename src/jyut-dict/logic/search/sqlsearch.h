#ifndef SQLSEARCH_H
#define SQLSEARCH_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchobservable.h"

#include <QObject>
#include <QtSql>

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// SQLSearch searches the database provided by SQLDatabaseManager.

class SQLSearch : public QObject,
                  virtual public ISearch,
                  virtual public ISearchObservable
{
public:
    SQLSearch();
    SQLSearch(std::shared_ptr<SQLDatabaseManager> manager);
    SQLSearch(const SQLSearch &search);
    ~SQLSearch() override;

    void registerObserver(ISearchObserver *observer) override;
    void deregisterObserver(ISearchObserver *observer) override;

    void searchSimplified(const QString &searchTerm) override;
    void searchTraditional(const QString &searchTerm) override;
    void searchJyutping(const QString &searchTerm) override;
    void searchPinyin(const QString &searchTerm) override;
    void searchEnglish(const QString &searchTerm) override;

private:
    void notifyObservers() override;

    void runThread(void (SQLSearch::*threadFunction)(const QString &searchTerm),
                   const QString &searchTerm);
    void searchSimplifiedThread(const QString &searchTerm);
    void searchTraditionalThread(const QString &searchTerm);
    void searchJyutpingThread(const QString &searchTerm);
    void searchPinyinThread(const QString &searchTerm);
    void searchEnglishThread(const QString &searchTerm);

    std::vector<std::string> explodePhonetic(const QString &string,
                                             const char delimiter);
    std::string implodePhonetic(std::vector<std::string> words,
                                const char *delimiter,
                                bool surroundWithQuotes=false);
    std::vector<Entry> parseEntries(QSqlQuery &query);

    static std::list<ISearchObserver *> _observers;
    std::vector<Entry> _results;

    std::shared_ptr<SQLDatabaseManager> _manager;
    QString _currentSearchString;
    QSqlQuery _query;
    std::mutex _queryMutex;
};

Q_DECLARE_METATYPE(SQLSearch);

#endif // SQLSEARCH_H
