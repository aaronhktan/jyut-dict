#ifndef SOURCEUPDATEWINDOW_H
#define SOURCEUPDATEWINDOW_H

#include "logic/update/iupdatechecker.h"

#include <QEvent>
#include <QMouseEvent>
#include <QWidget>

class Downloader;
class SQLDatabaseManager;
class SQLDatabaseUtils;
class SourceUpdateModel;

class QCheckBox;
class QGridLayout;
class QLabel;
class QPushButton;
class QProgressDialog;
class QSettings;
class QTableView;

// The SourceUpdateWindow displays available source updates (a.k.a. dictionary updates)
// to the user. This window allows a user to select which updates to download, then
// downloads and applies the updates.

// Modified from https://stackoverflow.com/questions/32018941/qlabel-click-event-using-qt
class ClickSignaler : public QObject
{
    Q_OBJECT

    bool eventFilter(QObject *obj, QEvent *ev) override
    {
        if (ev->type() == QEvent::MouseButtonRelease && obj->isWidgetType()) {
            emit mouseButtonEvent(static_cast<QWidget *>(obj),
                                  static_cast<QMouseEvent *>(ev));
        }
        return false;
    }

public:
    ClickSignaler(QObject *parent = nullptr)
        : QObject{parent}
    {}
    void installOn(QWidget *widget) { widget->installEventFilter(this); }

signals:
    void mouseButtonEvent(QWidget *, QMouseEvent *);
};

class SourceUpdateWindow : public QWidget
{
    Q_OBJECT

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
    // Note: This function should only be called in the slot wired to
    // Downloader::downloaded
    void startNextDownload();

    void notifyUpdateStatus();

    SourceUpdateModel *_model;
    QGridLayout *_layout;

    QTableView *_tableView;
    QLabel *_description;
    QPushButton *_toggleAllButton;
    QCheckBox *_disableNotifications;
    QLabel *_disableNotificationsLabel;
    QPushButton *_skipButton;
    QPushButton *_downloadButton;

    std::vector<Downloader *> _downloaders;
    std::vector<Downloader *>::iterator _nextDownloader;
    size_t _completedDownloaders;
    std::vector<std::string> _downloadedFiles;
    QProgressDialog *_dialog = nullptr;
    std::unordered_map<std::string, SourceUpdateStatus> _updateStatus;

    ClickSignaler *_clickSignaler;
    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLDatabaseUtils> _utils;
    std::shared_ptr<QSettings> _settings;

    bool _paletteRecentlyChanged = false;

public slots:
    void finishedAllSourceDownloads();
    void paintWithApplicationState(Qt::ApplicationState state);
};

#endif // SOURCEUPDATEWINDOW_H
