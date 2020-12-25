#include "mainsplitter.h"

#include "components/entrysearchresult/resultlistmodel.h"
#include "components/entrysearchresult/resultlistview.h"
#include "logic/settings/settingsutils.h"

#include <QList>
#include <QVariant>

MainSplitter::MainSplitter(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                           std::shared_ptr<SQLDatabaseManager> manager,
                           std::shared_ptr<SQLSearch> sqlSearch,
                           std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                           QWidget *parent)
    : QSplitter(parent)
    , _sqlUserUtils{sqlUserUtils}
    , _manager{manager}
    , _search{sqlSearch}
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _addToHistoryTimer = new QTimer{this};

    _entryScrollArea = new EntryScrollArea{sqlUserUtils, manager, this};
    _resultListView = new ResultListView{this};
    _model = new ResultListModel{sqlSearch, {}, false, this};
    _resultListView->setModel(_model);

    addWidget(_resultListView);
    addWidget(_entryScrollArea);

    // Don't use QListView::click, since it doesn't respond to changes
    // in the current index if user is navigating with the keyboard
    connect(_model,
            &QAbstractItemModel::modelReset,
            this,
            &MainSplitter::handleModelReset);

    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &MainSplitter::handleClick);

    connect(_resultListView,
            &QListView::doubleClicked,
            this,
            &MainSplitter::handleDoubleClick);

    connect(this,
            &MainSplitter::forwardSearchBarTextChange,
            _entryScrollArea,
            &EntryScrollArea::stallUIUpdate);

    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            _entryScrollArea,
            &EntryScrollArea::stallUIUpdate);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setStretchFactor(0, 0);
    setStretchFactor(1, 1);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#ifdef Q_OS_WIN
    setStyleSheet("QSplitter::handle { background-color: #b9b9b9; }");
#elif defined(Q_OS_DARWIN) || defined(Q_OS_LINUX)
    setStyleSheet("QSplitter::handle { background-color: none; }");
#endif
}

MainSplitter::~MainSplitter()
{

}

void MainSplitter::translateUI(void)
{
    static_cast<ResultListModel *>(_model)->setWelcome();
}

void MainSplitter::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QSplitter::changeEvent(event);
}

void MainSplitter::setFocusToResults(void)
{
    _resultListView->setFocus();
    QModelIndex entryIndex = _resultListView->model()->index(0, 0);
    _resultListView->setCurrentIndex(entryIndex);
    _resultListView->scrollTo(entryIndex);
}

void MainSplitter::openCurrentSelectionInNewWindow(void)
{
    QModelIndex entryIndex = _resultListView->currentIndex();
    handleDoubleClick(entryIndex);
}

void MainSplitter::forwardViewHistoryItem(Entry &entry)
{
    // Disable adding this item to history; _addToHistory should be reset as
    // soon as the results for this search come back so future views can be
    // added to history.
    // The current hack is to reset this variable to true when the model gets
    // reset (aka the some search result is available). It is possible that
    // the results come from another source, like searching, but in that case
    // we lose at most one entry. I consider this to be an acceptable tradeoff.
    _addToHistory = false;
    _search->searchByUnique(entry.getSimplified().c_str(),
                            entry.getTraditional().c_str(),
                            entry.getJyutping().c_str(),
                            entry.getPinyin().c_str());
}

void MainSplitter::prepareEntry(Entry &entry)
{
    prepareEntry(entry, true);
}

void MainSplitter::prepareEntry(Entry &entry, bool addToHistory)
{
    if (addToHistory) {
        // Only add to history after a few seconds of viewing an entry
        _addToHistoryTimer->stop();
        disconnect(_addToHistoryTimer, nullptr, nullptr, nullptr);
        _addToHistoryTimer->setSingleShot(true);
        connect(_addToHistoryTimer, &QTimer::timeout, this, [=]() {
            _sqlHistoryUtils->addViewToHistory(entry);
        });
        _addToHistoryTimer->start(1000);
    }

    entry.refreshColours(
        Settings::getSettings()
            ->value("entryColourPhoneticType",
                    QVariant::fromValue(EntryColourPhoneticType::JYUTPING))
            .value<EntryColourPhoneticType>());

    return;
}

void MainSplitter::handleModelReset(void)
{
    QModelIndex index = _model->index(0, 0);
    _resultListView->setCurrentIndex(index);
    _addToHistory = true;
}

void MainSplitter::handleClick(const QModelIndex &selection)
{
    Entry entry = qvariant_cast<Entry>(selection.data());
    bool isWelcomeEntry = entry.isWelcome();
    bool isEmptyEntry = entry.isEmpty();
    if (isWelcomeEntry || isEmptyEntry) {
        return;
    }

    prepareEntry(entry, _addToHistory);
    _entryScrollArea->setEntry(entry);
}

void MainSplitter::handleDoubleClick(const QModelIndex &selection)
{
    Entry entry = qvariant_cast<Entry>(selection.data());
    bool isWelcomeEntry = entry.isWelcome();
    bool isEmptyEntry = entry.isEmpty();
    if (isWelcomeEntry || isEmptyEntry) {
        return;
    }

    prepareEntry(entry, _addToHistory);

    QTimer::singleShot(50, this, [=]() {
        EntryScrollArea *area = new EntryScrollArea{_sqlUserUtils, _manager, nullptr};
        area->setParent(this, Qt::Window);
        area->setEntry(entry);
#ifndef Q_OS_MAC
        area->setWindowTitle(" ");
#endif
        emit area->stallUIUpdate();
        area->show();
    });
}
