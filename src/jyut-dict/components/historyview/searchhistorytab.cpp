#include "searchhistorytab.h"

SearchHistoryTab::SearchHistoryTab(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _listView = new SearchHistoryListView{this};

    _model = new SearchHistoryListModel{_sqlHistoryUtils, this};
    _listView->setModel(_model);

    _sqlHistoryUtils->searchAllSearchHistory();
    setMinimumSize(200, 300);
    _listView->setFixedSize(size());
}
