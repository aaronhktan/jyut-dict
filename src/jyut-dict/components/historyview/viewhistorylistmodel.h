#ifndef VIEWHISTORYLISTMODEL_H
#define VIEWHISTORYLISTMODEL_H

#include "logic/entry/entry.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/search/isearchobservable.h"
#include "logic/search/isearchobserver.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include <string>
#include <vector>

// The ViewHistoryListModel contains data (a vector of
// searchTermHistoryItem pairs) that represents past searches
// It is populated with the results of a search, being a searchobserver

// Entries are returned as QVariants when an index is provided
// Header data override is "good manners", but currently is not useful

class ViewHistoryListModel : public QAbstractListModel, public ISearchObserver
{
    Q_OBJECT

public:
    explicit ViewHistoryListModel(
        std::shared_ptr<ISearchObservable> sqlHistoryUtils,
        QObject *parent = nullptr);
    ~ViewHistoryListModel() override;

    void callback(const std::vector<Entry> entries, bool emptyQuery) override;
    void setEntries(std::vector<Entry> entries);
    void setEntries(std::vector<Entry> entries, bool emptyQuery);
    void setEmpty();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<Entry> _entries;

    std::shared_ptr<ISearchObservable> _search;
};

#endif // VIEWHISTORYLISTMODEL_H
