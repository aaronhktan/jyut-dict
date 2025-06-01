#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components/favouritewindow/favouritesplitter.h"
#include "components/mainwindow/mainsplitter.h"
#include "components/mainwindow/maintoolbar.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/database/sqluserdatautils.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/search/sqlsearch.h"
#include "logic/update/jyutdictionaryreleasechecker.h"
#include "windows/aboutwindow.h"
#include "windows/historywindow.h"
#include "windows/settingswindow.h"
#include "windows/welcomewindow.h"

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

    void changeEvent(QEvent *event) override;

private:
    JyutDictionaryReleaseChecker *_checker;

    MainToolBar *_mainToolBar;
    MainSplitter *_mainSplitter;

    QMenu *_fileMenu;
    QMenu *_editMenu;
    QMenu *_searchMenu;
    QMenu *_entryMenu;
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
    QAction *_dictationAction;
    QAction *_handwritingAction;
    QAction *_setFocusToResultsAction;
    QAction *_selectSimplifiedAction;
    QAction *_selectTraditionalAction;
    QAction *_selectJyutpingAction;
    QAction *_selectPinyinAction;
    QAction *_selectEnglishAction;
    QAction *_autoDetectLanguageAction;
    QAction *_fuzzyJyutpingAction;
    QAction *_fuzzyPinyinAction;

    QAction *_favouriteCurrentEntryAction;
    QAction *_shareCurrentEntryAction;
    QAction *_openCurrentEntryInNewWindowAction;
    QAction *_magnifyCurrentEntryAction;
    QAction *_viewAllSentencesAction;
    QAction *_searchWordsBeginningAction;
    QAction *_searchWordsContainingAction;
    QAction *_searchWordsEndingAction;

    QAction *_historyWindowAction;
    QAction *_favouritesWindowAction;
    QAction *_minimizeAction;
    QAction *_maximizeAction;
    QAction *_bringAllToFrontAction;

    QAction *_helpAction;
    QAction *_updateAction;

    QPointer<AboutWindow> _aboutWindow;
    QPointer<SettingsWindow> _settingsWindow;
    QPointer<HistoryWindow> _historyWindow;
    QPointer<FavouriteSplitter> _favouritesWindow;
    QPointer<WelcomeWindow> _welcomeWindow;

    QProgressDialog *_updateDialog = nullptr;
    QProgressDialog *_databaseMigrationDialog = nullptr;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::shared_ptr<SQLSearch> _sqlSearch;
    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;
    std::unique_ptr<SQLDatabaseUtils> _utils;
    std::unique_ptr<QSettings> _settings;

    bool _recentlyCheckedForUpdates = false;

    // Cached information for various dialogs
    bool _databaseMigrating = false;
    bool _updateAvailable = false;
    std::string _updateVersionNumber;
    std::string _updateURL;
    std::string _updateDescription;

    void installTranslator(void);
    void translateUI(void);

    void setStyle(bool use_dark);

    void createMenus(void);
    void createActions(void);

    void undo(void) const;
    void redo(void) const;
    void cut(void) const;
    void copy(void) const;
    void paste(void) const;
    void find(void) const;
    void findAndSelectAll(void) const;
    void dictation(void) const;
    void handwriting(void) const;
    void setFocusToResults(void) const;
    void selectSimplified(void) const;
    void selectTraditional(void) const;
    void selectJyutping(void) const;
    void selectPinyin(void) const;
    void selectEnglish(void) const;
    void autoDetectLanguage(void) const;
    void fuzzyJyutping(void) const;
    void fuzzyPinyin(void) const;

    void toggleMinimized(void);
    void toggleMaximized(void);

    void openAboutWindow(void);
    void openSettingsWindow(void);
    void openHistoryWindow(void);
    void openFavouritesWindow(void);
    void openWelcomeWindow(void);

    void checkForUpdate(bool showProgress);

    void closeEvent(QCloseEvent *event) override;

signals:
    void searchHistoryClicked(const searchTermHistoryItem &pair);
    void viewHistoryClicked(const Entry &entry);

    void favouriteCurrentEntry(void);
    void shareCurrentEntry(void);
    void openCurrentEntryInNewWindow(void);
    void magnifyCurrentEntry(void);
    void viewAllSentences(void);

    void searchEntriesBeginning(void);
    void searchEntriesContaining(void);
    void searchEntriesEnding(void);

public slots:
    void notifyUpdateAvailable(bool updateAvailable,
                               std::string versionNumber,
                               std::string url,
                               std::string description,
                               bool showIfNoUpdate = false);
    void notifyDatabaseMigration(void);
    void finishedDatabaseMigration(bool success);
    void forwardSearchHistoryItem(const searchTermHistoryItem &pair);
    void forwardViewHistoryItem(const Entry &entry);
    void searchRequested(void);
    void updateStyleRequested(void);
};

#endif // MAINWINDOW_H
