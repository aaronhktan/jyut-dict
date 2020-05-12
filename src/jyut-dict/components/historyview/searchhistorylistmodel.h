#ifndef SEARCHHISTORYLISTMODEL_H
#define SEARCHHISTORYLISTMODEL_H

#include "logic/database/sqluserhistoryutils.h"
#include "logic/search/isearchobservable.h"
#include "logic/search/isearchobserver.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include <string>
#include <utility>
#include <vector>

// The ResultListModel contains data (a vector of Entry objects)
// It is populated with the results of a search, being a searchobserver

// Entries are returned as QVariants when an index is provided
// Header data override is "good manners", but currently is not useful

class SearchHistoryListModel : public QAbstractListModel, public ISearchObserver
{
    Q_OBJECT

public:
    explicit SearchHistoryListModel(std::shared_ptr<ISearchObservable> sqlHistoryUtils,
                                    QObject *parent = nullptr);
    ~SearchHistoryListModel() override;

    void callback(const std::vector<std::pair<std::string, int>> searchTerms,
                  bool emptyQuery) override;
    void setEntries(std::vector<std::pair<std::string, int>> searchTerms);
    void setEntries(std::vector<std::pair<std::string, int>> searchTerms, bool emptyQuery);
    void setEmpty();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<std::pair<std::string, int>> _searchTerms;

    std::shared_ptr<ISearchObservable> _sqlHistoryUtils;
};

#endif // SEARCHHISTORYLISTMODEL_H
