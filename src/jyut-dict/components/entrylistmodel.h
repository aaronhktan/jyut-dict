#ifndef ENTRYLISTMODEL_H
#define ENTRYLISTMODEL_H

#include "logic/entry/entry.h"
#include "logic/search/isearchobservable.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"

#include <QAbstractListModel>

#include <string>
#include <vector>

// The EntryListModel contains data (a vector of Entry objects)
// It is populated with the results of a search, being a searchobserver

// Entries are returned as QVariants when an index is provided
// Header data override is "good manners", but currently is not useful

class EntryListModel : public QAbstractListModel, public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryListModel(std::vector<Entry> entries,
                            QObject *parent = nullptr);

    void callback(std::vector<Entry> entries) override;
    void setEntries(std::vector<Entry> entries);
    void setWelcome();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<Entry> _entries;

    ISearchObservable *_search;
};

#endif // ENTRYLISTMODEL_H
