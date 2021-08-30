#include "viewhistorylistmodel.h"

ViewHistoryListModel::ViewHistoryListModel(std::shared_ptr<ISearchObservable> sqlHistoryUtils,
                                           QObject *parent)
    : QAbstractListModel(parent)
    , _search{sqlHistoryUtils}
{
    _search->registerObserver(this);
}

ViewHistoryListModel::~ViewHistoryListModel()
{
    _search->deregisterObserver(this);
}

void ViewHistoryListModel::callback(const std::vector<Entry> entries, bool emptyQuery)
{
    setEntries(entries, emptyQuery);
}

void ViewHistoryListModel::setEntries(std::vector<Entry> entries)
{
    setEntries(entries, false);
}

void ViewHistoryListModel::setEntries(std::vector<Entry> entries, bool emptyQuery) {
    beginResetModel();
    _entries = entries;
    if (_entries.empty() && !emptyQuery) {
        setEmpty();
    }
    endResetModel();
}

void ViewHistoryListModel::setEmpty(void)
{
    Entry entry = Entry{tr("No viewed items...").toStdString(),
                        tr("No viewed items...").toStdString(),
                        "", "", {}};
    entry.addDefinitions(
        "CEDICT",
        {Definition::Definition{tr("After viewing an entry, you can find it "
                                   "in this list. Selecting an item in this "
                                   "list will let you see that entry again.")
                                    .toStdString(),
                                "",
                                {}}});
    entry.setJyutping(tr("—").toStdString());
    entry.setIsEmpty(true);

    setEntries(std::vector<Entry>{entry});
}

void ViewHistoryListModel::translateUI(void)
{
    try {
        if (_entries.at(0).isEmpty()) {
            setEmpty();
        }
    } catch (std::exception &e) {
        (void) (e);
        setEmpty();
    }
}

int ViewHistoryListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return static_cast<int>(_entries.size());
    }

    if (static_cast<unsigned long>(parent.row()) >= _entries.size()) {
        return static_cast<int>(_entries.size());
    }

    return static_cast<int>(_entries.size() - 1
                            - static_cast<unsigned long>(parent.row()));
}

QVariant ViewHistoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (static_cast<unsigned long>(index.row()) >= _entries.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QVariant var;
        var.setValue(_entries.at(static_cast<unsigned long>(index.row())));
        return var;
    } else {
        return QVariant();
    }
}

QVariant ViewHistoryListModel::headerData(int section, Qt::Orientation orientation,
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
