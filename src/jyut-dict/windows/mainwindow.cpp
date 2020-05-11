#include "windows/mainwindow.h"

#include "components/favouritewindow/favouritesplitter.h"
#include "dialogs/noupdatedialog.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/dictionary/dictionarysource.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "windows/aboutwindow.h"
#include "logic/utils/utils_qt.h"
#include "windows/settingswindow.h"
#include "windows/updatewindow.h"

#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QColor>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QIcon>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <QTimer>
#include <QUrl>

#include <memory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // Send analytics, but delayed for 100 ms so it doesn't cause
    // slow startup
    _analytics = new Analytics{this};
    QTimer::singleShot(100, this, [&]() {
        _analytics->startSession();
        _analytics->sendScreenview("Main");
        _analytics->sendEvent("language",
                              "load",
                              Settings::getCurrentLocale().name().toStdString());
    });

    // Set window stuff
#ifdef Q_OS_LINUX
    setMinimumSize(QSize(500, 350));
    resize(600, 450);
#else
    setMinimumSize(QSize(800, 600));
#endif

    // Change theme colours
#ifndef Q_OS_MAC
    QPalette defaultPalette = QApplication::palette();
    defaultPalette.setColor(QPalette::Highlight,
                            QColor(LIST_ITEM_ACTIVE_COLOUR_LIGHT_R,
                                   LIST_ITEM_ACTIVE_COLOUR_LIGHT_G,
                                   LIST_ITEM_ACTIVE_COLOUR_LIGHT_B));
    QApplication::setPalette(defaultPalette);
#endif

    // Instantiate services
    _manager = std::make_shared<SQLDatabaseManager>();
    _sqlSearch = std::make_shared<SQLSearch>(_manager);
    _sqlUserUtils = std::make_shared<SQLUserDataUtils>(_manager);

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

    // Populate sources
    SQLDatabaseUtils *_utils = new SQLDatabaseUtils{_manager};
    _utils->updateDatabase();
    std::vector<std::pair<std::string, std::string>> sources;
    _utils->readSources(sources);
    for (auto source : sources) {
        DictionarySourceUtils::addSource(source.first, source.second);
    }
    delete _utils;

    // Install translator
    installTranslator();

    // Create UI elements
    _mainToolBar = new MainToolBar{_sqlSearch, this};
    addToolBar(_mainToolBar);
    setUnifiedTitleAndToolBarOnMac(true);
#ifdef APPIMAGE
    setWindowIcon(QIcon{":/images/icon.png"});
#endif

    _mainSplitter = new MainSplitter{_sqlUserUtils, _manager, _sqlSearch, this};
    setCentralWidget(_mainSplitter);

    // Create menu bar and populate it
    createMenus();
    createActions();
    _mainToolBar->setOpenSettingsAction(_settingsWindowAction);
    _mainToolBar->setOpenFavouritesAction(_favouritesWindowAction);

    // Translate UI
    translateUI();

    // Set style
#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
#else
    setStyle(false);
#endif

    // Check for updates
    _checker = new GithubReleaseChecker{this};
    if (settings->value("Advanced/updateNotificationsEnabled", QVariant{true}).toBool()) {
        QTimer::singleShot(1000, this, [&]() {
            checkForUpdate(/* showProgress = */ false);
        });
    }
}

MainWindow::~MainWindow()
{
    _analytics->endSession();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::installTranslator(void)
{
    if (!Settings::getSettings()->contains("Advanced/locale")) {
        Settings::systemTranslator.load("qt_" + QLocale::system().name(),
                                        QLibraryInfo::location(
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
        for (auto language : QLocale::system().uiLanguages()) {
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

        Settings::systemTranslator.load("qt_" + locale.name(),
                                        QLibraryInfo::location(
                                            QLibraryInfo::TranslationsPath));
        qApp->installTranslator(&Settings::systemTranslator);

        Settings::applicationTranslator.load(/* QLocale */ locale,
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
    for (auto button : buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    setWindowTitle(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                               Strings::PRODUCT_NAME));

    _fileMenu->setTitle(tr("&File"));
    _editMenu->setTitle(tr("&Edit"));
    _windowMenu->setTitle(tr("&Window"));
    _searchMenu->setTitle(tr("&Search"));
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
    _findAndSelectAllAction->setText(tr("Select Contents of Search Bar"));
    _setFocusToResultsAction->setText(
        tr("Jump to First Item in Search Results"));
    _openCurrentSelectionInNewWindowAction->setText(
        tr("Open Selected Entry In New Window"));
    _selectSimplifiedAction->setText(tr("Search Simplified Chinese"));
    _selectTraditionalAction->setText(tr("Search Traditional Chinese"));
    _selectJyutpingAction->setText(tr("Search Jyutping"));
    _selectPinyinAction->setText(tr("Search Pinyin"));
    _selectEnglishAction->setText(tr("Search English"));

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
    (void) (use_dark);
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 20px; }");
#endif
}

void MainWindow::notifyUpdateAvailable(bool updateAvailable,
                                       std::string versionNumber,
                                       std::string url, std::string description,
                                       bool showIfNoUpdate)
{
    if (updateAvailable) {
        UpdateWindow *window = new UpdateWindow{this, versionNumber, url, description};
        window->show();
    } else if (showIfNoUpdate) {
        QString currentVersion = QString{Utils::CURRENT_VERSION};
        NoUpdateDialog *_message = new NoUpdateDialog{currentVersion, this};
        _message->exec();
    }
}

void MainWindow::createMenus(void)
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _editMenu = menuBar()->addMenu(tr("&Edit"));
    _searchMenu = menuBar()->addMenu(tr("&Search"));
    _windowMenu = menuBar()->addMenu(tr("&Window"));
    _helpMenu = menuBar()->addMenu(tr("&Help"));

#ifdef Q_OS_WIN
    menuBar()->setStyleSheet("QMenuBar { \
                                background-color: white; \
                             }");
#endif
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

    _openCurrentSelectionInNewWindowAction = new QAction{this};
    _openCurrentSelectionInNewWindowAction->setShortcut(
        QKeySequence{"Ctrl+Shift+E"});
    connect(_openCurrentSelectionInNewWindowAction,
            &QAction::triggered,
            this,
            &MainWindow::openCurrentSelectionInNewWindow);
    _searchMenu->addAction(_openCurrentSelectionInNewWindowAction);

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

void MainWindow::undo(void)
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

void MainWindow::redo(void)
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

void MainWindow::cut(void)
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

void MainWindow::copy(void)
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

void MainWindow::paste(void)
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

void MainWindow::find(void)
{
    _mainToolBar->setFocus();
}

void MainWindow::findAndSelectAll(void)
{
    _mainToolBar->selectAllEvent();
}

void MainWindow::setFocusToResults(void)
{
    _mainSplitter->setFocusToResults();
}

void MainWindow::openCurrentSelectionInNewWindow(void)
{
    _mainSplitter->openCurrentSelectionInNewWindow();
}

void MainWindow::selectSimplified(void)
{
    _mainToolBar->changeOptionEvent(Utils::SIMPLIFIED_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectTraditional(void)
{
    _mainToolBar->changeOptionEvent(Utils::TRADITIONAL_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectJyutping(void)
{
    _mainToolBar->changeOptionEvent(Utils::JYUTPING_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectPinyin(void)
{
    _mainToolBar->changeOptionEvent(Utils::PINYIN_BUTTON_INDEX);
    _mainToolBar->setFocus();
}

void MainWindow::selectEnglish(void)
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
    _favouritesWindow->show();
    _favouritesWindow->setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::checkForUpdate(bool showProgress)
{
    disconnect(_checker, nullptr, nullptr, nullptr);
    if (showProgress) {
        connect(_checker,
                &GithubReleaseChecker::foundUpdate,
                [&](bool updateAvailable,
                    std::string versionNumber,
                    std::string url,
                    std::string description) {
                    _dialog->reset();

                    disconnect(_checker, nullptr, nullptr, nullptr);
                    notifyUpdateAvailable(updateAvailable,
                                          versionNumber,
                                          url,
                                          description,
                                          /* showIfNoUpdate = */ true);

                    _recentlyCheckedForUpdates = false;
                });

        _dialog = new QProgressDialog{"", QString(), 0, 0, this};
        _dialog->setWindowModality(Qt::ApplicationModal);
        _dialog->setMinimumSize(300, 75);
        Qt::WindowFlags flags = _dialog->windowFlags()
                                | Qt::CustomizeWindowHint;
        flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
                   | Qt::WindowFullscreenButtonHint
                   | Qt::WindowContextHelpButtonHint);
        _dialog->setWindowFlags(flags);
        _dialog->setMinimumDuration(0);
#ifdef Q_OS_WIN
        _dialog->setWindowTitle(
            QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
        _dialog->setWindowTitle(" ");
#endif
        _dialog->setAttribute(Qt::WA_DeleteOnClose, true);

        _dialog->setLabelText(tr("Checking for update..."));
        _dialog->setRange(0, 0);
        _dialog->setValue(0);
    } else {
        connect(_checker,
                &GithubReleaseChecker::foundUpdate,
                [&](bool updateAvailable,
                    std::string versionNumber,
                    std::string url,
                    std::string description) {
                    disconnect(_checker, nullptr, nullptr, nullptr);
                    notifyUpdateAvailable(updateAvailable,
                                          versionNumber,
                                          url,
                                          description);

                    _recentlyCheckedForUpdates = false;
                });
    }

    if (!_recentlyCheckedForUpdates) {
        _checker->checkForNewUpdate();
        _recentlyCheckedForUpdates = true;
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
