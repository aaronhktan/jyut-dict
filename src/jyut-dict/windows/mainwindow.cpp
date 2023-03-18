#include "windows/mainwindow.h"

#include "dialogs/noupdatedialog.h"
#include "logic/dictionary/dictionarysource.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#include "windows/updatewindow.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QApplication>
#include <QClipboard>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QColor>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QIcon>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <QtSvg>
#include <QTimer>
#include <QUrl>

#include <memory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // Set window size
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    setMinimumSize(QSize{850, 300});
    resize(850, 600);
#else
    setMinimumSize(QSize{800, 300});
    resize(800, 600);
#endif

    // Instantiate services
    _manager = std::make_shared<SQLDatabaseManager>();
    _sqlSearch = std::make_shared<SQLSearch>(_manager);
    _sqlUserUtils = std::make_shared<SQLUserDataUtils>(_manager);
    _sqlHistoryUtils = std::make_shared<SQLUserHistoryUtils>(_manager);

    // Get colours from QSettings
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    settings->beginReadArray("jyutpingColours");
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::jyutpingToneColours.size();
         ++i) {
        settings->setArrayIndex(static_cast<int>(i));
        QColor color = settings
                           ->value("colour",
                                   QColor{
                                       Settings::jyutpingToneColours[i].c_str()})
                           .value<QColor>();
        Settings::jyutpingToneColours[i] = color.name().toStdString();
    }
    settings->endArray();

    settings->beginReadArray("pinyinColours");
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::pinyinToneColours.size();
         ++i) {
        settings->setArrayIndex(static_cast<int>(i));
        QColor color = settings
                           ->value("colour",
                                   QColor{
                                       Settings::pinyinToneColours[i].c_str()})
                           .value<QColor>();
        Settings::pinyinToneColours[i] = color.name().toStdString();
    }
    settings->endArray();

    // Connect signals to tell the user that database migration has occurred
    _utils = std::make_unique<SQLDatabaseUtils>(_manager);
    connect(_utils.get(),
            &SQLDatabaseUtils::migratingDatabase,
            this,
            &MainWindow::notifyDatabaseMigration);
    connect(_utils.get(),
            &SQLDatabaseUtils::finishedMigratingDatabase,
            this,
            [&](bool success) {
                _databaseMigrationDialog->setLabelText(
                    success ? tr("Database migration finished!")
                            : tr("Database migration failed!"));
                QTimer::singleShot(1000, this, [&] {
                    _databaseMigrationDialog->reset();
                });
            });

    // Populate global source table
    std::vector<std::pair<std::string, std::string>> sources;
    _utils->readSources(sources);
    for (const auto &source : sources) {
        DictionarySourceUtils::addSource(source.first, source.second);
    }

    // Install translator
    installTranslator();

    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());

    // Create UI elements
    _mainToolBar = new MainToolBar{_sqlSearch, _sqlHistoryUtils, this};
    addToolBar(_mainToolBar);
    setUnifiedTitleAndToolBarOnMac(true);
#ifdef APPIMAGE
    setWindowIcon(QIcon{":/images/icon.png"});
#endif
    connect(this,
            &MainWindow::searchHistoryClicked,
            _mainToolBar,
            &MainToolBar::forwardSearchHistoryItem);

    _mainSplitter = new MainSplitter{_sqlUserUtils,
                                     _manager,
                                     _sqlSearch,
                                     _sqlHistoryUtils,
                                     this};
    setCentralWidget(_mainSplitter);
    connect(this,
            &MainWindow::viewHistoryClicked,
            _mainSplitter,
            &MainSplitter::forwardViewHistoryItem);
    connect(_mainToolBar,
            &MainToolBar::searchBarTextChange,
            _mainSplitter,
            &MainSplitter::forwardSearchBarTextChange);
    connect(_mainSplitter,
            &MainSplitter::searchQuery,
            _mainToolBar,
            &MainToolBar::searchQueryRequested);

    // Create menu bar and populate it
    createMenus();
    createActions();
    _mainToolBar->setOpenSettingsAction(_settingsWindowAction);
    _mainToolBar->setOpenFavouritesAction(_favouritesWindowAction);
    _mainToolBar->setOpenHistoryAction(_historyWindowAction);
    connect(this,
            &MainWindow::favouriteCurrentEntry,
            _mainSplitter,
            &MainSplitter::favouriteCurrentEntryRequested);
    connect(this,
            &MainWindow::shareCurrentEntry,
            _mainSplitter,
            &MainSplitter::shareCurrentEntryRequested);
    connect(this,
            &MainWindow::openCurrentEntryInNewWindow,
            _mainSplitter,
            &MainSplitter::openCurrentEntryInNewWindowRequested);
    connect(this,
            &MainWindow::magnifyCurrentEntry,
            _mainSplitter,
            &MainSplitter::magnifyCurrentEntryRequested);
    connect(this,
            &MainWindow::viewAllSentences,
            _mainSplitter,
            &MainSplitter::viewAllSentencesRequested);
    connect(this,
            &MainWindow::searchEntriesBeginning,
            _mainSplitter,
            &MainSplitter::searchEntriesBeginningRequested);
    connect(this,
            &MainWindow::searchEntriesContaining,
            _mainSplitter,
            &MainSplitter::searchEntriesContainingRequested);
    connect(this,
            &MainWindow::searchEntriesEnding,
            _mainSplitter,
            &MainSplitter::searchEntriesEndingRequested);

    // Translate UI
    translateUI();

    // Check for updates
    _checker = new JyutDictionaryReleaseChecker{this};
    if (settings->value("Advanced/updateNotificationsEnabled", QVariant{true}).toBool()) {
        QTimer::singleShot(1000, this, [&]() {
            checkForUpdate(/* showProgress = */ false);
        });
    }

    // Perform database migration if needed, but delay for a bit so that
    // the notify dialog has time to show itself
    QTimer::singleShot(500, this, [&]() {
        std::ignore = QtConcurrent::run(&SQLDatabaseUtils::updateDatabase,
                                        _utils.get());
    });
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    if (event->type() == QEvent::PaletteChange) {
        setStyle(Utils::isDarkMode());
    }
#endif

    QMainWindow::changeEvent(event);
}

void MainWindow::installTranslator(void)
{
    if (!Settings::getSettings()->contains("Advanced/locale")) {
        std::ignore = Settings::systemTranslator.load("qt_" + QLocale::system().name(),
                                                      QLibraryInfo::path(
                                                          QLibraryInfo::TranslationsPath));
        qApp->installTranslator(&Settings::systemTranslator);

        // The reason why we need to loop through UI languages is because
        // if we just use QLocale::system() and the language is zh_Hant_HK,
        // zh_Hant will always be prioritized over zh_HK.

        // By creating a locale with the language being zh_Hant_HK, the correct
        // translation file is loaded.

        // Another example of QLocale::system() failing is having simplified
        // Cantonese as the UI language. QTranslator::load() will try to load
        // zh_Hant_HK instead of yue_Hans.
        for (const auto &language : QLocale::system().uiLanguages()) {
            QLocale locale{language};
            if (Settings::applicationTranslator
                    .load(/* QLocale */ locale,
                          /* filename */ "jyutdictionary",
                          /* prefix */ "-",
                          /* directory */ ":/translations")) {
                qApp->installTranslator(&Settings::applicationTranslator);
                break;
            }
        }
    } else {
        QString localeString
            = Settings::getSettings()->value("Advanced/locale").toString();
        QLocale locale{localeString};

        std::ignore = Settings::systemTranslator.load("qt_" + locale.name(),
                                                      QLibraryInfo::path(
                                                          QLibraryInfo::TranslationsPath));
        qApp->installTranslator(&Settings::systemTranslator);

        std::ignore = Settings::applicationTranslator.load(/* QLocale */ locale,
                                                           /* filename */ "jyutdictionary",
                                                           /* prefix */ "-",
                                                           /* directory */ ":/translations");
        qApp->installTranslator(&Settings::applicationTranslator);

        Settings::setCurrentLocale(locale);
    }
}

void MainWindow::translateUI(void)
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    setWindowTitle(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                               Strings::PRODUCT_NAME));

    _fileMenu->setTitle(tr("&File"));
    _editMenu->setTitle(tr("&Edit"));
    _searchMenu->setTitle(tr("&Search"));
    _entryMenu->setTitle(tr("E&ntry"));
    _windowMenu->setTitle(tr("&Window"));
    _helpMenu->setTitle(tr("&Help"));

    _aboutAction->setText(tr("&About"));
    _aboutAction->setStatusTip(tr("Show the application's About box"));

#ifdef Q_OS_MAC
    _settingsWindowAction->setText(tr("Preferences"));
#else
    _settingsWindowAction->setText(tr("Settings"));
#endif
    _settingsWindowAction->setStatusTip(tr("Change settings"));

    _closeWindowAction->setText(tr("Close Window"));

    _undoAction->setText(tr("Undo"));
    _redoAction->setText(tr("Redo"));
    _cutAction->setText(tr("Cut"));
    _copyAction->setText(tr("Copy"));
    _pasteAction->setText(tr("Paste"));

    _findAction->setText(tr("Start a New Search"));
    _findAndSelectAllAction->setText(tr("Select Search Bar Contents"));
    _setFocusToResultsAction->setText(
        tr("Jump to First Item in Search Results"));
    _selectSimplifiedAction->setText(tr("Search Simplified Chinese"));
    _selectTraditionalAction->setText(tr("Search Traditional Chinese"));
    _selectJyutpingAction->setText(tr("Search Jyutping"));
    _selectPinyinAction->setText(tr("Search Pinyin"));
    _selectEnglishAction->setText(tr("Search English"));

    _favouriteCurrentEntryAction->setText(tr("Save or Unsave Current Entry"));
    _shareCurrentEntryAction->setText(tr("Share Current Entry..."));
    _openCurrentEntryInNewWindowAction->setText(
        tr("Open Current Entry in New Window..."));
    _magnifyCurrentEntryAction->setText(
        tr("View Large Version of Current Entry..."));
    _viewAllSentencesAction->setText(
        tr("View All Sentences for Current Entry..."));
    _searchWordsBeginningAction->setText(
        tr("Find Entries That Begin With Current Entry"));
    _searchWordsContainingAction->setText(
        tr("Find Entries That Contain Current Entry"));
    _searchWordsEndingAction->setText(
        tr("Find Entries That End With Current Entry"));

    _historyWindowAction->setText(tr("View Search History"));
    _favouritesWindowAction->setText(tr("Open List of Saved Words"));
    _minimizeAction->setText(tr("Minimize"));
    _maximizeAction->setText(tr("Zoom"));
    _bringAllToFrontAction->setText(tr("Bring All to Front"));

    _helpAction->setText(tr("%1 Help").arg(
        QCoreApplication::translate("strings", Strings::PRODUCT_NAME)));
    _updateAction->setText(tr("Check for Updates..."));

    Utils::refreshLanguageMap();

#ifdef Q_OS_WIN
    QFont font;
    if (Settings::isCurrentLocaleTraditionalHan()) {
        font = QFont{"Microsoft Jhenghei", 10};
    } else if (Settings::isCurrentLocaleSimplifiedHan()) {
        font = QFont{"Microsoft YaHei", 10};
    } else if (Settings::isCurrentLocaleHan()) {
        font = QFont{"Microsoft YaHei", 10};
    } else {
        font = QFont{"Segoe UI", 10};
    }
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    qApp->setFont(font);
    foreach (QWidget *widget, QApplication::allWidgets()) {
        widget->setFont(font);
    }
#endif
}

void MainWindow::setStyle(bool use_dark)
{
#ifdef Q_OS_MAC
    (void) (use_dark);

    setStyleSheet(
        "QPushButton[isHan=\"true\"] { font-size: 12px; height: 16px; }");
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    if (!use_dark) {
        QPalette palette = QApplication::style()->standardPalette();
        palette.setColor(QPalette::Window, QColor{CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                                  CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                                  CONTENT_BACKGROUND_COLOUR_LIGHT_B});
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::AlternateBase, QColor{HEADER_BACKGROUND_COLOUR_LIGHT_R,
                                                         HEADER_BACKGROUND_COLOUR_LIGHT_G,
                                                         HEADER_BACKGROUND_COLOUR_LIGHT_B});
        palette.setColor(QPalette::Highlight,
                         QColor{LIST_ITEM_ACTIVE_COLOUR_LIGHT_R,
                                LIST_ITEM_ACTIVE_COLOUR_LIGHT_G,
                                LIST_ITEM_ACTIVE_COLOUR_LIGHT_B});
        palette.setColor(QPalette::Inactive, QPalette::Highlight,
                         QColor{LIST_ITEM_INACTIVE_COLOUR_LIGHT_R,
                                LIST_ITEM_INACTIVE_COLOUR_LIGHT_G,
                                LIST_ITEM_INACTIVE_COLOUR_LIGHT_B});
        qApp->setPalette(palette);
        qApp->setStyleSheet("");
    } else {
        QColor darkGray{53, 53, 53};
        QColor gray{128, 128, 128};

        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor{CONTENT_BACKGROUND_COLOUR_DARK_R,
                                                      CONTENT_BACKGROUND_COLOUR_DARK_G,
                                                      CONTENT_BACKGROUND_COLOUR_DARK_B});
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor{BACKGROUND_COLOUR_DARK_R,
                                                    BACKGROUND_COLOUR_DARK_G,
                                                    BACKGROUND_COLOUR_DARK_B});
        darkPalette.setColor(QPalette::AlternateBase, QColor{HEADER_BACKGROUND_COLOUR_DARK_R,
                                                             HEADER_BACKGROUND_COLOUR_DARK_G,
                                                             HEADER_BACKGROUND_COLOUR_DARK_B});
        darkPalette.setColor(QPalette::ToolTipBase, darkGray);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, darkGray);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Link, Qt::blue);
        darkPalette.setColor(QPalette::Highlight,
                             QColor{LIST_ITEM_ACTIVE_COLOUR_DARK_R,
                                    LIST_ITEM_ACTIVE_COLOUR_DARK_G,
                                    LIST_ITEM_ACTIVE_COLOUR_DARK_B});
        darkPalette.setColor(QPalette::HighlightedText, Qt::white);

        darkPalette.setColor(QPalette::Active, QPalette::Button, gray.darker());
        darkPalette.setColor(QPalette::Inactive,
                             QPalette::Highlight,
                             QColor{LIST_ITEM_INACTIVE_COLOUR_DARK_R,
                                    LIST_ITEM_INACTIVE_COLOUR_DARK_G,
                                    LIST_ITEM_INACTIVE_COLOUR_DARK_B});
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, gray);
        darkPalette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

        qApp->setPalette(darkPalette);
        // For some reason, if I don't set this, text in dark mode paints as black
        // (even for all widgets that are not tooltips)
        qApp->setStyleSheet("QToolTip { color: #ffffff; border: 1px solid white; }");
    }
#endif

#ifdef Q_OS_LINUX
    if (use_dark) {
        menuBar()->setStyleSheet("QMenuBar { "
                                 "   background-color: palette(alternate-base); "
                                 "   border-bottom: 1px solid palette(window);"
                                 "} ");
        qApp->setStyleSheet(
            "QCheckBox::indicator { "
            "   height: 20px; "
            "   width: 20px; "
            "} "
            " "
            // The SVG files don't work in dark mode (Qt renders
            // them very dark for some reason).
            "QCheckBox::indicator::checked { "
            "   image: url(:/images/check_box_checked_inverted.png); "
            "} "
            " "
            "QCheckBox::indicator::unchecked { "
            "   image: url(:/images/check_box_unchecked_inverted.png); "
            "} "
            " "
            "QGroupBox { "
            "   border: 1px solid palette(alternate-base); "
            "} "
            " "
            "QListView { "
            "   background-color: palette(base); "
            "} "
            " "
            "QRadioButton::indicator { "
            "   height: 18px; "
            "   width: 18px; "
            "} "
            " "
            // The SVG files don't work in dark mode (Qt renders
            // them very dark for some reason).
            "QRadioButton::indicator::checked { "
            "   image: url(:/images/radio_button_checked_inverted.png); "
            "} "
            " "
            "QRadioButton::indicator::unchecked { "
            "   image: url(:/images/radio_button_unchecked_inverted.png); "
            "} ");
    } else {
        menuBar()->setStyleSheet("QMenuBar { "
                                 "   background-color: palette(window); "
                                 "   border-bottom: 1px solid palette(alternate-base);"
                                 "} ");
        qApp->setStyleSheet(
            "QCheckBox::indicator { "
            "   height: 20px; "
            "   width: 20px; "
            "} "
            " "
            "QCheckBox::indicator::checked { "
            "   image: url(:/images/check_box_checked.svg); "
            "} "
            " "
            "QCheckBox::indicator::unchecked { "
            "   image: url(:/images/check_box_unchecked.svg); "
            "} "
            " "
            "QGroupBox { "
            "   border: 1px solid palette(alternate-base); "
            "} "
            " "
            "QListView { "
            "   background-color: palette(base); "
            "} "
            " "
            "QRadioButton::indicator { "
            "   height: 18px; "
            "   width: 18px; "
            "} "
            " "
            "QRadioButton::indicator::unchecked { "
            "   image: url(:/images/radio_button_unchecked.svg); "
            "} "
            " "
            "QRadioButton::indicator::checked { "
            "   image: url(:/images/radio_button_checked.svg); "
            "} ");
    }
#elif defined(Q_OS_WIN)
    // Some additional stylesheet overrides for Windows
    if (use_dark) {
        menuBar()->setStyleSheet("QMenuBar { "
                                 "   background-color: black; "
                                 "} "
                                 ""
                                 "QMenuBar::item:selected { "
                                 "   background-color: palette(highlight); "
                                 "   border-left: 1px solid black; "
                                 "   border-right: 1px solid black; "
                                 "} ");
        qApp->setStyleSheet(
            "QCheckBox::indicator { "
            "   height: 20px; "
            "   width: 20px; "
            "} "
            ""
            // The SVG files don't work in dark mode (Qt renders
            // them very dark for some reason).
            "QCheckBox::indicator::checked { "
            "   image: url(:/images/check_box_checked_inverted.png); "
            "} "
            ""
            "QCheckBox::indicator::unchecked { "
            "   image: url(:/images/check_box_unchecked_inverted.png); "
            "} "
            ""
            "QComboBox { "
            "   background-color: palette(window); "
            "   border: 1px solid palette(alternate-base); "
            "   padding: 3px; "
            "} "
            "QComboBox::drop-down { "
            "   width: 30px; "
            "   border: none; "
            "} "
            ""
            "QComboBox QAbstractItemView { "
            "   background-color: palette(window); "
            "   border: 1px solid palette(alternate-base); "
            "   selection-background-color: palette(highlight); "
            "   outline: 0px; "
            "} "
            ""
            "QComboBox::down-arrow { "
            "   image: url(:/images/chevron_down_inverted.svg); "
            "} "
            ""
            "QGroupBox { "
            "   border: 1px solid palette(alternate-base); "
            "} "
            ""
            "QListView { "
            "   background-color: palette(base); "
            "} "
            ""
            "QMenu { "
            "   background-color: palette(window); "
            "   border: 1px solid transparent; "
            "} "
            ""
            "QMenu::item { "
            "   padding: 3px 25px 3px 25px; "
            "   border: 1px solid transparent; "
            "} "
            ""
            "QMenu::item:selected { "
            "   background-color: palette(highlight); "
            "   padding: 3px 25px 3px 25px ;"
            "   border: 1px solid transparent; "
            "} "
            ""
            "QMenu::separator { "
            "   height: 1px; "
            "   background-color: palette(alternate-base); "
            "} "
            ""
            "QMenu::indicator { "
            "   width: 18px; "
            "   height: 18px; "
            "} "
            ""
            "QMessageBox { "
            "   background-color: palette(base); "
            "   border-top: 1px solid palette(alternate-base); "
            "} "
            ""
            "QPushButton { "
            "   background-color: palette(window); "
            "   border: 1px solid palette(alternate-base); "
            "   padding: 5px; "
            "} "
            ""
            "QPushButton:pressed { "
            "   background-color: palette(highlight);"
            "   border: none; "
            "} "
            ""
            "QRadioButton::indicator { "
            "   height: 18px; "
            "   width: 18px; "
            "} "
            ""
            // The SVG files don't work in dark mode (Qt renders
            // them very dark for some reason).
            "QRadioButton::indicator::checked { "
            "   image: url(:/images/radio_button_checked_inverted.png); "
            "} "
            ""
            "QRadioButton::indicator::unchecked { "
            "   image: url(:/images/radio_button_unchecked_inverted.png); "
            "} "
            ""
            "QScrollBar:vertical { "
            "   background-color: palette(window); "
            "   width: 16px; "
            "   margin: 0px; "
            "} "
            ""
            "QScrollBar::handle:vertical { "
            "   background-color: dimgrey; "
            "   min-height: 20px; "
            "   margin-top: 15px; "
            "   margin-bottom: 15px; "
            "}"
            ""
            "QScrollBar::handle:vertical:hover { "
            "   background-color: darkgrey; "
            "} "
            ""
            "QScrollBar::add-line:vertical { "
            "   background: none; "
            "   height: 15px; "
            "   subcontrol-position: bottom; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::sub-line:vertical{ "
            "   background: none; "
            "   height: 15px;   "
            "   subcontrol-position: top; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::add-page:vertical { "
            "   background: none; "
            "   height: 0px; "
            "   subcontrol-position: right; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::sub-page:vertical { "
            "   background: none; "
            "   height: 0px; "
            "   subcontrol-position: left; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::up-arrow:vertical { "
            "   border: 0px solid black; "
            "   width: 10px; "
            "   height: 10px; "
            "   image: url(:/images/chevron_up_inverted.svg); "
            "} "
            ""
            "QScrollBar::down-arrow:vertical { "
            "   border: 0px solid black; "
            "   width: 10px; "
            "   height: 10px; "
            "   image: url(:/images/chevron_down_inverted.svg); "
            "} "
            ""
            "QToolTip { "
            "   color: palette(text); "
            "   background-color: palette(window); "
            "   border: 1px solid palette(alternate-base); "
            "} ");
    } else {
        menuBar()->setStyleSheet("QMenuBar { \
                                    background-color: white; \
                                 }");
        qApp->setStyleSheet(
            "QCheckBox::indicator { "
            "   height: 20px; "
            "   width: 20px; "
            "} "
            ""
            "QCheckBox::indicator::checked { "
            "   image: url(:/images/check_box_checked.svg); "
            "} "
            ""
            "QCheckBox::indicator::unchecked { "
            "   image: url(:/images/check_box_unchecked.svg); "
            "} "
            ""
            "QRadioButton::indicator { "
            "   height: 18px; "
            "   width: 18px; "
            "} "
            ""
            "QRadioButton::indicator::unchecked { "
            "   image: url(:/images/radio_button_unchecked.svg); "
            "} "
            ""
            "QRadioButton::indicator::checked { "
            "   image: url(:/images/radio_button_checked.svg); "
            "} "
            ""
            "QScrollBar:vertical { "
            "   background-color: palette(window); "
            "   width: 16px; "
            "   margin: 0px; "
            "} "
            ""
            "QScrollBar::handle:vertical { "
            "   background-color: lightgrey; "
            "   min-height: 20px; "
            "   margin-top: 15px; "
            "   margin-bottom: 15px; "
            "}"
            ""
            "QScrollBar::handle:vertical:hover { "
            "   background-color: darkgrey; "
            "} "
            ""
            "QScrollBar::add-line:vertical { "
            "   background: none; "
            "   height: 15px; "
            "   subcontrol-position: bottom; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::sub-line:vertical{ "
            "   background: none; "
            "   height: 15px;   "
            "   subcontrol-position: top; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::add-page:vertical { "
            "   background: none; "
            "   height: 0px; "
            "   subcontrol-position: right; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::sub-page:vertical { "
            "   background: none; "
            "   height: 0px; "
            "   subcontrol-position: left; "
            "   subcontrol-origin: margin; "
            "} "
            ""
            "QScrollBar::up-arrow:vertical { "
            "   border: 0px solid black; "
            "   width: 10px; "
            "   height: 10px; "
            "   image: url(:/images/chevron_up_inverted.svg); "
            "} "
            ""
            "QScrollBar::down-arrow:vertical { "
            "   border: 0px solid black; "
            "   width: 10px; "
            "   height: 10px; "
            "   image: url(:/images/chevron_down_inverted.svg); "
            "} ");
    }
#endif
}

void MainWindow::notifyUpdateAvailable(bool updateAvailable,
                                       std::string versionNumber,
                                       std::string url, std::string description,
                                       bool showIfNoUpdate)
{
    if (updateAvailable) {
        UpdateAvailableWindow *window = new UpdateAvailableWindow{this, versionNumber, url, description};
        window->show();
    } else if (showIfNoUpdate) {
        QString currentVersion = QString{Utils::CURRENT_VERSION};
        NoUpdateDialog *_message = new NoUpdateDialog{currentVersion, this};
        _message->exec();
    }
}

void MainWindow::forwardSearchHistoryItem(const SearchTermHistoryItem &pair)
{
    emit searchHistoryClicked(pair);
}

void MainWindow::forwardViewHistoryItem(const Entry &entry)
{
    emit viewHistoryClicked(entry);
}

void MainWindow::createMenus(void)
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _editMenu = menuBar()->addMenu(tr("&Edit"));
    _searchMenu = menuBar()->addMenu(tr("&Search"));
    _entryMenu = menuBar()->addMenu(tr("E&ntry"));
    _windowMenu = menuBar()->addMenu(tr("&Window"));
    _helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::createActions(void)
{
    _aboutAction = new QAction{this};
    _aboutAction->setMenuRole(QAction::AboutRole);
    connect(_aboutAction,
            &QAction::triggered,
            this,
            &MainWindow::openAboutWindow);
    _fileMenu->addAction(_aboutAction);

    _settingsWindowAction = new QAction{this};
    _settingsWindowAction->setStatusTip(tr("Change settings"));
    _settingsWindowAction->setShortcut(QKeySequence{"Ctrl+,"});
    _settingsWindowAction->setMenuRole(QAction::PreferencesRole);
    connect(_settingsWindowAction,
            &QAction::triggered,
            this,
            &MainWindow::openSettingsWindow);
    _fileMenu->addAction(_settingsWindowAction);

    _closeWindowAction = new QAction{this};
    _closeWindowAction->setShortcut(QKeySequence{"Ctrl+W"});
    connect(_closeWindowAction, &QAction::triggered, this, &QWidget::close);
    _fileMenu->addAction(_closeWindowAction);

    _undoAction = new QAction{this};
    _undoAction->setShortcut(QKeySequence{"Ctrl+Z"});
    connect(_undoAction, &QAction::triggered, this, &MainWindow::undo);
    _editMenu->addAction(_undoAction);

    _redoAction = new QAction{this};
    _redoAction->setShortcut(QKeySequence{"Ctrl+Shift+Z"});
    connect(_redoAction, &QAction::triggered, this, &MainWindow::redo);
    _editMenu->addAction(_redoAction);

    _editMenu->addSeparator();

    _cutAction = new QAction{this};
    _cutAction->setShortcut(QKeySequence{"Ctrl+X"});
    connect(_cutAction, &QAction::triggered, this, &MainWindow::cut);
    _editMenu->addAction(_cutAction);

    _copyAction = new QAction{this};
    _copyAction->setShortcut(QKeySequence{"Ctrl+C"});
    connect(_copyAction, &QAction::triggered, this, &MainWindow::copy);
    _editMenu->addAction(_copyAction);

    _pasteAction = new QAction{this};
    _pasteAction->setShortcut(QKeySequence{"Ctrl+V"});
    connect(_pasteAction, &QAction::triggered, this, &MainWindow::paste);
    _editMenu->addAction(_pasteAction);

    _editMenu->addSeparator();

    _findAction = new QAction{this};
    _findAction->setShortcut(QKeySequence{"Ctrl+F"});
    connect(_findAction, &QAction::triggered, this, &MainWindow::find);
    _searchMenu->addAction(_findAction);

    _findAndSelectAllAction = new QAction{this};
    _findAndSelectAllAction->setShortcut(QKeySequence{"Ctrl+Shift+F"});
    connect(_findAndSelectAllAction,
            &QAction::triggered,
            this,
            &MainWindow::findAndSelectAll);
    _searchMenu->addAction(_findAndSelectAllAction);

    _searchMenu->addSeparator();

    _setFocusToResultsAction = new QAction{this};
    _setFocusToResultsAction->setShortcut(QKeySequence{"Ctrl+L"});
    connect(_setFocusToResultsAction,
            &QAction::triggered,
            this,
            &MainWindow::setFocusToResults);
    _searchMenu->addAction(_setFocusToResultsAction);

    _searchMenu->addSeparator();

    _selectSimplifiedAction = new QAction{this};
    _selectSimplifiedAction->setShortcut(QKeySequence{"Ctrl+1"});
    connect(_selectSimplifiedAction,
            &QAction::triggered,
            this,
            &MainWindow::selectSimplified);
    _searchMenu->addAction(_selectSimplifiedAction);

    _selectTraditionalAction = new QAction{this};
    _selectTraditionalAction->setShortcut(QKeySequence{"Ctrl+2"});
    connect(_selectTraditionalAction,
            &QAction::triggered,
            this,
            &MainWindow::selectTraditional);
    _searchMenu->addAction(_selectTraditionalAction);

    _selectJyutpingAction = new QAction{this};
    _selectJyutpingAction->setShortcut(QKeySequence{"Ctrl+3"});
    connect(_selectJyutpingAction,
            &QAction::triggered,
            this,
            &MainWindow::selectJyutping);
    _searchMenu->addAction(_selectJyutpingAction);

    _selectPinyinAction = new QAction{this};
    _selectPinyinAction->setShortcut(QKeySequence{"Ctrl+4"});
    connect(_selectPinyinAction,
            &QAction::triggered,
            this,
            &MainWindow::selectPinyin);
    _searchMenu->addAction(_selectPinyinAction);

    _selectEnglishAction = new QAction{this};
    _selectEnglishAction->setShortcut(QKeySequence{"Ctrl+5"});
    connect(_selectEnglishAction,
            &QAction::triggered,
            this,
            &MainWindow::selectEnglish);
    _searchMenu->addAction(_selectEnglishAction);

    _favouriteCurrentEntryAction = new QAction{this};
    _favouriteCurrentEntryAction->setShortcut(QKeySequence{"Ctrl+S"});
    connect(_favouriteCurrentEntryAction, &QAction::triggered, this, [&]() {
        emit favouriteCurrentEntry();
    });
    _entryMenu->addAction(_favouriteCurrentEntryAction);

    _shareCurrentEntryAction = new QAction{this};
    _shareCurrentEntryAction->setShortcut(QKeySequence{"Ctrl+P"});
    connect(_shareCurrentEntryAction, &QAction::triggered, this, [&]() {
        emit shareCurrentEntry();
    });
    _entryMenu->addAction(_shareCurrentEntryAction);

    _openCurrentEntryInNewWindowAction = new QAction{this};
    _openCurrentEntryInNewWindowAction->setShortcut(QKeySequence{"Ctrl+N"});
    connect(_openCurrentEntryInNewWindowAction,
            &QAction::triggered,
            this,
            [&]() { emit openCurrentEntryInNewWindow(); });
    _entryMenu->addAction(_openCurrentEntryInNewWindowAction);

    _magnifyCurrentEntryAction = new QAction{this};
    _magnifyCurrentEntryAction->setShortcut(QKeySequence{"Ctrl+G"});
    connect(_magnifyCurrentEntryAction, &QAction::triggered, this, [&]() {
        emit magnifyCurrentEntry();
    });
    _entryMenu->addAction(_magnifyCurrentEntryAction);

    _entryMenu->addSeparator();

    _viewAllSentencesAction = new QAction{this};
    _viewAllSentencesAction->setShortcut(QKeySequence{"Ctrl+T"});
    connect(_viewAllSentencesAction, &QAction::triggered, this, [&]() {
        emit viewAllSentences();
    });
    _entryMenu->addAction(_viewAllSentencesAction);

    _entryMenu->addSeparator();

    _searchWordsBeginningAction = new QAction{this};
    _searchWordsBeginningAction->setShortcut(QKeySequence{"Ctrl+U"});
    connect(_searchWordsBeginningAction, &QAction::triggered, this, [&]() {
        emit searchEntriesBeginning();
    });
    _entryMenu->addAction(_searchWordsBeginningAction);

    _searchWordsContainingAction = new QAction{this};
    _searchWordsContainingAction->setShortcut(QKeySequence{"Ctrl+I"});
    connect(_searchWordsContainingAction, &QAction::triggered, this, [&]() {
        emit searchEntriesContaining();
    });
    _entryMenu->addAction(_searchWordsContainingAction);

    _searchWordsEndingAction = new QAction{this};
    _searchWordsEndingAction->setShortcut(QKeySequence{"Ctrl+O"});
    connect(_searchWordsEndingAction, &QAction::triggered, this, [&]() {
        emit searchEntriesEnding();
    });
    _entryMenu->addAction(_searchWordsEndingAction);

    _historyWindowAction = new QAction{this};
    _historyWindowAction->setShortcut(QKeySequence{"Ctrl+Shift+H"});
    connect(_historyWindowAction,
            &QAction::triggered,
            this,
            &MainWindow::openHistoryWindow);
    _windowMenu->addAction(_historyWindowAction);

    _favouritesWindowAction = new QAction{this};
    _favouritesWindowAction->setShortcut(QKeySequence{"Ctrl+Shift+S"});
    connect(_favouritesWindowAction,
            &QAction::triggered,
            this,
            &MainWindow::openFavouritesWindow);
    _windowMenu->addAction(_favouritesWindowAction);

    _windowMenu->addSeparator();

    _minimizeAction = new QAction{this};
    _minimizeAction->setShortcut(QKeySequence{"Ctrl+M"});
    connect(_minimizeAction,
            &QAction::triggered,
            this,
            &MainWindow::toggleMinimized);
    _windowMenu->addAction(_minimizeAction);

    _maximizeAction = new QAction{this};
    connect(_maximizeAction,
            &QAction::triggered,
            this,
            &MainWindow::toggleMaximized);
    _windowMenu->addAction(_maximizeAction);

    _windowMenu->addSeparator();

    _bringAllToFrontAction = new QAction{this};
    connect(_bringAllToFrontAction,
            &QAction::triggered,
            this,
            &QWidget::showNormal);
    _windowMenu->addAction(_bringAllToFrontAction);

    _helpAction = new QAction{this};
    connect(_helpAction, &QAction::triggered, this, []() {
        QDesktopServices::openUrl(QUrl{Utils::GITHUB_LINK});
    });
    _helpMenu->addAction(_helpAction);

    _updateAction = new QAction{this};
    connect(_updateAction, &QAction::triggered, [&]() {
        checkForUpdate(/* showProgress = */ true);
    });
    _helpMenu->addAction(_updateAction);
}

void MainWindow::undo(void) const
{
    QWidget *focused = QApplication::focusWidget();
    if (focused) {
        QApplication::postEvent(focused,
                                new QKeyEvent{QEvent::KeyPress,
                                              Qt::Key_Z,
                                              Qt::ControlModifier});
        QApplication::postEvent(focused,
                                new QKeyEvent{QEvent::KeyRelease,
                                              Qt::Key_Z,
                                              Qt::ControlModifier});
    }
}

void MainWindow::redo(void) const
{
    QWidget *focused = QApplication::focusWidget();
    if (focused) {
        QApplication::postEvent(focused,
                                new QKeyEvent{QEvent::KeyPress,
                                              Qt::Key_Z,
                                              Qt::ControlModifier |
                                              Qt::ShiftModifier});
        QApplication::postEvent(focused,
                                new QKeyEvent{QEvent::KeyRelease,
                                              Qt::Key_Z,
                                              Qt::ControlModifier |
                                              Qt::ShiftModifier});
    }
}

void MainWindow::cut(void) const
{
    QWidget *focused = QApplication::focusWidget();
    if (focused) {
        QApplication::postEvent(focused,
                                new QKeyEvent{QEvent::KeyPress,
                                              Qt::Key_X,
                                              Qt::ControlModifier});
        QApplication::postEvent(focused,
                                new QKeyEvent{QEvent::KeyRelease,
                                              Qt::Key_X,
                                              Qt::ControlModifier});
    }
}

void MainWindow::copy(void) const
{
    QWidget *focused = QApplication::focusWidget();
    if (!focused) {
        return;
    }

    if (typeid(*focused) == typeid(QLabel)) {
        QGuiApplication::clipboard()->setText(
                    static_cast<QLabel *>(focused)->selectedText());
        return;
    }

    QApplication::postEvent(focused,
                            new QKeyEvent{QEvent::KeyPress,
                                          Qt::Key_C,
                                          Qt::ControlModifier});
    QApplication::postEvent(focused,
                            new QKeyEvent{QEvent::KeyRelease,
                                          Qt::Key_C,
                                          Qt::ControlModifier});
}

void MainWindow::paste(void) const
{
    QWidget *focused = QApplication::focusWidget();
    if (focused) {
        QApplication::postEvent(focused,
                                new QKeyEvent(QEvent::KeyPress,
                                              Qt::Key_V,
                                              Qt::ControlModifier));
        QApplication::postEvent(focused,
                                new QKeyEvent(QEvent::KeyRelease,
                                              Qt::Key_V,
                                              Qt::ControlModifier));
    }
}

void MainWindow::find(void) const
{
    _mainToolBar->setFocus();
}

void MainWindow::findAndSelectAll(void) const
{
    _mainToolBar->selectAllEvent();
}

void MainWindow::setFocusToResults(void) const
{
    _mainSplitter->setFocusToResults();
}

void MainWindow::selectSimplified(void) const
{
    _mainToolBar->changeOptionEvent(Utils::SIMPLIFIED_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectTraditional(void) const
{
    _mainToolBar->changeOptionEvent(Utils::TRADITIONAL_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectJyutping(void) const
{
    _mainToolBar->changeOptionEvent(Utils::JYUTPING_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectPinyin(void) const
{
    _mainToolBar->changeOptionEvent(Utils::PINYIN_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectEnglish(void) const
{
    _mainToolBar->changeOptionEvent(Utils::ENGLISH_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::toggleMinimized(void)
{
    if (!isMinimized()) {
        showMinimized();
    } else {
        showNormal();
    }
}

void MainWindow::toggleMaximized(void)
{
    if (!isMaximized()) {
        showMaximized();
    } else {
        showNormal();
    }
}

void MainWindow::openAboutWindow(void)
{
    if (_aboutWindow) {
        _aboutWindow->activateWindow();
        _aboutWindow->raise();
        return;
    }

    _aboutWindow = new AboutWindow{this};
    _aboutWindow->show();
}

void MainWindow::openSettingsWindow(void)
{
    if (_settingsWindow) {
        _settingsWindow->activateWindow();
        _settingsWindow->raise();
        return;
    }

    _settingsWindow = new SettingsWindow{_manager, this};
    _settingsWindow->show();

    connect(_settingsWindow,
            &SettingsWindow::updateStyle,
            this,
            &MainWindow::updateStyleRequested);
}

void MainWindow::openHistoryWindow(void)
{
    if (_historyWindow) {
        _historyWindow->activateWindow();
        _historyWindow->raise();
        return;
    }

    _historyWindow = new HistoryWindow{_sqlHistoryUtils, nullptr};
    _historyWindow->setParent(this, Qt::Window);
    _historyWindow->setAttribute(Qt::WA_DeleteOnClose);
    _historyWindow->move(x() + width(), y());
    _historyWindow->show();

    connect(static_cast<HistoryWindow *>(_historyWindow),
            &HistoryWindow::searchHistoryClicked,
            this,
            &MainWindow::forwardSearchHistoryItem);

    connect(static_cast<HistoryWindow *>(_historyWindow),
            &HistoryWindow::viewHistoryClicked,
            this,
            &MainWindow::forwardViewHistoryItem);
}

void MainWindow::openFavouritesWindow(void)
{
    if (_favouritesWindow) {
        _favouritesWindow->activateWindow();
        _favouritesWindow->raise();
        return;
    }

    _favouritesWindow = new FavouriteSplitter{_sqlUserUtils, _manager, nullptr};
    _favouritesWindow->setParent(this, Qt::Window);
    _favouritesWindow->setAttribute(Qt::WA_DeleteOnClose);
    _favouritesWindow->show();
    _favouritesWindow
        ->move(x() + (width() - _favouritesWindow->size().width()) / 2,
               y() + (height() - _favouritesWindow->size().height()) / 2);
    _favouritesWindow->setFocus();

    connect(_favouritesWindow,
            &FavouriteSplitter::searchQuery,
            _mainToolBar,
            &MainToolBar::searchQueryRequested);
}

void MainWindow::checkForUpdate(bool showProgress)
{
    disconnect(_checker, nullptr, nullptr, nullptr);
    if (showProgress) {
        connect(_checker,
                &JyutDictionaryReleaseChecker::foundUpdate,
                this,
                [&](bool updateAvailable,
                    std::string versionNumber,
                    std::string url,
                    std::string description) {
                    _updateDialog->reset();

                    disconnect(_checker, nullptr, nullptr, nullptr);
                    notifyUpdateAvailable(updateAvailable,
                                          versionNumber,
                                          url,
                                          description,
                                          /* showIfNoUpdate = */ true);

                    _recentlyCheckedForUpdates = false;
                });

        _updateDialog = new QProgressDialog{"", QString(), 0, 0, this};
        _updateDialog->setWindowModality(Qt::ApplicationModal);
        _updateDialog->setMinimumSize(300, 75);
        Qt::WindowFlags flags = _updateDialog->windowFlags()
                                | Qt::CustomizeWindowHint;
        flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
                   | Qt::WindowFullscreenButtonHint
                   | Qt::WindowContextHelpButtonHint);
        _updateDialog->setWindowFlags(flags);
        _updateDialog->setMinimumDuration(0);
#ifdef Q_OS_WIN
        _updateDialog->setWindowTitle(
            QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
        _updateDialog->setWindowTitle(" ");
#endif
        _updateDialog->setAttribute(Qt::WA_DeleteOnClose, true);

        _updateDialog->setLabelText(tr("Checking for update..."));
        _updateDialog->setRange(0, 0);
        _updateDialog->setValue(0);
    } else {
        connect(_checker,
                &JyutDictionaryReleaseChecker::foundUpdate,
                this,
                [&](bool updateAvailable,
                    std::string versionNumber,
                    std::string url,
                    std::string description) {
                    disconnect(_checker, nullptr, nullptr, nullptr);
                    notifyUpdateAvailable(updateAvailable,
                                          versionNumber,
                                          url,
                                          description,
                                          /* showIfNoUpdateo = */ false);

                    _recentlyCheckedForUpdates = false;
                });
    }

    if (!_recentlyCheckedForUpdates) {
        _checker->checkForNewUpdate();
        _recentlyCheckedForUpdates = true;
    }
}

void MainWindow::notifyDatabaseMigration(void)
{
    _databaseMigrationDialog = new QProgressDialog{"", QString(), 0, 0, this};
    _databaseMigrationDialog->setWindowModality(Qt::ApplicationModal);
    _databaseMigrationDialog->setMinimumSize(300, 75);
    Qt::WindowFlags flags = _databaseMigrationDialog->windowFlags()
                            | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint
               | Qt::WindowContextHelpButtonHint);
    _databaseMigrationDialog->setWindowFlags(flags);
    _databaseMigrationDialog->setMinimumDuration(0);
#ifdef Q_OS_WIN
    _databaseMigrationDialog->setWindowTitle(
        QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                    Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
    _databaseMigrationDialog->setWindowTitle(" ");
#endif
    _databaseMigrationDialog->setAttribute(Qt::WA_DeleteOnClose, true);

    _databaseMigrationDialog->setLabelText(
        tr("Migrating database to new version...\n"
           "This might take a few minutes.\nHang tight!"));
    _databaseMigrationDialog->setRange(0, 0);
    _databaseMigrationDialog->setValue(0);
}

void MainWindow::updateStyleRequested(void)
{
    _mainToolBar->updateStyleRequested();
    _mainSplitter->updateStyleRequested();

    if (_favouritesWindow) {
        _favouritesWindow->updateStyleRequested();
    }
    if (_historyWindow) {
        _historyWindow->updateStyleRequested();
    }
}

// Must close settings window, since settings window does not pass the main
// window as parent into the QWidget constructor.
// This is because the settings window uses QColorDialog, which will set the
// focus on the highest-level main window after selecting a colour - hiding the
// settings window.
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_settingsWindow) {
        _settingsWindow->close();
    }
    event->accept();
}
