#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components/mainsplitter.h"
#include "components/maintoolbar.h"
#include "logic/update/githubreleasechecker.h"
#include "logic/database/sqldatabasemanager.h"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QProgressDialog>
#include <QWidget>

#include <memory>

// As its name suggests, is the main window of the application
// Contains a toolbar (for searching), and splitter (for results/detail)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    GithubReleaseChecker *_checker;

    MainToolBar *_mainToolBar;
    MainSplitter *_mainSplitter;

    QMenu *_fileMenu;
    QMenu *_editMenu;
    QMenu *_windowMenu;
    QMenu *_helpMenu;

    QPointer<QWidget> _aboutWindow;
    QPointer<QWidget> _settingsWindow;

    QProgressDialog *_dialog;

    std::shared_ptr<SQLDatabaseManager> _manager;

    bool _recentlyCheckedForUpdates = false;

    void createMenus();
    void createActions();

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();

    void toggleMinimized();
    void toggleMaximized();

    void openAboutWindow();
    void openSettingsWindow();

    void checkForUpdate(bool showProgress);

    void closeEvent(QCloseEvent *event) override;

public slots:
    void notifyUpdateAvailable(bool updateAvailable,
                               std::string versionNumber,
                               std::string url,
                               std::string description,
                               bool showIfNoUpdate = false);
};

#endif // MAINWINDOW_H
