#ifndef SOURCEUPDATEWINDOW_H
#define SOURCEUPDATEWINDOW_H

#include "logic/update/iupdatechecker.h"

#include <QWidget>

class Downloader;
class SQLDatabaseManager;
class SQLDatabaseUtils;
class SourceUpdateModel;

class QCheckBox;
class QLabel;
class QPushButton;
class QProgressDialog;
class QSettings;
class QTableView;
class QVBoxLayout;

// TODO:
// Implement keyboard shortcuts (return/enter, esc)
// Implement update status notification window

class SourceUpdateWindow : public QWidget
{
public:
    enum SourceUpdateStatus {
        kSuccess,
        kDownloadError,
        kChecksumMismatch,
        kSourcenameMismatch,
        kSourceReadWriteError,
        kSourceMergeFailure,
    };

    explicit SourceUpdateWindow(
        std::vector<IUpdateChecker::SourceManifestMetadata> &a,
        std::shared_ptr<SQLDatabaseManager> manager,
        QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI();
    void translateUI();
    void setStyle(bool use_dark);

    void toggleRowCheckState(const QModelIndex &index);
    void toggleAllRows();
    void updateToggleAllButtonText();

    void downloadSourceUpdates();
    // Note: This function should only be called in the lambda associated
    // with Downloader::downloaded
    void startNextDownload();

    void notifyUpdateStatus();

    SourceUpdateModel *_model;
    QWidget *_widget;
    QVBoxLayout *_layout;

    QTableView *_tableView;
    QLabel *_description;
    QPushButton *_toggleAllButton;
    QCheckBox *_disableNotifications;
    QPushButton *_skipButton;
    QPushButton *_downloadButton;

    std::vector<Downloader *> _downloaders;
    std::vector<Downloader *>::iterator _nextDownloader;
    size_t _completedDownloaders;
    std::vector<std::string> _downloadedFiles;
    QProgressDialog *_dialog = nullptr;
    std::unordered_map<std::string, SourceUpdateStatus> _updateStatus;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLDatabaseUtils> _utils;
    std::shared_ptr<QSettings> _settings;

    bool _paletteRecentlyChanged = false;

public slots:
    void finishedAllSourceDownloads();
    void paintWithApplicationState(Qt::ApplicationState state);
};

#endif // SOURCEUPDATEWINDOW_H
