#include "windows/mainwindow.h"

#include "logic/database/sqldatabaseutils.h"
#include "logic/dictionary/dictionarysource.h"
#include "logic/entry/sentence.h"
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
    defaultPalette.setColor(QPalette::Highlight, Utils::LIST_ITEM_ACTIVE_COLOUR_LIGHT);
    QApplication::setPalette(defaultPalette);
#endif

    // Instantiate services
    _manager = std::make_shared<SQLDatabaseManager>();
    _manager->openDatabase();

    // Get colours from QSettings
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    int size = settings->beginReadArray("jyutpingColours");
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

    size = settings->beginReadArray("pinyinColours");
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
    std::vector<std::pair<std::string, std::string>> sources;
    _utils->readSources(sources);
    for (auto source : sources) {
        DictionarySourceUtils::addSource(source.first, source.second);
    }
    delete _utils;

    // Install translator
    installTranslator();

    // Create UI elements
    _mainToolBar = new MainToolBar{_manager, this};
    addToolBar(_mainToolBar);
    setUnifiedTitleAndToolBarOnMac(true);
#ifdef APPIMAGE
    setWindowIcon(QIcon{":/images/icon.png"});
#endif

    _mainSplitter = new MainSplitter{this};
    setCentralWidget(_mainSplitter);

    // Create menu bar and populate it
    createMenus();
    createActions();

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

void MainWindow::installTranslator()
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

void MainWindow::translateUI()
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

    _minimizeAction->setText(tr("Minimize"));
    _maximizeAction->setText(tr("Zoom"));
    _bringAllToFrontAction->setText(tr("Bring All to Front"));

    _helpAction->setText(tr("%1 Help").arg(
        QCoreApplication::translate("strings", Strings::PRODUCT_NAME)));
    _updateAction->setText(tr("Check for updates..."));

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
        QMessageBox *_message = new QMessageBox{this};
        Qt::WindowFlags flags = _message->windowFlags()
                                | Qt::CustomizeWindowHint;
        flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
                   | Qt::WindowFullscreenButtonHint);
        _message->setWindowFlags(flags);
        _message->setAttribute(Qt::WA_DeleteOnClose, true);
        _message->setText(tr("No update found!"));
        _message->setInformativeText(tr("You are on the newest version, %1.").arg(Utils::CURRENT_VERSION));
        _message->setStandardButtons(QMessageBox::Ok);
        _message->setIcon(QMessageBox::Information);
#ifdef Q_OS_WIN
        _message->setWindowTitle(
            QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
        _message->setWindowTitle(tr("No update available!"));
#endif
        // Setting minimum width also doesn't work, so use this
        // workaround to set a width.
        QSpacerItem *horizontalSpacer = new QSpacerItem(400,
                                                        0,
                                                        QSizePolicy::Minimum,
                                                        QSizePolicy::Minimum);
        QGridLayout *layout = static_cast<QGridLayout *>(_message->layout());
        layout->addItem(horizontalSpacer,
                        layout->rowCount(),
                        0,
                        1,
                        layout->columnCount());

        _message->exec();
    }
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _editMenu = menuBar()->addMenu(tr("&Edit"));
    _windowMenu = menuBar()->addMenu(tr("&Window"));
    _helpMenu = menuBar()->addMenu(tr("&Help"));

#ifdef Q_OS_WIN
    menuBar()->setStyleSheet("QMenuBar { \
                                background-color: white; \
                             }");
#endif
}

void MainWindow::createActions()
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

void MainWindow::undo()
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

void MainWindow::redo()
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

void MainWindow::cut()
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

void MainWindow::copy()
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

void MainWindow::paste()
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

void MainWindow::toggleMinimized()
{
    if (!isMinimized()) {
        showMinimized();
    } else {
        showNormal();
    }
}

void MainWindow::toggleMaximized()
{
    if (!isMaximized()) {
        showMaximized();
    } else {
        showNormal();
    }
}

void MainWindow::openAboutWindow()
{
    if (_aboutWindow) {
        _aboutWindow->activateWindow();
        _aboutWindow->raise();
        return;
    }

    _aboutWindow = new AboutWindow{this};
    _aboutWindow->show();
}

void MainWindow::openSettingsWindow()
{
    if (_settingsWindow) {
        _settingsWindow->activateWindow();
        _settingsWindow->raise();
        return;
    }

    _settingsWindow = new SettingsWindow{_manager, this};
    _settingsWindow->show();
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
