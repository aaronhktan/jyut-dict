#include "historywindow.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

HistoryWindow::HistoryWindow(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
{
    setObjectName("HistoryWindow");

    _searchHistoryTab = new SearchHistoryTab{sqlHistoryUtils, this};
    _viewHistoryTab = new ViewHistoryTab{sqlHistoryUtils, this};

    connect(static_cast<SearchHistoryTab *>(_searchHistoryTab),
            &SearchHistoryTab::searchHistoryClicked,
            this,
            &HistoryWindow::forwardSearchHistoryItem);

    connect(static_cast<ViewHistoryTab *>(_viewHistoryTab),
            &ViewHistoryTab::viewHistoryClicked,
            this,
            &HistoryWindow::forwardViewHistoryItem);

    setupUI();
    translateUI();
    setMinimumSize(300, 500);
    setStyle(Utils::isDarkMode());
}

void HistoryWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void HistoryWindow::setupUI(void)
{
    _tabWidget = new QTabWidget{this};
    _tabWidget->addTab(_searchHistoryTab, tr("Search History"));
    _tabWidget->addTab(_viewHistoryTab, tr("Viewed Entries"));

    _windowLayout = new QVBoxLayout{this};
    _windowLayout->addWidget(_tabWidget);
}

void HistoryWindow::translateUI(void)
{
    QString title = tr("History");
    setWindowTitle(title);
    _tabWidget->setTabText(0, tr("Search History"));
    _tabWidget->setTabText(1, tr("Viewed Entries"));
}

void HistoryWindow::setStyle(bool use_dark)
{
    (void)(use_dark);
#ifdef Q_OS_MAC
    QString styleSheet = "QWidget#HistoryWindow { "
                         "   background-color: palette(base); "
                         "   border-top: 1px solid palette(alternate-base); "
                         "} ";
#elif defined(Q_OS_LINUX)
    QString styleSheet = "QWidget#HistoryWindow { "
                         "   background-color: palette(alternate-base); "
                         "   border-top: 1px solid palette(alternate-base); "
                         "} ";
#elif defined(Q_OS_WINDOWS)
    QString styleSheet = "QWidget#HistoryWindow { "
                         "   background-color: palette(base); "
                         "   border-top: 1px solid palette(alternate-base); "
                         "} ";
#endif
    setStyleSheet(styleSheet);
    setAttribute(Qt::WA_StyledBackground);

// QTabWidget is really weird on Windows, and the -1px stylesheet is a workaround:
// See https://stackoverflow.com/questions/38369015/customuzing-qtabwidget-with-style-sheets
#if defined(Q_OS_WIN)
    QString tabStyleSheet = "QTabBar::tab { "
                            "   background-color: palette(alternate-base); "
                            "   border: 1px solid palette(base); "
                            "   padding: 7px; "
                            "} "
                            ""
                            "QTabBar::tab:selected { "
                            "   border: 0px; "
                            "   margin-bottom: -1px; "
                            "} "
                            ""
                            "QTabBar::tab:!selected { "
                            "   border: 1px solid palette(base); "
                            "   padding: 7px; "
                            "} "
                            ""
                            "QTabWidget::tab-bar { "
                            "   alignment: center; "
                            "} "
                            ""
                            "QTabWidget::pane { "
                            "   border: 1px solid palette(base); "
                            "   top: -1px; "
                            "} ";
    _tabWidget->setStyleSheet(tabStyleSheet);
#endif
}

void HistoryWindow::forwardSearchHistoryItem(const searchTermHistoryItem &pair)
{
    emit searchHistoryClicked(pair);
}

void HistoryWindow::forwardViewHistoryItem(const Entry &entry)
{
    emit viewHistoryClicked(entry);
}
