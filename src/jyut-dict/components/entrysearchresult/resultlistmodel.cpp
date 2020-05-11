#include "resultlistmodel.h"

ResultListModel::ResultListModel(std::shared_ptr<ISearchObservable> sqlSearch,
                                 std::vector<Entry> entries, QObject *parent)
    : QAbstractListModel(parent)
{
    if (entries.empty()) {
        setWelcome();
    } else {
        _entries = entries;
    }

    _search = sqlSearch;
    _search->registerObserver(this);
}

ResultListModel::~ResultListModel()
{
    _search->deregisterObserver(this);
}

void ResultListModel::callback(const std::vector<Entry> entries, bool emptyQuery)
{
    setEntries(entries, emptyQuery);
}

void ResultListModel::setEntries(std::vector<Entry> entries)
{
    setEntries(entries, false);
}

void ResultListModel::setEntries(std::vector<Entry> entries, bool emptyQuery) {
    beginResetModel();
    _entries = entries;
    if (_entries.empty() && !emptyQuery) {
        setEmpty();
    }
    endResetModel();
}

void ResultListModel::setWelcome()
{
    if (_isFavouritesList) {
        setEmpty();
        return;
    }
    Entry entry = Entry{tr("Welcome!").toStdString(), tr("Welcome!").toStdString(),
                        "—", "—", {}, {}, {}};
    entry.addDefinitions("CEDICT",
                         {tr("Start typing to search for words").toStdString()});
    entry.setIsWelcome(true);

    setEntries(std::vector<Entry>{entry});
}

void ResultListModel::setEmpty()
{
    if (!_isFavouritesList) {
        Entry entry = Entry{tr("No results...").toStdString(),
                            tr("No results...").toStdString(),
                            "", "", {}, {}, {}};
        entry.addDefinitions("CEDICT",
                             {tr("Simplified (SC) and Traditional (TC) Chinese, "
                                 "Jyutping (JP), Pinyin (PY), and English (EN) "
                                 "are options to the right of the search bar.")
                                  .toStdString()});
        entry.setJyutping(tr("Try switching between languages!").toStdString());
        entry.setIsEmpty(true);

        setEntries(std::vector<Entry>{entry});
    } else {
        Entry entry = Entry{tr("Nothing saved...").toStdString(),
                            tr("Nothing saved...").toStdString(),
                            "", "", {}, {}, {}};
        entry.addDefinitions("CEDICT",
                             {tr("Clicking the \"save\" button when viewing "
                                 "a word or phrase adds it to this list. Try "
                                 "adding a word that sounds cool!")
                                  .toStdString()});
        entry.setJyutping(tr("Save a word to get started!").toStdString());
        entry.setIsEmpty(true);

        setEntries(std::vector<Entry>{entry});
    }
}

void ResultListModel::setIsFavouritesList(bool isFavouritesList)
{
    _isFavouritesList = isFavouritesList;
}

int ResultListModel::rowCount(const QModelIndex &parent) const
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

QVariant ResultListModel::data(const QModelIndex &index, int role) const
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

QVariant ResultListModel::headerData(int section, Qt::Orientation orientation,
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
