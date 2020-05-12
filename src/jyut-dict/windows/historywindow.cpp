#include "historywindow.h"

HistoryWindow::HistoryWindow(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
{
    _searchHistoryTab = new SearchHistoryTab{sqlHistoryUtils, this};

    _tabWidget = new QTabWidget{this};
    _tabWidget->addTab(_searchHistoryTab, "Search History");

    _windowLayout = new QVBoxLayout{this};
    _windowLayout->addWidget(_tabWidget);

    setFixedSize(200, 300);
}
