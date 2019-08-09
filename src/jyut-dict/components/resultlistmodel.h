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
    explicit ResultListModel(std::vector<Entry> entries,
                            QObject *parent = nullptr);
    ~ResultListModel() override;

    void callback(std::vector<Entry> entries) override;
    void setEntries(std::vector<Entry> entries);
    void setWelcome();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<Entry> _entries;

    ISearchObservable *_search;
};

#endif // RESULTLISTMODEL_H
