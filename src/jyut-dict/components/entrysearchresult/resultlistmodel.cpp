#include "resultlistmodel.h"

ResultListModel::ResultListModel(std::shared_ptr<ISearchObservable> sqlSearch,
                                 std::vector<Entry> entries,
                                 bool isFavouritesList, QObject *parent)
    : QAbstractListModel(parent),
    _isFavouritesList{isFavouritesList}
{
    _updateModelTimer = new QTimer{this};

    qRegisterMetaType<std::vector<Entry>>();

    if (entries.empty() && !isFavouritesList) {
        setWelcome();
    } else {
        _entries = entries;
    }

    _search = sqlSearch;
    _search->registerObserver(this);

    connect(this,
            &ResultListModel::callbackInvoked,
            this,
            &ResultListModel::copyEntries);
}

ResultListModel::~ResultListModel()
{
    _search->deregisterObserver(this);
}

void ResultListModel::callback(const std::vector<Entry> &entries, bool emptyQuery)
{
    // This function is usually called in another thread (since ISearchObservable
    // objects do their work in a separate thread to avoid congesting the UI thread).
    //
    // Copying entries into the result model is NOT re-entrant. But with Qt's
    // signals/slots mechanism, since the connection is a QueuedConnection,
    // only one copyEntries is called at a time by the main thread
    // AND in the order the callbackInvoked signals came in, because the thread's
    // event loop processes signals as a FIFO queue.
    emit callbackInvoked(entries, emptyQuery);
}

void ResultListModel::copyEntries(const std::vector<Entry> &entries, bool emptyQuery)
{
    // As soon as another event wants to update the list model, kill
    // any prior pending updates by stopping the timer.
    _updateModelTimer->stop();
    disconnect(_updateModelTimer, nullptr, nullptr, nullptr);

    if (entries.empty() && !emptyQuery) {
        _updateModelTimer->setInterval(500);
        _updateModelTimer->setSingleShot(true);
        QObject::connect(_updateModelTimer, &QTimer::timeout, this, [=, this]() {
            setEntries(entries, emptyQuery);
        });
        _updateModelTimer->start();
    } else {
        setEntries(entries, emptyQuery);
    }
}

void ResultListModel::setEntries(const std::vector<Entry> &entries, bool emptyQuery) {
    beginResetModel();
    _entries = entries;
    endResetModel();
    if (_entries.empty() && !emptyQuery) {
        setEmpty();
    }
}

void ResultListModel::setWelcome()
{
    if (_isFavouritesList) {
        setEmpty();
        return;
    }
    Entry entry = Entry{tr("Welcome!").toStdString(),
                        tr("Welcome!").toStdString(),
                        "—",
                        "—",
                        {}};
    entry.addDefinitions("CEDICT",
                         {Definition::Definition{
                             tr("Start typing to search for words").toStdString(),
                             "",
                             {}}});
    entry.setIsWelcome(true);

    setEntries(std::vector<Entry>{entry});
}

void ResultListModel::setEmpty()
{
    if (!_isFavouritesList) {
        Entry entry = Entry{tr("No results...").toStdString(),
                            tr("No results...").toStdString(),
                            "", "", {}};
        entry.addDefinitions("CEDICT",
                             {Definition::Definition{
                                 tr("Simplified Chinese, Traditional Chinese, "
                                    "Jyutping, Pinyin, and English "
                                    "are options beneath the search bar.")
                                     .toStdString(),
                                 "",
                                 {}}});
        entry.setJyutping(tr("Try switching between languages!").toStdString());
        entry.setIsEmpty(true);

        setEntries(std::vector<Entry>{entry});
    } else {
        Entry entry = Entry{tr("Nothing saved...").toStdString(),
                            tr("Nothing saved...").toStdString(),
                            "", "", {}};
        entry
            .addDefinitions("CEDICT",
                            {Definition::Definition{
                                tr("Clicking the \"save\" button when viewing "
                                   "a word or phrase adds it to this list. Try "
                                   "adding a word that sounds cool!")
                                    .toStdString(),
                                "",
                                {}}});
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
