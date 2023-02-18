#include "favouritesplitter.h"

#include "components/entrysearchresult/resultlistmodel.h"
#include "components/entrysearchresult/resultlistview.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_WIN
#include "logic/utils/utils_windows.h"
#endif

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

void FavouriteSplitter::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QSplitter::changeEvent(event);
}

void FavouriteSplitter::keyPressEvent(QKeyEvent *event)
{
    if (isWindow() && event->key() == Qt::Key_Escape) {
        close();
    }
}

void FavouriteSplitter::setupUI()
{
    _entryScrollArea = new EntryScrollArea{_sqlUserUtils, _manager, this};
    _resultListView = new ResultListView{this};

    _model = new ResultListModel{_sqlUserUtils, {}, true, this};
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

    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            _entryScrollArea,
            &EntryScrollArea::stallSentenceUIUpdate);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
    setMinimumHeight(400);
#ifdef Q_OS_MAC
    setStyleSheet("QSplitter::handle { "
                  "   background-color: none; "
                  "} ");
#else
    setStyleSheet("QSplitter::handle { "
                  "   background-color: palette(alternate-base); "
                  "} ");
#endif
#ifdef Q_OS_WIN
    setStyle(Utils::isDarkMode());
#endif
}

void FavouriteSplitter::translateUI(void)
{
    QString title = tr("Saved Words");
    setWindowTitle(title);
    _sqlUserUtils->searchForAllFavouritedWords();
}

#ifdef Q_OS_WIN
void FavouriteSplitter::setStyle(bool use_dark)
{
    (void) (use_dark);
    setStyleSheet("QSplitter { border-top: 1px solid palette(alternate-base); }");
}
#endif

void FavouriteSplitter::openCurrentSelectionInNewWindow(void)
{
    QModelIndex entryIndex = _resultListView->currentIndex();
    handleDoubleClick(entryIndex);
}

void FavouriteSplitter::updateStyleRequested(void)
{
    static_cast<ResultListView *>(_resultListView)->paintWithApplicationState();

    QList<EntryScrollArea *> scrollAreas
        = this->findChildren<EntryScrollArea *>();
    foreach (auto &area, scrollAreas) {
        area->updateStyleRequested();
    }
}

void FavouriteSplitter::prepareEntry(Entry &entry)
{
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

    QTimer::singleShot(50, this, [&]() {
        EntryScrollArea *area = new EntryScrollArea{_sqlUserUtils, _manager, nullptr};
        area->setParent(this, Qt::Window);
        area->setEntry(entry);
#ifndef Q_OS_MAC
        area->setWindowTitle(" ");
#endif
        emit area->stallSentenceUIUpdate();
        area->show();
    });
}
