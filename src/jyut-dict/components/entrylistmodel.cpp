#include "entrylistmodel.h"

EntryListModel::EntryListModel(std::vector<Entry> entries, QObject *parent)
    : QAbstractListModel(parent)
{
    _entries = entries;

    _search = new SQLSearch();
    _search->registerObserver(this);
}

void EntryListModel::callback(std::vector<Entry> entries)
{
    setEntries(entries);
}

void EntryListModel::setEntries(std::vector<Entry> entries)
{
    beginResetModel();
    _entries = entries;
    endResetModel();
}

int EntryListModel::rowCount(const QModelIndex &parent) const
{
    return static_cast<int>(_entries.size());
}

QVariant EntryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= _entries.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QVariant var;
        var.setValue(_entries.at(index.row()));
        return var;
    } else {
        return QVariant();
    }
}

QVariant EntryListModel::headerData(int section, Qt::Orientation orientation, int role) const
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
