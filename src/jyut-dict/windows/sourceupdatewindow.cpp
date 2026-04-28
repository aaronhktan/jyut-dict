#include "sourceupdatewindow.h"

#include "components/sourceupdatelist/sourceupdatemodel.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/download/downloader.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/source/sourceutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/strings/strings.h"
#include "logic/utils/utils_windows.h"
#endif
#include "windows/sourceupdateresultwindow.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QProgressDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QTimer>
#include <QUuid>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>

#include <iostream>
#include <ranges>

namespace {
constexpr auto kWindowWidth = 600;
constexpr auto kWindowHeight = 400;
constexpr auto kMaxSimultaneousDownloads = 3;

// Without overriding the paintEvent of QTableView, there is a small vertical
// gutter in the rightmost cell of each row. This subclass fixes the issue.
class SourceUpdateTableView : public QTableView
{
public:
    using QTableView::QTableView;

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QTableView::paintEvent(event);

        if (!model()) {
            return;
        }

        QPainter painter{viewport()};
        const QColor lineColor{palette().color(QPalette::Base)};
        const QRect clip{event->rect()};

        // At the right border of each column except the last, draw a vertical line
        for (int col = 0; col < model()->columnCount() - 1; ++col) {
            if (isColumnHidden(col)) {
                continue;
            }

            const int x = columnViewportPosition(col) + columnWidth(col) - 1;
            painter.fillRect(QRect{x, clip.top(), 1, clip.height()}, lineColor);
        }

        // At the bottom border of each row, draw a horizontal grid line
        int firstRow = rowAt(clip.top());
        if (firstRow < 0) {
            firstRow = 0;
        }

        int lastRow = rowAt(clip.bottom());
        if (lastRow < 0) {
            lastRow = model()->rowCount() - 1;
        }

        for (int row = firstRow; row <= lastRow; ++row) {
            if (isRowHidden(row)) {
                continue;
            }

            const int y = rowViewportPosition(row) + rowHeight(row) - 1;
            painter.fillRect(QRect{clip.left(), y, clip.width(), 1}, lineColor);
        }
    }
};

// On Windows, hovering over a cell in the QTableView causes the entire cell
// to be highlighted with a distracting colour. Disable this behaviour with
// a custom QStyledItemDelegate that removes the MouseOver state.
#ifdef Q_OS_WIN
class NoHoverItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem viewOption{option};
        viewOption.state &= ~QStyle::State_MouseOver;
        QStyledItemDelegate::paint(painter, viewOption, index);
    }
};
#endif
} // namespace

SourceUpdateWindow::SourceUpdateWindow(
    std::vector<IUpdateChecker::SourceManifestMetadata> &a,
    std::shared_ptr<SQLDatabaseManager> manager,
    QWidget *parent)
    : QWidget{parent, Qt::Window}
    , _clickSignaler{new ClickSignaler}
    , _manager{manager}
    , _utils{new SQLDatabaseUtils}
    , _settings{Settings::getSettings()}
{
    setObjectName("SourceUpdateWindow");

    // Get list of existing sources
    std::vector<SourceMetadata> sources;
    QSqlDatabase db = _manager->getDatabase();
    _utils->readSources(db, sources);
    std::unordered_map<std::string, SourceMetadata> sourceMetadata;
    for (const auto &s : sources) {
        sourceMetadata[s.getName()] = s;
    }

    // Find intersection of existing sources + ones that have update available
    std::vector<SourceUpdateModel::MetadataWrapper> updateMetadata;
    for (const auto &s : a) {
        const std::string &name = s.sourceName;
        updateMetadata.emplace_back(sourceMetadata.at(name),
                                    s,
                                    /* checked */ true);
    }

    // Put that data into the model
    _model = new SourceUpdateModel{updateMetadata, this};

    setupUI();
    translateUI();

    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &SourceUpdateWindow::paintWithApplicationState);

    setAttribute(Qt::WA_DeleteOnClose);
}

void SourceUpdateWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() {
            _paletteRecentlyChanged = false;
        });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void SourceUpdateWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        _downloadButton->click();
    }
}

void SourceUpdateWindow::setupUI()
{
    resize(kWindowWidth, kWindowHeight);
    setFixedSize(kWindowWidth, kWindowHeight);

    QWidget *tableWidget = new QWidget{this};

    _description = new QLabel{tableWidget};

    _tableView = new SourceUpdateTableView{tableWidget};
    _tableView->setModel(_model);
    _tableView->setAlternatingRowColors(true);
    _tableView->setSelectionMode(QAbstractItemView::NoSelection);
    _tableView->setFocusPolicy(Qt::NoFocus);
    _tableView->setShowGrid(false);
    _tableView->horizontalHeader()
        ->setSectionResizeMode(SourceUpdateModel::kNameColumn,
                               QHeaderView::Stretch);
    _tableView->horizontalHeader()
        ->setSectionResizeMode(SourceUpdateModel::kInstalledVersionColumn,
                               QHeaderView::Stretch);
    _tableView->horizontalHeader()
        ->setSectionResizeMode(SourceUpdateModel::kAvailableVersionColumn,
                               QHeaderView::Stretch);
    _tableView->horizontalHeader()
        ->setSectionResizeMode(SourceUpdateModel::kCheckColumn,
                               QHeaderView::Interactive);
    _tableView->horizontalHeader()->setSectionsClickable(false);
    _tableView->horizontalHeader()->setSectionsMovable(false);
    _tableView->horizontalHeader()->setHighlightSections(false);
    _tableView->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    _tableView->verticalHeader()->setVisible(false);
#ifdef Q_OS_WIN
    _tableView->setItemDelegate(new NoHoverItemDelegate{_tableView});
#endif

    _toggleAllButton = new QPushButton{tableWidget};

    QVBoxLayout *tableWidgetLayout = new QVBoxLayout{tableWidget};
    tableWidgetLayout->setContentsMargins(0, 11, 0, 0);
    tableWidgetLayout->setSpacing(2);
    tableWidgetLayout->addWidget(_tableView);
    tableWidgetLayout->addWidget(_toggleAllButton);

    QWidget *disableNotificationsWidget = new QWidget{this};
    _disableNotifications = new QCheckBox{disableNotificationsWidget};
    _disableNotifications->setTristate(false);
    _disableNotifications->setChecked(
        !_settings
             ->value("Advanced/sourceUpdateNotificationsEnabled", QVariant{true})
             .toBool());
    _disableNotificationsLabel = new QLabel{disableNotificationsWidget};
    _clickSignaler->installOn(_disableNotificationsLabel);

    QHBoxLayout *disableNotificationsWidgetLayout = new QHBoxLayout{
        disableNotificationsWidget};
    disableNotificationsWidgetLayout->setContentsMargins(0, 0, 0, 0);
    disableNotificationsWidgetLayout->addWidget(_disableNotifications);
    disableNotificationsWidgetLayout->addWidget(_disableNotificationsLabel);
    disableNotificationsWidgetLayout->addStretch();

    _skipButton = new QPushButton{this};
    _downloadButton = new QPushButton{this};
    _downloadButton->setDefault(true);

    _layout = new QGridLayout{this};
    _layout->setContentsMargins(22, 22, 22, 22);
    _layout->addWidget(_description, 0, 0, 1, 2);
    _layout->addWidget(tableWidget, 1, 0, 1, 2);
    _layout->addWidget(disableNotificationsWidget, 3, 0, 1, 2);
    _layout->addWidget(_skipButton, 4, 0, 1, 1);
    _layout->addWidget(_downloadButton, 4, 1, 1, 1);

    connect(_tableView,
            &QTableView::clicked,
            this,
            &SourceUpdateWindow::toggleRowCheckState);
    connect(_toggleAllButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateWindow::toggleAllRows);
    connect(_disableNotifications, &QCheckBox::checkStateChanged, this, [&] {
        _settings->setValue("Advanced/sourceUpdateNotificationsEnabled",
                            !_disableNotifications->checkState());
        _settings->sync();
    });
    connect(_clickSignaler,
            &ClickSignaler::mouseButtonEvent,
            this,
            [this](QWidget *w, QMouseEvent *e) {
                if (w != _disableNotificationsLabel) {
                    return;
                }

                if (e->type() == QEvent::MouseButtonRelease) {
                    _disableNotifications->toggle();
                }
            });
    connect(_skipButton, &QPushButton::clicked, this, [this] { close(); });
    connect(_downloadButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateWindow::downloadSourceUpdates);
    connect(_model,
            &QAbstractItemModel::dataChanged,
            this,
            &SourceUpdateWindow::updateToggleAllButtonText);

    setStyle(Utils::isDarkMode());
}

void SourceUpdateWindow::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());
    _description->setText(
        _model->rowCount() > 1
            ? tr(
                "New versions of your dictionaries are available for download!")
            : tr("A new version of a dictionary is available for download!"));
    updateToggleAllButtonText();
    _disableNotificationsLabel->setText(
        tr("Disable automatic checking for dictionary updates"));
    _skipButton->setText(tr("Skip"));
    _downloadButton->setText(tr("Download Updates..."));
#ifndef Q_OS_MAC
    setWindowTitle(tr("Dictionary Updates"));
#endif
}

void SourceUpdateWindow::setStyle(bool use_dark)
{
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int bodyFontSizeHan = Settings::bodyFontSizeHan.at(
        static_cast<unsigned long>(interfaceSize - 1));

    QString colour = use_dark ? "#424242" : "#d5d5d5";

#ifdef Q_OS_MAC
    QString style{"QLabel[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "} "
                  " "
                  "QLabel { "
                  "   font-size: %2px; "
                  "} "
                  " "
                  "QCheckBox[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "} "
                  " "
                  "QCheckBox { "
                  "   font-size: %2px; "
                  "} "
                  " "
                  "QPushButton[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  //// QPushButton falls back to Fusion style on macOS when the
                  //// height exceeds 16px. Set the maximum size to 16px.
                  "   height: 16px; "
                  "} "
                  " "
                  "QPushButton { "
                  "   font-size: %2px; "
                  "   height: 16px; "
                  "} "};

#else
    QString style{"QLabel[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "} "
                  " "
                  "QLabel { "
                  "   font-size: %2px; "
                  "} "
                  " "
                  "QCheckBox[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "} "
                  " "
                  "QCheckBox { "
                  "   font-size: %2px; "
                  "} "
                  " "
                  "QPushButton[isHan=\"true\"] { "
                  "   font-size: %1px; "
                  "   height: 16px; "
                  "} "
                  " "
                  "QPushButton { "
                  "   font-size: %2px; "
                  "   height: 16px; "
                  "} "
                  "QWidget#SourceUpdateWindow { "
                  "   background-color: palette(base);"
                  "} "};
    setAttribute(Qt::WA_StyledBackground);
#endif
    setStyleSheet(style.arg(std::to_string(bodyFontSizeHan).c_str(),
                            std::to_string(bodyFontSize).c_str()));

    QString headerStyle{"QHeaderView::section { "
                        "   border: none; "
                        "   border-right: 1px solid palette(base); "
                        "} "
                        " "
                        "QHeaderView::section:last { "
                        "   border-right: none; "
                        "} "};

    _tableView->horizontalHeader()->setStyleSheet(headerStyle);
}

void SourceUpdateWindow::toggleRowCheckState(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const int row = index.row();
    const QModelIndex checkboxIndex
        = _model->index(row, SourceUpdateModel::kCheckColumn);
    const Qt::CheckState currentState = static_cast<Qt::CheckState>(
        _model->data(checkboxIndex, Qt::CheckStateRole).toInt());
    const Qt::CheckState nextState = currentState == Qt::Checked ? Qt::Unchecked
                                                                 : Qt::Checked;

    _model->setData(checkboxIndex, nextState, Qt::CheckStateRole);
}

void SourceUpdateWindow::toggleAllRows()
{
    bool allChecked = _model->rowCount() > 0;

    for (const auto r : std::views::iota(0, _model->rowCount())) {
        const QModelIndex checkboxIndex
            = _model->index(r, SourceUpdateModel::kCheckColumn);
        const Qt::CheckState checkState = static_cast<Qt::CheckState>(
            _model->data(checkboxIndex, Qt::CheckStateRole).toInt());

        if (checkState != Qt::Checked) {
            allChecked = false;
            break;
        }
    }

    const Qt::CheckState nextState = allChecked ? Qt::Unchecked : Qt::Checked;

    for (const auto r : std::views::iota(0, _model->rowCount())) {
        _model->setData(_model->index(r, SourceUpdateModel::kCheckColumn),
                        nextState,
                        Qt::CheckStateRole);
    }
}

void SourceUpdateWindow::updateToggleAllButtonText()
{
    bool allChecked = _model->rowCount() > 0;

    for (const auto r : std::views::iota(0, _model->rowCount())) {
        const QModelIndex checkboxIndex
            = _model->index(r, SourceUpdateModel::kCheckColumn);
        const Qt::CheckState checkState = static_cast<Qt::CheckState>(
            _model->data(checkboxIndex, Qt::CheckStateRole).toInt());

        if (checkState != Qt::Checked) {
            allChecked = false;
            break;
        }
    }

    _toggleAllButton->setText(allChecked ? tr("Deselect All")
                                         : tr("Select All"));
}

void SourceUpdateWindow::downloadSourceUpdates()
{
    // Get information for checked items
    std::vector<const IUpdateChecker::SourceManifestMetadata *> sourcesToUpdate;
    for (const auto row : std::views::iota(0, _model->rowCount())) {
        const QModelIndex checkboxIndex
            = _model->index(row, SourceUpdateModel::Columns::kCheckColumn);
        const Qt::CheckState checkState = static_cast<Qt::CheckState>(
            _model->data(checkboxIndex, Qt::CheckStateRole).toInt());

        if (checkState == Qt::Checked) {
            const QModelIndex genericIndex = _model->index(row, 0);
            auto x = _model->data(genericIndex,
                                  SourceUpdateModel::UserRoles::kUpdateInfo);
            sourcesToUpdate.emplace_back(
                x.value<const IUpdateChecker::SourceManifestMetadata *>());
        }
    }

    if (sourcesToUpdate.empty()) {
        close();
    }

    _downloaders.clear();
    std::unordered_set<std::string> urls;
    for (const auto s : sourcesToUpdate) {
        if (urls.contains(s->url)) {
            continue;
        }

        QString outPath = QStandardPaths::standardLocations(
                              QStandardPaths::TempLocation)
                              .at(0)
                          + "/"
                          + QUuid::createUuid().toString(QUuid::WithoutBraces);
        _downloaders.emplace_back(
            new Downloader{QUrl{QString::fromStdString(s->url)}, outPath, this});
        urls.insert(s->url);
        connect(_downloaders.back(),
                &Downloader::downloaded,
                this,
                [this, s](QString outputPath) {
                    ++_completedDownloaders;

                    const auto &outputDatabasePath = outputPath;
                    QFile databaseFile{outputDatabasePath};
                    if (!databaseFile.open(QIODevice::ReadWrite)) {
                        _updateStatus
                            .emplace(s->sourceName,
                                     SourceUpdateStatus::kSourceReadWriteError);
                        startNextDownload();
                        return;
                    }

                    QCryptographicHash hash{QCryptographicHash::Sha256};
                    hash.addData(&databaseFile);
                    if (hash.result().toHex().toStdString() != s->checksum) {
                        _updateStatus
                            .emplace(s->sourceName,
                                     SourceUpdateStatus::kChecksumMismatch);
                        std::cerr << "Downloaded checksum: "
                                  << hash.result().toHex().toStdString()
                                  << " does not match expected checksum "
                                  << s->checksum << " on file "
                                  << outputPath.toStdString() << std::endl;
                        startNextDownload();
                        return;
                    }

                    QString outputConnectionName{
                        QUuid::createUuid().toString(QUuid::WithoutBraces)};
                    QSqlDatabase::addDatabase("QSQLITE", outputConnectionName);
                    QSqlDatabase::database(outputConnectionName)
                        .setDatabaseName(outputDatabasePath);
                    QSqlDatabase::database(outputConnectionName).open();
                    QSqlDatabase outputDatabase = QSqlDatabase::database(
                        outputConnectionName);

                    std::vector<std::pair<std::string, std::string>> sources;
                    _utils->updateDatabase(outputDatabase);
                    _utils->readSources(outputDatabase, sources);

                    if (sources.size() != 1
                        || sources[0].first != s->sourceName) {
                        _updateStatus
                            .emplace(s->sourceName,
                                     SourceUpdateStatus::kSourcenameMismatch);
                        std::cerr << sources[0].first
                                  << " downloaded source name does not match "
                                     "expected source name: "
                                  << s->sourceName << std::endl;
                        startNextDownload();
                        return;
                    }

                    _downloadedFiles.emplace_back(outputPath.toStdString());
                    startNextDownload();
                });

        connect(_downloaders.back(),
                &Downloader::error,
                this,
                [this, s](int err) {
                    _updateStatus.emplace(s->sourceName,
                                          SourceUpdateStatus::kDownloadError);
                    ++_completedDownloaders;
                    startNextDownload();
                });
    }

    _nextDownloader = _downloaders.begin();
    for (int i = 0; i < kMaxSimultaneousDownloads; ++i) {
        if (_nextDownloader != _downloaders.end()) {
            Downloader *front = *_nextDownloader++;
            front->startDownload();
        } else {
            break;
        }
    }

    _dialog = new QProgressDialog{"", QString(), 0, 0, this};
    _dialog->setWindowModality(Qt::ApplicationModal);
    _dialog->setMinimumSize(300, 75);
    Qt::WindowFlags flags = _dialog->windowFlags() | Qt::CustomizeWindowHint;
    flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
               | Qt::WindowFullscreenButtonHint
               | Qt::WindowContextHelpButtonHint);
    _dialog->setWindowFlags(flags);
    _dialog->setMinimumDuration(0);
#ifdef Q_OS_WIN
    _dialog->setWindowTitle(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                        Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
    _dialog->setWindowTitle(" ");
#endif
    _dialog->setAttribute(Qt::WA_DeleteOnClose, true);

    _dialog->setLabelText(tr("Downloading dictionary updates..."));
    _dialog->setRange(0, 0);
    _dialog->setValue(0);

    // The window is closed later, when the results of the update are displayed
    // to the user. See SourceUpdateWindow::notifyUpdateStatus().
    hide();
}

void SourceUpdateWindow::startNextDownload()
{
    if (_downloaders.size() == _completedDownloaders) {
        finishedAllSourceDownloads();
    } else if (_nextDownloader != _downloaders.end()) {
        Downloader *front = *_nextDownloader++;
        front->startDownload();
    }
}

void SourceUpdateWindow::notifyUpdateStatus()
{
    std::ostringstream oss;
    SourceUpdateResultWindow::UpdateResult result
        = SourceUpdateResultWindow::UpdateResult::kNoneSucceeded;
    bool allSucceeded = true;
    for (const auto &[k, v] : _updateStatus) {
        switch (v) {
        case kSuccess: {
            result = SourceUpdateResultWindow::UpdateResult::kSomeSucceeded;
            oss << k << ": succeeded\n";
            break;
        }
        case kDownloadError: {
            oss << k << ": failed due to download error\n";
            allSucceeded = false;
            break;
        }
        case kChecksumMismatch: {
            oss << k << ": failed due to checksum mismatch\n";
            allSucceeded = false;
            break;
        }
        case kSourcenameMismatch: {
            oss << k
                << ": failed because downloaded file did not contain expected "
                   "sourcename\n";
            allSucceeded = false;
            break;
        }
        case kSourceReadWriteError: {
            oss << k
                << ": failed because downloaded file was not read/write "
                   "accessible\n";
            allSucceeded = false;
            break;
        }
        case kSourceMergeFailure: {
            oss << k << ": failed because databases could not be merged\n";
            allSucceeded = false;
            break;
        }
        }
    }

    if (allSucceeded
        && result == SourceUpdateResultWindow::UpdateResult::kSomeSucceeded) {
        result = SourceUpdateResultWindow::UpdateResult::kAllSucceeded;
    }
    std::string description{oss.str()};

    QTimer::singleShot(200, this, [this, result, description] {
        if (_dialog) {
            _dialog->hide();
            _dialog->deleteLater();
            _dialog = nullptr;
        }

        SourceUpdateResultWindow *window
            = new SourceUpdateResultWindow(result, description, this);
        window->setAttribute(Qt::WA_DeleteOnClose, true);
        connect(window, &SourceUpdateWindow::destroyed, this, [this] {
            close();
        });
        window->show();
    });
}

void SourceUpdateWindow::finishedAllSourceDownloads()
{
    if (_downloadedFiles.empty()) {
        if (_dialog) {
            _dialog->hide();
            _dialog->deleteLater();
            _dialog = nullptr;
        }
        notifyUpdateStatus();
        return;
    }

    // All files should now be merged into the first item
    // This lets us bulk-add definitions/sentences/etc. without having
    // to remove and re-create indexes for each source that gets updated.
    auto future = QtConcurrent::run(
        [this] { _utils->mergeDatabases(_downloadedFiles); });

    future.then(this, [this] {
        if (_dialog) {
            _dialog->hide();
            _dialog->deleteLater();
            _dialog = nullptr;
        }

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
            QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                        Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
        _dialog->setWindowTitle(" ");
#endif

        _dialog->setLabelText(tr("Removing old version..."));
        _dialog->setRange(0, 0);
        _dialog->setValue(0);

        disconnect(_utils.get(), nullptr, nullptr, nullptr);

        connect(_utils.get(), &SQLDatabaseUtils::deletingDefinitions, this, [&] {
            _dialog->setLabelText(
                tr("Removing definitions from old version..."));
        });

        connect(_utils.get(),
                &SQLDatabaseUtils::totalToDelete,
                this,
                [&](int numToDelete) {
                    _dialog->setRange(0, numToDelete + 1);
                    _dialog->setLabelText(
                        QString{tr("Deleted entry 0 of %1 from old version")}
                            .arg(numToDelete));
                });

        connect(_utils.get(),
                &SQLDatabaseUtils::deletionProgress,
                this,
                [&](int deleted, int total) {
                    _dialog->setLabelText(
                        QString{tr("Deleted entry %1 of %2 from old version")}
                            .arg(deleted)
                            .arg(total));
                    _dialog->setValue(deleted);
                });

        connect(_utils.get(), &SQLDatabaseUtils::insertingSource, this, [&] {
            _dialog->setValue(0);
            _dialog->setRange(0, 0);
            _dialog->setLabelText(tr("Adding new version..."));
        });

        connect(_utils.get(), &SQLDatabaseUtils::insertingEntries, this, [&] {
            _dialog->setLabelText(tr("Adding new entries..."));
        });

        connect(_utils.get(), &SQLDatabaseUtils::insertingDefinitions, this, [&] {
            _dialog->setLabelText(tr("Adding new definitions..."));
        });

        connect(_utils.get(), &SQLDatabaseUtils::rebuildingIndexes, this, [&] {
            _dialog->setLabelText(tr("Rebuilding search indexes..."));
        });

        connect(_utils.get(),
                &SQLDatabaseUtils::finishedAddition,
                this,
                [this](bool success, QString reason, QString description) {
                    _dialog->reset();
                    std::vector<std::pair<std::string, std::string>> sources;
                    QSqlDatabase db = _manager->getDatabase();
                    _utils->readSources(db, sources);
                    for (const auto &source : sources) {
                        SourceUtils::addSource(source.first, source.second);
                    }

                    _updateStatus
                        .emplace("others",
                                 success
                                     ? SourceUpdateStatus::kSuccess
                                     : SourceUpdateStatus::kSourceMergeFailure);

                    notifyUpdateStatus();
                });

        std::ignore = QtConcurrent::run([this]() {
            QSqlDatabase db = _manager->getDatabase();
            _utils->addSource(db,
                              _downloadedFiles[0],
                              _manager,
                              /* overwriteConflictingDictionaries */ true);
        });
    });
}

void SourceUpdateWindow::paintWithApplicationState(
    [[maybe_unused]] Qt::ApplicationState state)
{
    setStyle(Utils::isDarkMode());
}
