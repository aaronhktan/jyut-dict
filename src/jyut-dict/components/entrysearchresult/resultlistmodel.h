#ifndef RESULTLISTMODEL_H
#define RESULTLISTMODEL_H

#include "logic/entry/entry.h"
#include "logic/search/isearchobservable.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include <string>
#include <vector>

// The ResultListModel contains data (a vector of Entry objects)
// It is populated with the results of a search, being a searchobserver

// Entries are returned as QVariants when an index is provided
// Header data override is "good manners", but currently is not useful

class ResultListModel : public QAbstractListModel, public ISearchObserver
{
    Q_OBJECT

public:
    explicit ResultListModel(std::shared_ptr<ISearchObservable> sqlSearch,
                             std::vector<Entry> entries,
                             QObject *parent = nullptr);
    ~ResultListModel() override;

    void callback(const std::vector<Entry> entries, bool emptyQuery) override;
    void setEntries(std::vector<Entry> entries);
    void setEntries(std::vector<Entry> entries, bool emptyQuery);
    void setWelcome();
    void setEmpty();

    void setIsFavouritesList(bool isFavouritesList);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    bool _isFavouritesList = false;

    std::vector<Entry> _entries;

    std::shared_ptr<ISearchObservable> _search;
};

#endif // RESULTLISTMODEL_H
