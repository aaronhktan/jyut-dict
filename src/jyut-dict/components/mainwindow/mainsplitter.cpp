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
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _analytics = new Analytics{this};

    _entryScrollArea = new EntryScrollArea{sqlUserUtils, manager, this};
    _resultListView = new ResultListView{this};
    _model = new ResultListModel{sqlSearch, {}, this};
    _resultListView->setModel(_model);

    addWidget(_resultListView);
    addWidget(_entryScrollArea);

    // Don't use QListView::click, since it doesn't respond to changes
    // in the current index if user is navigating with the keyboard
    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &MainSplitter::handleClick);

    connect(_resultListView,
            &QListView::doubleClicked,
            this,
            &MainSplitter::handleDoubleClick);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#ifdef Q_OS_WIN
    setStyleSheet("QSplitter::handle { background-color: #b9b9b9; }");
#elif defined(Q_OS_DARWIN)
    setStyleSheet("QSplitter::handle { background-color: none; }");
#else
    setStyleSheet("QSplitter::handle { background-color: lightgray; }");
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

void MainSplitter::prepareEntry(Entry &entry)
{
    if (Settings::getSettings()
            ->value("Advanced/analyticsEnabled", QVariant{true})
            .toBool()) {
        _analytics->sendEvent("entry",
                              "view",
                              entry.getTraditional() + " / "
                                  + entry.getSimplified());
    }

    entry.refreshColours(
        Settings::getSettings()
            ->value("entryColourPhoneticType",
                    QVariant::fromValue(EntryColourPhoneticType::JYUTPING))
            .value<EntryColourPhoneticType>());

    _sqlHistoryUtils->addViewToHistory(entry);

    return;
}

void MainSplitter::handleClick(const QModelIndex &selection)
{
    Entry entry = qvariant_cast<Entry>(selection.data());
    bool isWelcomeEntry = entry.isWelcome();
    bool isEmptyEntry = entry.isEmpty();
    if (isWelcomeEntry || isEmptyEntry) {
        return;
    }

    prepareEntry(entry);
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

    prepareEntry(entry);

    QTimer::singleShot(50, this, [=]() {
        EntryScrollArea *area = new EntryScrollArea{_sqlUserUtils, _manager, nullptr};
        area->setParent(this, Qt::Window);
        area->setEntry(entry);
#ifndef Q_OS_MAC
        area->setWindowTitle(" ");
#endif
        area->show();
    });
}
