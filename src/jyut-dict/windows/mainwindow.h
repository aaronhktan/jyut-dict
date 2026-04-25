#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "logic/database/sqluserhistoryutils.h"
#include "logic/update/iupdatechecker.h"

#include <QMainWindow>
#include <QPointer>

#include <deque>
#include <memory>
#include <optional>
#include <string>

// As its name suggests, is the main window of the application
// Contains a toolbar (for searching), and splitter (for results/detail)

class AboutWindow;
class Entry;
class FavouriteSplitter;
class HistoryWindow;
class JyutDictionaryReleaseChecker;
class MainSplitter;
class MainToolBar;
class SettingsWindow;
class SourceUpdateWindow;
class SourceReleaseChecker;
class SQLDatabaseManager;
class SQLDatabaseUtils;
class SQLSearch;
class SQLUserHistoryUtils;
class SQLUserDataUtils;
class UpdateAvailableWindow;
class WelcomeWindow;

class QAction;
class QEvent;
class QMenu;
class QMenuBar;
class QProgressDialog;
class QSettings;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    JyutDictionaryReleaseChecker *_checker;
    SourceReleaseChecker *_sourceChecker;

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
    QAction *_updateSourcesAction;

    QPointer<AboutWindow> _aboutWindow = nullptr;
    QPointer<SettingsWindow> _settingsWindow = nullptr;
    QPointer<HistoryWindow> _historyWindow = nullptr;
    QPointer<FavouriteSplitter> _favouritesWindow = nullptr;
    QPointer<WelcomeWindow> _welcomeWindow = nullptr;

    QPointer<UpdateAvailableWindow> _updateAvailableWindow = nullptr;
    QPointer<SourceUpdateWindow> _sourceUpdateWindow = nullptr;

    QProgressDialog *_updateCheckProgressDialog = nullptr;
    QProgressDialog *_databaseMigrationDialog = nullptr;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::shared_ptr<SQLSearch> _sqlSearch;
    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;
    std::unique_ptr<SQLDatabaseUtils> _utils;
    std::unique_ptr<QSettings> _settings;

    bool _recentlyCheckedForUpdates = false;
    bool _recentlyCheckedForSourceUpdates = false;

    bool _databaseMigrating = false;

    std::deque<std::function<void()>> _dialogQueue;

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
    void checkForSourceUpdate(bool showProgress);

    void closeEvent(QCloseEvent *event) override;

signals:
    void searchHistoryClicked(const SearchTermHistoryItem &pair);
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
                               std::optional<std::string> versionNumber,
                               std::optional<std::string> url,
                               std::optional<std::string> description,
                               bool showIfNoUpdate = false);
    void notifySourceUpdateAvailable(
        std::vector<IUpdateChecker::SourceManifestMetadata> &a,
        bool showIfNoUpdate = false);
    void notifyDatabaseMigration(void);
    void finishedDatabaseMigration(bool success);
    void forwardSearchHistoryItem(const SearchTermHistoryItem &pair);
    void forwardViewHistoryItem(const Entry &entry);
    void searchRequested(void);
    void updateStyleRequested(void);
};

#endif // MAINWINDOW_H
