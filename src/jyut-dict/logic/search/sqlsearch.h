#ifndef SQLSEARCH_H
#define SQLSEARCH_H

#include "logic/entry/entry.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchobservable.h"

#include <QtSql>

#include <string>

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

    std::vector<Entry> parseEntries(QSqlQuery query);

    static std::list<ISearchObserver *> _observers;
    std::vector<Entry> _results;
    QSqlQuery _query;
};

#endif // SQLSEARCH_H
