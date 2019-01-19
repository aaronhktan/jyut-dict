#include "entrylistmodel.h"

EntryListModel::EntryListModel(std::vector<Entry> entries, QObject *parent)
    : QAbstractListModel(parent)
{
    if (entries.empty()) {
        setWelcome();
    } else {
        _entries = entries;
    }

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

void EntryListModel::setWelcome()
{
    Entry entry = Entry{tr("Welcome!").toStdString(), tr("Welcome!").toStdString(),
                        "—", "—", {}, {}, {}};
    entry.addDefinitions(DictionarySource::CEDICT, {tr("Start typing to search for words").toStdString()});

    setEntries(std::vector<Entry>{entry});
}

int EntryListModel::rowCount(const QModelIndex &parent __unused) const
{
    return static_cast<int>(_entries.size());
}

QVariant EntryListModel::data(const QModelIndex &index, int role) const
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
