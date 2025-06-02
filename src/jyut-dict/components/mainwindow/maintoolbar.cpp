#include "maintoolbar.h"

#include "logic/search/searchoptionsmediator.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/settings/settings.h"
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QGuiApplication>

MainToolBar::MainToolBar(std::shared_ptr<SQLSearch> sqlSearch,
                         std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                         QWidget *parent) : QToolBar(parent)
{
    setContextMenuPolicy(Qt::PreventContextMenu);

    _searchOptions = std::make_shared<SearchOptionsMediator>();
    _settings = Settings::getSettings(this);

    _searchBar
        = new SearchLineEdit(_searchOptions, sqlSearch, sqlHistoryUtils, this);
    _searchOptions->registerLineEdit(_searchBar);

    _optionsBox = new SearchOptionsRadioGroupBox(_searchOptions,
                                                 sqlSearch,
                                                 this);
    _searchOptions->registerOptionSelector(_optionsBox);

    connect(_searchBar,
            &QLineEdit::textChanged,
            this,
            &MainToolBar::searchBarTextChange);

    setupUI();
}

void MainToolBar::setupUI(void)
{
    _toolBarWidget = new QWidget{this};
    _toolBarLayout = new QGridLayout{_toolBarWidget};
    _toolBarLayout->setContentsMargins(11, 11, 11, 11);
    _toolBarLayout->setSpacing(0);
    _toolBarLayout->setVerticalSpacing(7);

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

    _toolBarLayout->addWidget(_searchBar, 0, 0, 1, 1);
    _toolBarLayout->addItem(new QSpacerItem{25, 0}, 0, 1, 1, 1);
    _toolBarLayout->addWidget(_openHistoryButton, 0, 2, 1, 1);
    _toolBarLayout->addWidget(_openFavouritesButton, 0, 3, 1, 1);
    _toolBarLayout->addWidget(_openSettingsButton, 0, 4, 1, 1);
    _toolBarLayout->addWidget(_optionsBox, 1, 0, 1, -1);
#ifdef Q_OS_WIN
    _toolBarLayout->setContentsMargins(6, 2, 6, 9);
#endif

    addWidget(_toolBarWidget);
    setMovable(false);
    setFloatable(false);
    setFocusPolicy(Qt::ClickFocus);

    setStyle(Utils::isDarkMode());

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [&]() {
        if (!_paletteRecentlyChanged) {
            setStyle(Utils::isDarkMode());
        }
    });

    _searchBar->setFocus();
}

void MainToolBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    QToolBar::changeEvent(event);
}

void MainToolBar::setStyle(bool use_dark)
{
#ifdef Q_OS_MAC
    QColor backgroundColour = use_dark
                                  ? QColor{TITLE_BAR_BACKGROUND_COLOR_DARK_R,
                                           TITLE_BAR_BACKGROUND_COLOR_DARK_G,
                                           TITLE_BAR_BACKGROUND_COLOR_DARK_B}
                                  : QColor{TITLE_BAR_BACKGROUND_COLOR_R,
                                           TITLE_BAR_BACKGROUND_COLOR_G,
                                           TITLE_BAR_BACKGROUND_COLOR_B};
    QColor backgroundColourInactive
        = use_dark ? QColor{TITLE_BAR_INACTIVE_BACKGROUND_COLOR_DARK_R,
                            TITLE_BAR_INACTIVE_BACKGROUND_COLOR_DARK_G,
                            TITLE_BAR_INACTIVE_BACKGROUND_COLOR_DARK_B}
                   : QColor{TITLE_BAR_INACTIVE_BACKGROUND_COLOR_R,
                            TITLE_BAR_INACTIVE_BACKGROUND_COLOR_G,
                            TITLE_BAR_INACTIVE_BACKGROUND_COLOR_B};
    QColor borderColour = use_dark ? QColor{TITLE_BAR_BORDER_COLOR_DARK_R,
                                            TITLE_BAR_BORDER_COLOR_DARK_G,
                                            TITLE_BAR_BORDER_COLOR_DARK_B}
                                   : QColor{TITLE_BAR_BORDER_COLOR_R,
                                            TITLE_BAR_BORDER_COLOR_G,
                                            TITLE_BAR_BORDER_COLOR_B};
    if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
        _inactiveCount++;
        if (_inactiveCount > 2) {
            setStyleSheet(
                QString{"QToolBar { "
                        "   background-color: %1; "
                        "   border-bottom: %2; "
                        "} "}
                    .arg(backgroundColourInactive.name(), borderColour.name()));
        } else {
            setStyleSheet(
                QString{"QToolBar { "
                        "   background-color: %1; "
                        "   border-bottom: %2; "
                        "} "}
                    .arg(backgroundColour.name(), borderColour.name()));
        }
    } else {
        setStyleSheet(QString{"QToolBar { "
                              "   background-color: %1; "
                              "   border-bottom: %2; "
                              "} "}
                          .arg(backgroundColour.name(), borderColour.name()));
    }
#endif

#ifdef Q_OS_WIN
    _openHistoryButton->setIcon(QIcon{use_dark ? ":/images/clock_inverted_nopadding.png"
                                               : ":/images/clock_darkgrey_nopadding.png"});
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
    _openFavouritesButton->setIcon(QIcon{use_dark ? ":/images/star_inverted_nopadding.png"
                                                  : ":/images/star_darkgrey_nopadding.png"});
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
    _openSettingsButton->setIcon(QIcon{use_dark
                                       ? ":/images/settings_inverted_nopadding.png"
                                       : ":/images/settings_darkgrey_nopadding.png"});
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

#ifdef Q_OS_LINUX
    if (Utils::isDarkMode()) {
        setStyleSheet("QToolBar { "
                      "   background-color: palette(alternate-base); "
                      "   border-bottom: 1px solid palette(window); "
                      "}");
    } else {
        setStyleSheet("QToolBar { "
                      "   background: palette(window); "
                      "   border-bottom: 1px solid palette(alternate-base); "
                      "}");
    }
#endif

#ifdef Q_OS_WIN
    if (use_dark) {
        setStyleSheet("QToolBar { "
                      "   background-color: black; "
                      "   border-top: 1px solid black; "
                      "}");
    } else {
        setStyleSheet("QToolBar { "
                      "   background-color: white; "
                      "   border-bottom: 1px solid lightgrey; "
                      "} ");
    }

    Settings::InterfaceSize interfaceSize
        = _settings
              ->value("Interface/size",
                      QVariant::fromValue(Settings::InterfaceSize::NORMAL))
              .value<Settings::InterfaceSize>();
    if (interfaceSize == Settings::InterfaceSize::LARGER) {
        setMinimumHeight(55);
    } else {
        setMinimumHeight(50);
    }
#endif
}

void MainToolBar::focusInEvent(QFocusEvent *event)
{
    QToolBar::focusInEvent(event);
    _searchBar->setFocus();
}

void MainToolBar::selectAllEvent(void) const
{
    _searchBar->setFocus();
    _searchBar->selectAll();
}

void MainToolBar::changeSearchParameters(const SearchParameters params) const
{
    _optionsBox->setOption(params);
}

void MainToolBar::setOpenHistoryAction(QAction *action) const
{
    connect(_openHistoryButton, &QToolButton::pressed, this, [=, this]() {
        action->trigger();
    });
}

void MainToolBar::setOpenSettingsAction(QAction *action) const
{
    connect(_openSettingsButton, &QToolButton::pressed, this, [=, this]() {
        action->trigger();
    });
}

void MainToolBar::setOpenFavouritesAction(QAction *action) const
{
    connect(_openFavouritesButton, &QToolButton::pressed, this, [=, this]() {
        action->trigger();
    });
}

void MainToolBar::forwardSearchHistoryItem(const searchTermHistoryItem &pair) const
{
    _optionsBox->setOption(static_cast<SearchParameters>(pair.second));
    _searchBar->setText(pair.first.c_str());
    _searchBar->search(static_cast<SearchParameters>(pair.second));
}

void MainToolBar::searchQueryRequested(const QString &query,
                                       const SearchParameters &parameters) const
{
    _searchBar->setText(query);
    _optionsBox->setOption(parameters);
}

void MainToolBar::searchRequested(void) const
{
    _searchBar->searchTriggered();
}

void MainToolBar::dictationRequested(void) const
{
#ifdef Q_OS_LINUX
    std::cerr << "MainToolbar::dictationRequested called but is not "
                 "implemented on Linux"
              << std::endl;
#else
    _searchBar->dictationRequested();
#endif
}

void MainToolBar::handwritingRequested(void) const
{
    _searchBar->handwritingRequested();
}

void MainToolBar::updateStyleRequested(void)
{
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_searchBar, &event);
    QCoreApplication::sendEvent(_optionsBox, &event);
    setStyle(Utils::isDarkMode());
}
