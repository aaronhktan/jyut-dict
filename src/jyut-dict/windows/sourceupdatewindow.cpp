#include "sourceupdatewindow.h"

#include "components/sourceupdatelist/sourceupdatemodel.h"
// #include "dialogs/downloadresultdialog.h"
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
#include "logic/utils/utils_windows.h"
#endif

#include <QAbstractItemView>
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
#include <QTableView>
#include <QTimer>
#include <QUuid>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>

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

        if (model() == nullptr) {
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
} // namespace

SourceUpdateWindow::SourceUpdateWindow(
    std::vector<IUpdateChecker::SourceManifestMetadata> &a,
    std::shared_ptr<SQLDatabaseManager> manager,
    QWidget *parent)
    : QWidget{parent, Qt::Window}
    , _manager{manager}
    , _utils{new SQLDatabaseUtils{manager}}
    , _settings{Settings::getSettings()}
{
    // Get list of existing sources
    std::vector<SourceMetadata> sources;
    _utils->readSources(sources);
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
}

void SourceUpdateWindow::setupUI()
{
    resize(kWindowWidth, kWindowHeight);
    setFixedSize(kWindowWidth, kWindowHeight);

    _widget = new QWidget{this};

    _description = new QLabel{_widget};

    _tableView = new SourceUpdateTableView{_widget};
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
        ->setSectionResizeMode(SourceUpdateModel::kNewVersionColumn,
                               QHeaderView::Stretch);
    _tableView->horizontalHeader()
        ->setSectionResizeMode(SourceUpdateModel::kCheckColumn,
                               QHeaderView::Interactive);
    _tableView->horizontalHeader()->setSectionsClickable(false);
    _tableView->horizontalHeader()->setSectionsMovable(false);
    _tableView->horizontalHeader()->setHighlightSections(false);
    _tableView->horizontalHeader()->setFocusPolicy(Qt::NoFocus);
    _tableView->verticalHeader()->setVisible(false);

    _toggleAllButton = new QPushButton{_widget};

    _skipButton = new QPushButton{_widget};
    _downloadButton = new QPushButton{_widget};
    _downloadButton->setDefault(true);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addWidget(_skipButton);
    buttonLayout->addWidget(_downloadButton);

    QVBoxLayout *widgetLayout = new QVBoxLayout{_widget};
    widgetLayout->setContentsMargins(0, 11, 0, 0);
    widgetLayout->setSpacing(2);
    widgetLayout->addWidget(_tableView);
    widgetLayout->addWidget(_toggleAllButton);

    _layout = new QVBoxLayout{this};
    _layout->setContentsMargins(22, 22, 22, 22);
    _layout->addWidget(_description);
    _layout->addWidget(_widget);
    _layout->addLayout(buttonLayout);

    connect(_tableView,
            &QTableView::clicked,
            this,
            &SourceUpdateWindow::toggleRowCheckState);
    connect(_toggleAllButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateWindow::toggleAllRows);
    connect(_skipButton, &QPushButton::clicked, this, [this]() { close(); });
    connect(_downloadButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateWindow::updateSources);
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
    _skipButton->setText(tr("Skip"));
    _downloadButton->setText(tr("Download Updates..."));
    setWindowTitle(tr("Dictionary Updates"));
}

void SourceUpdateWindow::setStyle(bool use_dark)
{
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int uiFontSize = Settings::uiFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int uiFontSizeHan = Settings::uiFontSizeHan.at(
        static_cast<unsigned long>(interfaceSize - 1));
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
                  "} "};
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

void SourceUpdateWindow::updateSources()
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
            qDebug() << x.canConvert<
                const IUpdateChecker::SourceManifestMetadata *>();
            sourcesToUpdate.emplace_back(
                x.value<const IUpdateChecker::SourceManifestMetadata *>());
        }
    }

    _downloaders.clear();
    for (const auto s : sourcesToUpdate) {
        QString outPath = QStandardPaths::standardLocations(
                              QStandardPaths::TempLocation)
                              .at(0)
                          + "/"
                          + QUuid::createUuid().toString(QUuid::WithoutBraces);
        _downloaders.emplace_back(
            new Downloader{QUrl{QString::fromStdString(s->url)}, outPath, this});
        connect(_downloaders.back(),
                &Downloader::downloaded,
                this,
                [this](QString outputPath) {
                    _downloadedFiles.emplace_back(outputPath.toStdString());
                    if (_downloaders.empty()) {
                        // We're done downloading!
                        finishedAllSourceDownloads();
                    } else {
                        Downloader *front = _downloaders.front();
                        _downloaders.pop_front();
                        front->startDownload();
                    }
                });

        connect(_downloaders.back(), &Downloader::error, this, [this](int err) {
            // auto downloadResultDialog = new DownloadResultDialog{tr("Failed to download update for source"),
            // "",
            // this};
        });
    }

    for (int i = 0; i < kMaxSimultaneousDownloads; ++i) {
        if (!_downloaders.empty()) {
            Downloader *front = _downloaders.front();
            _downloaders.pop_front();
            front->startDownload();
        } else {
            break;
        }
    }
}

void SourceUpdateWindow::finishedAllSourceDownloads()
{
    // All files should now be merged into the first item
    SQLDatabaseUtils::mergeDatabases(_downloadedFiles);

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

    _dialog->setLabelText(tr("Removing old dictionary version..."));
    _dialog->setRange(0, 0);
    _dialog->setValue(0);

    disconnect(_utils.get(), nullptr, nullptr, nullptr);

    connect(_utils.get(), &SQLDatabaseUtils::deletingDefinitions, this, [&] {
        _dialog->setLabelText(tr("Removing old dictionary definitions..."));
    });

    connect(_utils.get(),
            &SQLDatabaseUtils::totalToDelete,
            this,
            [&](int numToDelete) {
                _dialog->setRange(0, numToDelete + 1);
                _dialog->setLabelText(
                    QString{tr("Deleted entry 0 of %1")}.arg(numToDelete));
            });

    connect(_utils.get(),
            &SQLDatabaseUtils::deletionProgress,
            this,
            [&](int deleted, int total) {
                _dialog->setLabelText(
                    QString{tr("Deleted entry %1 of %2")}.arg(deleted).arg(
                        total));
                _dialog->setValue(deleted);
            });

    connect(_utils.get(), &SQLDatabaseUtils::insertingSource, this, [&] {
        _dialog->setValue(0);
        _dialog->setRange(0, 0);
        _dialog->setLabelText(tr("Adding new dictionary..."));
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
            [&](bool success, QString reason, QString description) {
                _dialog->reset();
                std::vector<std::pair<std::string, std::string>> sources;
                _utils->readSources(sources);
                for (const auto &source : sources) {
                    SourceUtils::addSource(source.first, source.second);
                }

                if (!success) {
                    // failureMessage(reason, description);
                }
            });

    (void) QtConcurrent::run(&SQLDatabaseUtils::addSource,
                             _utils.get(),
                             _downloadedFiles[0],
                             /* overwriteConflictingDictionaries */ true);
}

void SourceUpdateWindow::paintWithApplicationState(Qt::ApplicationState state)
{
    (void) (state);
    setStyle(Utils::isDarkMode());
}
