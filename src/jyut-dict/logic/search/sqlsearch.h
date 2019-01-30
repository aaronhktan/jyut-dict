#ifndef SQLSEARCH_H
#define SQLSEARCH_H

#include "logic/entry/entry.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchobservable.h"
#include "logic/search/sqldatabasemanager.h"

#include <QtSql>

#include <string>

// SQLSearch searches the database provided by SQLDatabaseManager.

class SQLSearch : public ISearch, public ISearchObservable
{
public:
    void registerObserver(ISearchObserver *observer) override;
    void deregisterObserver(ISearchObserver *observer) override;

    SQLSearch();

    void searchSimplified(const QString& searchTerm) override;
    void searchTraditional(const QString& searchTerm) override;
    void searchJyutping(const QString& searchTerm) override;
    void searchPinyin(const QString& searchTerm) override;
    void searchEnglish(const QString& searchTerm) override;

private:
    void notifyObservers() override;

    std::vector<std::string> explodePhonetic(const QString& string, const char delimiter);
    std::string implodePhonetic(std::vector<std::string> words, const char *delimiter);
    std::vector<Entry> parseEntries(QSqlQuery query);

    static std::list<ISearchObserver *> _observers;
    std::vector<Entry> _results;

    static SQLDatabaseManager *_manager;
    QSqlQuery _query;
};

#endif // SQLSEARCH_H
