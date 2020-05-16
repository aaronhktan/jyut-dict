#include "maintoolbar.h"

#include "logic/search/searchoptionsmediator.h"
#include "logic/utils/utils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

MainToolBar::MainToolBar(std::shared_ptr<SQLSearch> sqlSearch,
                         std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                         QWidget *parent) : QToolBar(parent)
{
    _searchBar = new SearchLineEdit(_searchOptions,
                                    sqlSearch,
                                    sqlHistoryUtils,
                                    this);

    _searchOptions = new SearchOptionsMediator{};

    _searchOptions->registerLineEdit(_searchBar);
    _optionsBox = new SearchOptionsRadioGroupBox(_searchOptions, this);

    setupUI();
}

MainToolBar::~MainToolBar()
{
    delete _searchOptions;
}

void MainToolBar::setupUI(void)
{
    _toolBarWidget = new QWidget;
    _toolBarLayout = new QHBoxLayout;
    _toolBarLayout->setContentsMargins(11, 11, 11, 11);
    _toolBarLayout->setSpacing(0);

    _openHistoryButton = new QToolButton{this};
    _openHistoryButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _openHistoryButton->setIconSize(QSize{15, 15});
    _openHistoryButton->setCursor(Qt::PointingHandCursor);

    _openFavouritesButton = new QToolButton{this};
    _openFavouritesButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _openFavouritesButton->setIconSize(QSize{15, 15});
    _openFavouritesButton->setCursor(Qt::PointingHandCursor);

    _openSettingsButton = new QToolButton{this};
    _openSettingsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _openSettingsButton->setIconSize(QSize{15, 15});
    _openSettingsButton->setCursor(Qt::PointingHandCursor);

    _toolBarLayout->addWidget(_searchBar);
    _toolBarLayout->addWidget(_optionsBox);
    _toolBarLayout->addWidget(_openHistoryButton);
    _toolBarLayout->addWidget(_openFavouritesButton);
    _toolBarLayout->addWidget(_openSettingsButton);
#ifdef Q_OS_WIN
    _toolBarLayout->setContentsMargins(6, 2, 6, 9);
#endif

    _toolBarWidget->setLayout(_toolBarLayout);

    addWidget(_toolBarWidget);
    setMovable(false);
    setFloatable(false);
    setFocusPolicy(Qt::ClickFocus);

#ifdef Q_OS_WIN
    setStyleSheet("QToolBar { background-color: white; }");
#elif defined(Q_OS_LINUX)
    setStyleSheet("QToolBar { border-bottom: 1px solid lightgray; }");
#endif

#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */ false);
#endif
}

void MainToolBar::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    QToolBar::changeEvent(event);
}

void MainToolBar::setStyle(bool use_dark)
{
#ifdef Q_OS_WIN
    _openHistoryButton->setIcon(QIcon{":/images/clock_darkgrey_nopadding.png"});
#else
    _openHistoryButton->setIcon(
        QIcon{use_dark ? ":/images/clock_inverted_nopadding.png"
                       : ":/images/clock_nopadding.png"});
#endif
    _openHistoryButton->setStyleSheet(
        QString{"QToolButton { padding: 3px; margin-right: 6px; }"
                "QToolButton:hover { background-color: %1; border-radius: 3px; "
                "padding: 3px; }"}
            .arg(use_dark ? "grey" : "whitesmoke"));

#ifdef Q_OS_WIN
    _openFavouritesButton->setIcon(QIcon{":/images/star_darkgrey_nopadding.png"});
#else
    _openFavouritesButton->setIcon(
        QIcon{use_dark ? ":/images/star_inverted_nopadding.png"
                       : ":/images/star_nopadding.png"});
#endif
    _openFavouritesButton->setStyleSheet(QString{
        "QToolButton { padding: 3px; margin-top: 2px; margin-bottom: 2px; "
        "margin-right: 4px; }"
        "QToolButton:hover { background-color: %1; border-radius: 3px; "
        "padding: 3px; margin-top: 2px; margin-bottom: 2px; }"}
                                             .arg(use_dark ? "grey"
                                                           : "whitesmoke"));

#ifdef Q_OS_WIN
    _openSettingsButton->setIcon(QIcon{":/images/settings_darkgrey_nopadding.png"});
#else
    _openSettingsButton->setIcon(QIcon{use_dark
                                   ? ":/images/settings_inverted_nopadding.png"
                                   : ":/images/settings_nopadding.png"});
#endif
    _openSettingsButton->setStyleSheet(
        QString{"QToolButton { padding: 3px; margin: 0px; }"
                "QToolButton:hover { background-color: %1; border-radius: 3px; "
                "padding: 3px; margin: 0px; }"}
            .arg(use_dark ? "grey" : "whitesmoke"));
}

void MainToolBar::focusInEvent(QFocusEvent *event)
{
    QToolBar::focusInEvent(event);
    _searchBar->setFocus();
}

void MainToolBar::selectAllEvent(void)
{
    _searchBar->setFocus();
    _searchBar->selectAll();
}

void MainToolBar::changeOptionEvent(const Utils::ButtonOptionIndex index)
{
    _optionsBox->setOption(index);
}

void MainToolBar::setOpenHistoryAction(QAction *action)
{
    connect(_openHistoryButton, &QToolButton::pressed, this, [=]() {
        action->trigger();
    });
}

void MainToolBar::setOpenSettingsAction(QAction *action)
{
    connect(_openSettingsButton, &QToolButton::pressed, this, [=]() {
        action->trigger();
    });
}

void MainToolBar::setOpenFavouritesAction(QAction *action)
{
    connect(_openFavouritesButton, &QToolButton::pressed, this, [=]() {
        action->trigger();
    });
}

void MainToolBar::forwardSearchHistoryItem(searchTermHistoryItem &pair)
{
    _optionsBox->setOption(static_cast<SearchParameters>(pair.second));
    _searchBar->setText(pair.first.c_str());
}
