#include "favouritesplitter.h"

#include "components/entrysearchresult/resultlistmodel.h"
#include "components/entrysearchresult/resultlistview.h"
#include "logic/settings/settingsutils.h"

#include <QList>
#include <QVariant>

FavouriteSplitter::FavouriteSplitter(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                                     std::shared_ptr<SQLDatabaseManager> manager,
                                     QWidget *parent)
    : QSplitter(parent)
    , _sqlUserUtils{sqlUserUtils}
    , _manager{manager}
{
    setupUI();
    translateUI();

    _sqlUserUtils->searchForAllFavouritedWords();
}

FavouriteSplitter::~FavouriteSplitter()
{
}

void FavouriteSplitter::setupUI()
{
    _entryScrollArea = new EntryScrollArea{_sqlUserUtils, _manager, this};
    _resultListView = new ResultListView{this};

    _model = new ResultListModel{_sqlUserUtils, {}, this};
    static_cast<ResultListModel *>(_model)->setIsFavouritesList(true);
    _resultListView->setModel(_model);

    addWidget(_resultListView);
    addWidget(_entryScrollArea);

    // Don't use QListView::click, since it doesn't respond to changes
    // in the current index if user is navigating with the keyboard
    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &FavouriteSplitter::handleClick);

    connect(_resultListView,
            &QListView::doubleClicked,
            this,
            &FavouriteSplitter::handleDoubleClick);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#ifdef Q_OS_WIN
    setStyleSheet("QSplitter::handle { background-color: #b9b9b9; }"
                  "QSplitter { border-top: 1px solid lightgrey; }");
#elif defined(Q_OS_DARWIN) || defined(Q_OS_LINUX)
    setStyleSheet("QSplitter::handle { background-color: none; }");
#endif
    setMinimumHeight(400);
}

void FavouriteSplitter::translateUI(void)
{
    QString title = tr("Saved Words");
    setWindowTitle(title);
    _sqlUserUtils->searchForAllFavouritedWords();
}

void FavouriteSplitter::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QSplitter::changeEvent(event);
}

void FavouriteSplitter::openCurrentSelectionInNewWindow(void)
{
    QModelIndex entryIndex = _resultListView->currentIndex();
    handleDoubleClick(entryIndex);
}

void FavouriteSplitter::prepareEntry(Entry &entry)
{
    entry.refreshColours(
        Settings::getSettings()
            ->value("entryColourPhoneticType",
                    QVariant::fromValue(EntryColourPhoneticType::JYUTPING))
            .value<EntryColourPhoneticType>());

    return;
}

void FavouriteSplitter::handleClick(const QModelIndex &selection)
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

void FavouriteSplitter::handleDoubleClick(const QModelIndex &selection)
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
