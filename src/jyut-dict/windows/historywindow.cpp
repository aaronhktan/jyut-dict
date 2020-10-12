#include "historywindow.h"

#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#endif
#include "logic/utils/utils_qt.h"

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

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */ false);
#endif
}

void HistoryWindow::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
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
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    QColor backgroundColour = use_dark ? QColor{BACKGROUND_COLOUR_DARK_R,
                                                BACKGROUND_COLOUR_DARK_G,
                                                BACKGROUND_COLOUR_DARK_B}
                                       : QColor{BACKGROUND_COLOUR_LIGHT_R,
                                                BACKGROUND_COLOUR_LIGHT_G,
                                                BACKGROUND_COLOUR_LIGHT_B};
    QString styleSheet = "QWidget#HistoryWindow { background-color: %1; }";
    setStyleSheet(styleSheet.arg(backgroundColour.name()));
    setAttribute(Qt::WA_StyledBackground);
#else
    (void) (use_dark);
#endif
}

void HistoryWindow::forwardSearchHistoryItem(searchTermHistoryItem &pair)
{
    emit searchHistoryClicked(pair);
}

void HistoryWindow::forwardViewHistoryItem(Entry &entry)
{
    emit viewHistoryClicked(entry);
}
