#include "searchhistorylistmodel.h"

SearchHistoryListModel::SearchHistoryListModel(std::shared_ptr<ISearchObservable> sqlHistoryUtils,
                                               QObject *parent)
    : QAbstractListModel(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    setEmpty();
    _sqlHistoryUtils->registerObserver(this);
}

SearchHistoryListModel::~SearchHistoryListModel()
{
    _sqlHistoryUtils->deregisterObserver(this);
}

void SearchHistoryListModel::callback(const std::vector<searchTermHistoryItem> &searchTerms,
                                      bool emptyQuery)
{
    setEntries(searchTerms, emptyQuery);
}

void SearchHistoryListModel::setEntries(const std::vector<searchTermHistoryItem> &searchTerms)
{
    setEntries(searchTerms, false);
}

void SearchHistoryListModel::setEntries(const std::vector<searchTermHistoryItem> &searchTerms, bool emptyQuery) {
    beginResetModel();
    _searchTerms = searchTerms;
    if (_searchTerms.empty() && !emptyQuery) {
        setEmpty();
    }
    endResetModel();
}

void SearchHistoryListModel::setEmpty(void)
{
    searchTermHistoryItem pair{tr("No search history...").toStdString(),
                                     -1};

    setEntries(std::vector<searchTermHistoryItem>{pair});
}

void SearchHistoryListModel::translateUI(void)
{
    try {
        if (_searchTerms[0].second == -1) {
            setEmpty();
        }
    } catch (std::exception &e) {
        (void) (e);
        setEmpty();
    }
}

int SearchHistoryListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return static_cast<int>(_searchTerms.size());
    }

    if (static_cast<unsigned long>(parent.row()) >= _searchTerms.size()) {
        return static_cast<int>(_searchTerms.size());
    }

    return static_cast<int>(_searchTerms.size() - 1
                            - static_cast<unsigned long>(parent.row()));
}

QVariant SearchHistoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (static_cast<unsigned long>(index.row()) >= _searchTerms.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QVariant var;
        var.setValue(_searchTerms.at(static_cast<unsigned long>(index.row())));
        return var;
    } else {
        return QVariant();
    }
}

QVariant SearchHistoryListModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return QString("Row %1").arg(section);
    } else {
        return QVariant();
    }
}
