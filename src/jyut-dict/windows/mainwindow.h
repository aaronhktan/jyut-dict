#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components/mainsplitter.h"
#include "components/maintoolbar.h"
#include "logic/analytics/analytics.h"
#include "logic/update/githubreleasechecker.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/search/sqlsearch.h"

#include <QAction>
#include <QEvent>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QProgressDialog>
#include <QTranslator>
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

    void changeEvent(QEvent *event) override;

private:
    Analytics *_analytics;
    GithubReleaseChecker *_checker;

    MainToolBar *_mainToolBar;
    MainSplitter *_mainSplitter;

    QMenu *_fileMenu;
    QMenu *_editMenu;
    QMenu *_searchMenu;
    QMenu *_windowMenu;
    QMenu *_helpMenu;

    QAction *_aboutAction;
    QAction *_settingsWindowAction;
    QAction *_closeWindowAction;

    QAction *_undoAction;
    QAction *_redoAction;
    QAction *_cutAction;
    QAction *_copyAction;
    QAction *_pasteAction;
    QAction *_findAction;
    QAction *_findAndSelectAllAction;
    QAction *_setFocusToResultsAction;
    QAction *_openCurrentSelectionInNewWindowAction;
    QAction *_selectSimplifiedAction;
    QAction *_selectTraditionalAction;
    QAction *_selectJyutpingAction;
    QAction *_selectPinyinAction;
    QAction *_selectEnglishAction;

    QAction *_minimizeAction;
    QAction *_maximizeAction;
    QAction *_bringAllToFrontAction;

    QAction *_helpAction;
    QAction *_updateAction;

    QPointer<QWidget> _aboutWindow;
    QPointer<QWidget> _settingsWindow;

    QProgressDialog *_dialog;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::shared_ptr<SQLSearch> _sqlSearch;

    bool _recentlyCheckedForUpdates = false;

    void installTranslator(void);
    void translateUI(void);

    void setStyle(bool use_dark);

    void createMenus(void);
    void createActions(void);

    void undo(void);
    void redo(void);
    void cut(void);
    void copy(void);
    void paste(void);
    void find(void);
    void findAndSelectAll(void);
    void setFocusToResults(void);
    void openCurrentSelectionInNewWindow(void);
    void selectSimplified(void);
    void selectTraditional(void);
    void selectJyutping(void);
    void selectPinyin(void);
    void selectEnglish(void);

    void toggleMinimized(void);
    void toggleMaximized(void);

    void openAboutWindow(void);
    void openSettingsWindow(void);

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
