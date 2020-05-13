#include "historywindow.h"

HistoryWindow::HistoryWindow(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
{
    _searchHistoryTab = new SearchHistoryTab{sqlHistoryUtils, this};
    _viewHistoryTab = new ViewHistoryTab{sqlHistoryUtils, this};

    _tabWidget = new QTabWidget{this};
    _tabWidget->addTab(_searchHistoryTab, tr("Search History"));
    _tabWidget->addTab(_viewHistoryTab, tr("Viewed Entries"));

    _windowLayout = new QVBoxLayout{this};
    _windowLayout->addWidget(_tabWidget);

    setMinimumSize(300, 500);
    _viewHistoryTab->adjustSize();
}
