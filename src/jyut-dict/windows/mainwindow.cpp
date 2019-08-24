#include "windows/mainwindow.h"

#include "logic/database/sqldatabaseutils.h"
#include "logic/dictionary/dictionarysource.h"
#include "logic/entry/sentence.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "windows/aboutwindow.h"
#include "windows/settingswindow.h"
#include "windows/updatewindow.h"

#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <QTimer>
#include <QUrl>

#include <memory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(tr(Utils::PRODUCT_NAME));
#ifdef Q_OS_LINUX
    setMinimumSize(QSize(500, 350));
    resize(600, 450);
#else
    setMinimumSize(QSize(800, 600));
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

    // Create UI elements
    _mainToolBar = new MainToolBar{_manager, this};
    addToolBar(_mainToolBar);
    setUnifiedTitleAndToolBarOnMac(true);

    _mainSplitter = new MainSplitter{this};
    setCentralWidget(_mainSplitter);

    // Create menu bar and populate it
    createMenus();
    createActions();

    // Check for updates
    _checker = new GithubReleaseChecker{this};
    if (settings->value("Advanced/UpdateNotificationsEnabled", QVariant{true}).toBool()) {
        QTimer::singleShot(1000, this, [&]() {
            checkForUpdate(/* showProgress = */ false);
        });
    }
}

MainWindow::~MainWindow()
{

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
        _message->setWindowTitle(tr(Utils::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
        _message->setWindowTitle(" ");
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
    QAction *aboutAction = new QAction{tr("&About"), this};
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::openAboutWindow);
    _fileMenu->addAction(aboutAction);

#ifdef Q_OS_MAC
    QAction *settingsWindowAction = new QAction{tr("Preferences"), this};
#else
    QAction *settingsWindowAction = new QAction{tr("Settings"), this};
#endif
    settingsWindowAction->setStatusTip(tr("Change settings"));
    settingsWindowAction->setShortcut(QKeySequence{"Ctrl+,"});
    connect(settingsWindowAction, &QAction::triggered, this, &MainWindow::openSettingsWindow);
    _fileMenu->addAction(settingsWindowAction);

    QAction *closeWindowAction = new QAction{tr("Close Window"), this};
    closeWindowAction->setShortcut(QKeySequence{"Ctrl+W"});
    connect(closeWindowAction, &QAction::triggered, this, &QWidget::close);
    _fileMenu->addAction(closeWindowAction);

    QAction *undoAction = new QAction{tr("Undo"), this};
    undoAction->setShortcut(QKeySequence{"Ctrl+Z"});
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);
    _editMenu->addAction(undoAction);

    QAction *redoAction = new QAction{tr("Redo"), this};
    redoAction->setShortcut(QKeySequence{"Ctrl+Shift+Z"});
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);
    _editMenu->addAction(redoAction);

    _editMenu->addSeparator();

    QAction *cutAction = new QAction{tr("Cut"), this};
    cutAction->setShortcut(QKeySequence{"Ctrl+X"});
    connect(cutAction, &QAction::triggered, this, &MainWindow::cut);
    _editMenu->addAction(cutAction);

    QAction *copyAction = new QAction{tr("Copy"), this};
    copyAction->setShortcut(QKeySequence{"Ctrl+C"});
    connect(copyAction, &QAction::triggered, this, &MainWindow::copy);
    _editMenu->addAction(copyAction);

    QAction *pasteAction = new QAction{tr("Paste"), this};
    pasteAction->setShortcut(QKeySequence{"Ctrl+V"});
    connect(pasteAction, &QAction::triggered, this, &MainWindow::paste);
    _editMenu->addAction(pasteAction);

    QAction *minimizeAction = new QAction{tr("Minimize"), this};
    minimizeAction->setShortcut(QKeySequence{"Ctrl+M"});
    connect(minimizeAction, &QAction::triggered, this, &MainWindow::toggleMinimized);
    _windowMenu->addAction(minimizeAction);

    QAction *maximizeAction = new QAction{tr("Zoom"), this};
    connect(maximizeAction, &QAction::triggered, this, &MainWindow::toggleMaximized);
    _windowMenu->addAction(maximizeAction);

    _windowMenu->addSeparator();

    QAction *bringAllToFrontAction = new QAction{tr("Bring All to Front"), this};
    connect(bringAllToFrontAction, &QAction::triggered, this, &QWidget::showNormal);
    _windowMenu->addAction(bringAllToFrontAction);

    QAction *helpAction = new QAction{tr("%1 Help").arg(tr(Utils::PRODUCT_NAME)), this};
    connect(helpAction, &QAction::triggered, this, [](){QDesktopServices::openUrl(QUrl{Utils::GITHUB_LINK});});
    _helpMenu->addAction(helpAction);

    QAction *updateAction = new QAction{tr("Check for updates..."), this};
    updateAction->setMenuRole(QAction::ApplicationSpecificRole);
    connect(updateAction, &QAction::triggered, [&]() {
        checkForUpdate(/* showProgress = */ true);
    });
    _helpMenu->addAction(updateAction);
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
        _dialog->setWindowTitle(tr(Utils::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
        _dialog->setWindowTitle(" ");
#endif
        _dialog->setAttribute(Qt::WA_DeleteOnClose, true);

        _dialog->setLabelText(tr("Checking for update..."));
        _dialog->setRange(0, 0);
        _dialog->setValue(0);

        connect(_checker,
                &GithubReleaseChecker::foundUpdate,
                [&](bool updateAvailable,
                    std::string versionNumber,
                    std::string url,
                    std::string description) {
                    _dialog->reset();

                    notifyUpdateAvailable(updateAvailable,
                                          versionNumber,
                                          url,
                                          description,
                                          /* showIfNoUpdate = */ true);
                });
    } else {
        connect(_checker,
                &GithubReleaseChecker::foundUpdate,
                this,
                [&](bool updateAvailable,
                    std::string versionNumber,
                    std::string url,
                    std::string description) {
                    notifyUpdateAvailable(updateAvailable,
                                          versionNumber,
                                          url,
                                          description);
                });
    }

    _checker->checkForNewUpdate();
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
