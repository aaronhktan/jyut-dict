#include "sourceupdatewindow.h"

#include "components/sourceupdatelist/sourceupdatemodel.h"
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
#include <QPushButton>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

#include <ranges>

namespace {
constexpr auto kWindowWidth = 600;
constexpr auto kWindowHeight = 400;
} // namespace

SourceUpdateWindow::SourceUpdateWindow(QWidget *parent)
    : QWidget{parent}
    , _model{new SourceUpdateModel{this}}
    , _widget{new QWidget{this}}
    , _settings{Settings::getSettings()}
{
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

    _widget = new QWidget(this);
    _tableView = new QTableView(_widget);
    _toggleAllButton = new QPushButton(_widget);
    _okButton = new QPushButton(_widget);

    _tableView->setModel(_model);
    _tableView->setAlternatingRowColors(true);
    _tableView->setSelectionMode(QAbstractItemView::NoSelection);
    _tableView->setFocusPolicy(Qt::NoFocus);
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
                               QHeaderView::ResizeToContents);
    _tableView->verticalHeader()->setVisible(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(_toggleAllButton);
    buttonLayout->addWidget(_okButton);

    QVBoxLayout *widgetLayout = new QVBoxLayout{_widget};
    widgetLayout->addWidget(_tableView);
    widgetLayout->addLayout(buttonLayout);

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

    // Set background color of tabs in toolbar
    QColor selectedBackgroundColour;
    QColor currentTextColour;
    QColor otherTextColour;
    if (QGuiApplication::applicationState() == Qt::ApplicationInactive) {
        selectedBackgroundColour
            = QGuiApplication::palette().color(QPalette::Inactive,
                                               QPalette::Highlight);
        currentTextColour = use_dark
                                ? QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_R,
                                         TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_G,
                                         TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_B}
                                : QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_R,
                                         TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_G,
                                         TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_B};
        otherTextColour = use_dark
                              ? QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_R,
                                       TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_G,
                                       TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_B}
                              : QColor{TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_R,
                                       TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_G,
                                       TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_B};
    } else {
#ifdef Q_OS_MAC
        selectedBackgroundColour = Utils::getAppleControlAccentColor();
#else
        selectedBackgroundColour = use_dark
                                       ? QColor{LIST_ITEM_ACTIVE_COLOUR_DARK_R,
                                                LIST_ITEM_ACTIVE_COLOUR_DARK_G,
                                                LIST_ITEM_ACTIVE_COLOUR_DARK_B}
                                       : QColor{LIST_ITEM_ACTIVE_COLOUR_LIGHT_R,
                                                LIST_ITEM_ACTIVE_COLOUR_LIGHT_G,
                                                LIST_ITEM_ACTIVE_COLOUR_LIGHT_B};
#endif
        currentTextColour = Utils::getContrastingColour(
            selectedBackgroundColour);
#ifdef Q_OS_MAC
        otherTextColour = QGuiApplication::palette().color(QPalette::Active,
                                                           QPalette::Text);
#else
        otherTextColour = use_dark
                              ? QColor{TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_R,
                                       TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_G,
                                       TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_B}
                              : QColor{TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_R,
                                       TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_G,
                                       TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_B};
#endif
    }

#ifdef Q_OS_MAC
    QString style{"QLabel[isHan=\"true\"] { "
                  "   font-size: %6px; "
                  "} "
                  " "
                  "QLabel { "
                  "   font-size: %7px; "
                  "} "
                  " "
                  "QCheckBox[isHan=\"true\"] { "
                  "   font-size: %6px; "
                  "} "
                  " "
                  "QCheckBox { "
                  "   font-size: %7px; "
                  "} "
                  " "
                  "QPushButton[isHan=\"true\"] { "
                  "   font-size: %6px; "
                  //// QPushButton falls back to Fusion style on macOS when the
                  //// height exceeds 16px. Set the maximum size to 16px.
                  "   height: 16px; "
                  "} "
                  " "
                  "QPushButton { "
                  "   font-size: %7px; "
                  "   height: 16px; "
                  "} "};
#else
    QString style{"QLabel[isHan=\"true\"] { "
                  "   font-size: %6px; "
                  "} "
                  " "
                  "QLabel { "
                  "   font-size: %7px; "
                  "} "
                  " "
                  "QCheckBox[isHan=\"true\"] { "
                  "   font-size: %6px; "
                  "} "
                  " "
                  "QCheckBox { "
                  "   font-size: %7px; "
                  "} "
                  " "
                  "QPushButton[isHan=\"true\"] { "
                  "   font-size: %6px; "
                  "   height: 16px; "
                  "} "
                  " "
                  "QPushButton { "
                  "   font-size: %7px; "
                  "   height: 16px; "
                  "} "};
#endif
    setStyleSheet(style.arg(selectedBackgroundColour.name(),
                            std::to_string(uiFontSizeHan).c_str(),
                            std::to_string(uiFontSize).c_str(),
                            currentTextColour.name(),
                            otherTextColour.name(),
                            std::to_string(bodyFontSizeHan).c_str(),
                            std::to_string(bodyFontSize).c_str()));
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
