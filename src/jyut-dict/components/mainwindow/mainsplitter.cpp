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
            &EntryScrollArea::stallSentenceUIUpdate);

    connect(this,
            &MainSplitter::forwardSearchBarTextChange,
            _entryScrollArea,
            &EntryScrollArea::stallEntryUIUpdate);

    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            _entryScrollArea,
            &EntryScrollArea::stallSentenceUIUpdate);

    connect(this,
            &MainSplitter::favouriteCurrentEntry,
            _entryScrollArea,
            &EntryScrollArea::favouriteCurrentEntryRequested);

    connect(this,
            &MainSplitter::shareCurrentEntry,
            _entryScrollArea,
            &EntryScrollArea::shareCurrentEntryRequested);

    connect(this,
            &MainSplitter::openCurrentEntryInNewWindow,
            _entryScrollArea,
            &EntryScrollArea::openCurrentEntryInNewWindowRequested);

    connect(this,
            &MainSplitter::magnifyCurrentEntry,
            _entryScrollArea,
            &EntryScrollArea::magnifyCurrentEntryRequested);

    connect(this,
            &MainSplitter::viewAllSentences,
            _entryScrollArea,
            &EntryScrollArea::viewAllSentencesRequested);

    connect(this,
            &MainSplitter::searchEntriesBeginning,
            _entryScrollArea,
            &EntryScrollArea::searchEntriesBeginningRequested);

    connect(this,
            &MainSplitter::searchEntriesContaining,
            _entryScrollArea,
            &EntryScrollArea::searchEntriesContainingRequested);

    connect(this,
            &MainSplitter::searchEntriesEnding,
            _entryScrollArea,
            &EntryScrollArea::searchEntriesEndingRequested);

    connect(_entryScrollArea,
            &EntryScrollArea::searchQuery,
            this,
            &MainSplitter::searchQueryRequested);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setStretchFactor(0, 0);
    setStretchFactor(1, 1);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    setStyleSheet("QSplitter::handle { "
                  "   background-color: none; "
                  "} ");
#else
    setStyleSheet("QSplitter::handle { "
                  "   background-color: palette(alternate-base); "
                  "} ");
#endif
}

void MainSplitter::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QSplitter::changeEvent(event);
}

void MainSplitter::setFocusToResults(void) const
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

void MainSplitter::translateUI(void)
{
    static_cast<ResultListModel *>(_model)->setWelcome();
}

void MainSplitter::forwardViewHistoryItem(const Entry &entry)
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

void MainSplitter::updateStyleRequested(void)
{
    static_cast<ResultListView *>(_resultListView)->paintWithApplicationState();

    QList<EntryScrollArea *> scrollAreas
        = this->findChildren<EntryScrollArea *>();
    foreach (auto &area, scrollAreas) {
        area->updateStyleRequested();
    }
}

void MainSplitter::prepareEntry(Entry &entry)
{
    prepareEntry(entry, true);
}

void MainSplitter::prepareEntry(Entry &entry, bool addToHistory) const
{
    if (addToHistory) {
        // Only add to history after a few seconds of viewing an entry
        _addToHistoryTimer->stop();
        disconnect(_addToHistoryTimer, nullptr, nullptr, nullptr);
        _addToHistoryTimer->setSingleShot(true);
        connect(_addToHistoryTimer, &QTimer::timeout, this, [=, this]() {
            _sqlHistoryUtils->addViewToHistory(entry);
        });
        _addToHistoryTimer->start(1000);
    }

    entry.refreshColours(
        Settings::getSettings()
            ->value("entryColourPhoneticType",
                    QVariant::fromValue(EntryColourPhoneticType::CANTONESE))
            .value<EntryColourPhoneticType>());

    CantoneseOptions cantoneseOptions
        = Settings::getSettings()
              ->value("Entry/cantonesePronunciationOptions",
                      QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
              .value<CantoneseOptions>();
    MandarinOptions mandarinOptions
        = Settings::getSettings()
              ->value("Entry/mandarinPronunciationOptions",
                      QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
              .value<MandarinOptions>();
    entry.generatePhonetic(cantoneseOptions, mandarinOptions);

    cantoneseOptions = Settings::getSettings()
                           ->value("Preview/cantonesePronunciationOptions",
                                   QVariant::fromValue(
                                       CantoneseOptions::RAW_JYUTPING))
                           .value<CantoneseOptions>();
    mandarinOptions = Settings::getSettings()
                          ->value("Preview/mandarinPronunciationOptions",
                                  QVariant::fromValue(
                                      MandarinOptions::PRETTY_PINYIN))
                          .value<MandarinOptions>();
    entry.generateDefinitionsPhonetic(cantoneseOptions, mandarinOptions);
}

void MainSplitter::handleModelReset(void)
{
    QModelIndex index = _model->index(0, 0);
    _resultListView->setCurrentIndex(index);
    _addToHistory = true;

    // Assumption: a model reset occurs because the user has entered a new query
    // Therefore, we want to emit forwardSearchBarTextChange, so that
    // all the behaviour associated with "user enters something on their
    // keyboard" occurs.
    emit forwardSearchBarTextChange();
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

    QTimer::singleShot(50, this, [=, this]() {
        EntryScrollArea *area = new EntryScrollArea{_sqlUserUtils,
                                                    _manager,
                                                    nullptr};
        area->setParent(this, Qt::Window);
        area->setEntry(entry);
#ifndef Q_OS_MAC
        area->setWindowTitle(" ");
#endif
        emit area->stallSentenceUIUpdate();
        area->show();
    });
}

void MainSplitter::favouriteCurrentEntryRequested(void)
{
    emit favouriteCurrentEntry();
}

void MainSplitter::shareCurrentEntryRequested(void)
{
    emit shareCurrentEntry();
}

void MainSplitter::openCurrentEntryInNewWindowRequested(void)
{
    emit openCurrentEntryInNewWindow();
}

void MainSplitter::magnifyCurrentEntryRequested(void)
{
    emit magnifyCurrentEntry();
}

void MainSplitter::viewAllSentencesRequested(void)
{
    emit viewAllSentences();
}

void MainSplitter::searchEntriesBeginningRequested(void)
{
    emit searchEntriesBeginning();
}

void MainSplitter::searchEntriesContainingRequested(void)
{
    emit searchEntriesContaining();
}

void MainSplitter::searchEntriesEndingRequested(void)
{
    emit searchEntriesEnding();
}

void MainSplitter::searchQueryRequested(const QString &query,
                                        const SearchParameters &parameters)
{
    emit searchQuery(query, parameters);
}
