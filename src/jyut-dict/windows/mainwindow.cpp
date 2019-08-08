#include "windows/mainwindow.h"

#include "logic/database/sqldatabaseutils.h"
#include "logic/dictionary/dictionarysource.h"
#include "logic/entry/sentence.h"
#include "windows/settingswindow.h"
#include "windows/updatewindow.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QUrl>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("Jyut Dictionary");
#ifdef Q_OS_LINUX
    setMinimumSize(QSize(300, 450));
#else
    setMinimumSize(QSize(800, 600));
#endif

    // Instantiate services
    _manager = std::make_shared<SQLDatabaseManager>();
    _manager->openDatabase();

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
    QTimer::singleShot(1000, _checker, &GithubReleaseChecker::checkForNewUpdate);
    connect(_checker, &GithubReleaseChecker::foundUpdate, this,
            &MainWindow::notifyUpdateAvailable);
}

MainWindow::~MainWindow()
{

}

void MainWindow::notifyUpdateAvailable(bool updateAvailable,
                                       std::string versionNumber,
                                       std::string url, std::string description)
{
    if (updateAvailable) {
        UpdateWindow *window = new UpdateWindow{this, versionNumber, url, description};
        window->show();
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
    connect(aboutAction, &QAction::triggered, this, &QApplication::aboutQt);
    // TODO: Implement an about dialog to give credits to contributors :)
    // connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
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

    QAction *helpAction = new QAction{tr("Jyut Dictionary Help"), this};
    connect(helpAction, &QAction::triggered, this, [](){QDesktopServices::openUrl(QUrl{Utils::GITHUB_LINK});});
    _helpMenu->addAction(helpAction);
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
