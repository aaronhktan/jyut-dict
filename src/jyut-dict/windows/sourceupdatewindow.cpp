#include "sourceupdatewindow.h"

#include "components/sourceupdatelist/sourceupdatemodel.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqldatabaseutils.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QAbstractItemView>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

#include <ranges>

namespace {
constexpr auto kWindowWidth = 600;
constexpr auto kWindowHeight = 400;

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

        QPainter painter(viewport());
        const QColor lineColor = palette().color(QPalette::Base);
        const QRect clip = event->rect();

        // Draw vertical separators at the same x positions the header uses.
        for (int col = 0; col < model()->columnCount() - 1; ++col) {
            if (isColumnHidden(col)) {
                continue;
            }

            const int x = columnViewportPosition(col) + columnWidth(col) - 1;
            painter.fillRect(QRect{x, clip.top(), 1, clip.height()}, lineColor);
        }

        // Draw horizontal row separators too.
        int firstRow = rowAt(clip.top());
        if (firstRow < 0) {
            firstRow = 0;
        }

        int lastRow = rowAt(clip.bottom());
        if (lastRow < 0) {
            lastRow = model()->rowCount() - 1;
        }

        for (int row = firstRow; row < lastRow; ++row) {
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
    std::vector<IUpdateChecker::SourceUpdateAvailability> &a,
    std::shared_ptr<SQLDatabaseManager> manager,
    QWidget *parent)
    : QWidget{parent, Qt::Window}
    , _manager{manager}
    , _utils{new SQLDatabaseUtils{manager}}
    , _settings{Settings::getSettings()}
{
    // Get list of existing sources
    std::vector<DictionaryMetadata> sources;
    _utils->readSources(sources);
    std::unordered_map<std::string, DictionaryMetadata> sourceMetadata;
    for (const auto &s : sources) {
        sourceMetadata[s.getName()] = s;
    }

    // Find intersection of existing sources + ones that have update available
    std::vector<SourceUpdateModel::MetadataWrapper> updateMetadata;
    for (const auto &s : a) {
        const std::string &name = s.sourceName;
        updateMetadata.emplace_back(sourceMetadata.at(name),
                                    s.versionNumber,
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
    _tableView = new SourceUpdateTableView{_widget};
    _toggleAllButton = new QPushButton{_widget};
    _okButton = new QPushButton{_widget};

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

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addWidget(_toggleAllButton);
    buttonLayout->addWidget(_okButton);

    QVBoxLayout *widgetLayout = new QVBoxLayout{_widget};
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->addWidget(_tableView);
    widgetLayout->addLayout(buttonLayout);

    _layout = new QVBoxLayout{this};
    _layout->setSpacing(5);
    _layout->addWidget(_widget);

    connect(_tableView,
            &QTableView::clicked,
            this,
            &SourceUpdateWindow::toggleRowCheckState);
    connect(_toggleAllButton,
            &QPushButton::clicked,
            this,
            &SourceUpdateWindow::toggleAllRows);
    connect(_okButton,
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
    updateToggleAllButtonText();
    _okButton->setText(tr("OK"));
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
    // TODO: implement
}

void SourceUpdateWindow::paintWithApplicationState(Qt::ApplicationState state)
{
    (void) (state);
    setStyle(Utils::isDarkMode());
}
