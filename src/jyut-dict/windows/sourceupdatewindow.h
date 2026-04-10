#ifndef SOURCEUPDATEWINDOW_H
#define SOURCEUPDATEWINDOW_H

#include "logic/update/iupdatechecker.h"

#include <QWidget>

#include <deque>

class Downloader;
class SQLDatabaseManager;
class SQLDatabaseUtils;
class SourceUpdateModel;

class QLabel;
class QPushButton;
class QProgressDialog;
class QSettings;
class QTableView;
class QVBoxLayout;

class SourceUpdateWindow : public QWidget
{
public:
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

    void updateSources();

    SourceUpdateModel *_model;
    QWidget *_widget;
    QVBoxLayout *_layout;

    QTableView *_tableView;
    QLabel *_description;
    QPushButton *_toggleAllButton;
    QPushButton *_skipButton;
    QPushButton *_downloadButton;

    std::deque<Downloader *> _downloaders;
    std::vector<std::string> _downloadedFiles;
    QProgressDialog *_dialog;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLDatabaseUtils> _utils;
    std::shared_ptr<QSettings> _settings;

    bool _paletteRecentlyChanged = false;

public slots:
    void finishedAllSourceDownloads();
    void paintWithApplicationState(Qt::ApplicationState state);
};

#endif // SOURCEUPDATEWINDOW_H
