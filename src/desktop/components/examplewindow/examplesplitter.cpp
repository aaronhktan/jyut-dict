#include "examplesplitter.h"

#include "components/examplesearchresult/exampleresultlistmodel.h"
#include "components/examplesearchresult/exampleresultlistview.h"
#include "components/exampleview/examplescrollarea.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/example/example.h"
#include "logic/search/sqlsearch.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_WIN
#include "logic/utils/utils_windows.h"
#endif

#include <QAbstractListModel>
#include <QEvent>
#include <QKeyEvent>
#include <QList>
#include <QListView>
#include <QModelIndex>
#ifdef Q_OS_WIN
#include <QTimer>
#endif
#include <QVariant>

ExampleSplitter::ExampleSplitter(std::shared_ptr<SQLDatabaseManager> manager,
                                 QWidget *parent)
    : QSplitter(parent)
    , _manager{manager}
{
    setMinimumSize(QSize{700, 450});

    _sqlSearch = std::make_shared<SQLSearch>(manager);
    _model = new ExampleResultListModel{_sqlSearch, {}, this};

    _exampleScrollArea = new ExampleScrollArea{this};
    _resultListView = new ExampleResultListView{this};
    _resultListView->setModel(_model);

    addWidget(_resultListView);
    addWidget(_exampleScrollArea);

    // Don't use QListView::click, since it doesn't respond to changes
    // in the current index if user is navigating with the keyboard
    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &ExampleSplitter::handleClick);

    connect(_resultListView,
            &QListView::doubleClicked,
            this,
            &ExampleSplitter::handleDoubleClick);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
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

void ExampleSplitter::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [this] { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QSplitter::changeEvent(event);
}

void ExampleSplitter::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

#ifdef Q_OS_WIN
void ExampleSplitter::setStyle([[maybe_unused]] bool use_dark)
{
    setStyleSheet("QSplitter { border-top: 1px solid palette(alternate-base); }");
}
#endif

void ExampleSplitter::setExamples(std::span<const Example> examples)
{
    static_cast<ExampleResultListModel *>(_model)->setExamples(examples);
    _size = static_cast<int>(examples.size());
}

void ExampleSplitter::setSearchTerm(const QString &searchTerm)
{
    _searchTerm = searchTerm;
    translateUI();
}

void ExampleSplitter::translateUI(void)
{
    QString title;
    if (_size == 1) {
        title = QString{tr("Examples for %1 (%2 result)")}.arg(_searchTerm,
                                                               QString::number(
                                                                   _size));
    } else {
        title = QString{tr("Examples for %1 (%2 results)")}.arg(_searchTerm,
                                                                QString::number(
                                                                    _size));
    }
    setWindowTitle(title);
}

void ExampleSplitter::prepareExample(Example &example) const
{
    // Although the setting is named Entry/xxxPronunciationOptions, it applies
    // to all detail views (i.e. both the entry detail view and example detail
    // view.)
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
    example.generatePhonetic(cantoneseOptions, mandarinOptions);
}

void ExampleSplitter::openCurrentSelectionInNewWindow(void)
{
    QModelIndex entryIndex = _resultListView->currentIndex();
    handleDoubleClick(entryIndex);
}

void ExampleSplitter::handleClick(const QModelIndex &selection)
{
    Example example = qvariant_cast<Example>(selection.data());

    prepareExample(example);

    _exampleScrollArea->setExample(example);
}

void ExampleSplitter::handleDoubleClick(const QModelIndex &selection)
{
    Example example = qvariant_cast<Example>(selection.data());

    prepareExample(example);

    ExampleScrollArea *area = new ExampleScrollArea{nullptr};
    area->setParent(this, Qt::Window);
    area->setExample(example);
#ifndef Q_OS_MAC
    area->setWindowTitle(" ");
#endif
    area->show();
}

void ExampleSplitter::updateStyleRequested(void)
{
    static_cast<ExampleResultListView *>(_resultListView)
        ->paintWithApplicationState();
    _exampleScrollArea->updateStyleRequested();
}
