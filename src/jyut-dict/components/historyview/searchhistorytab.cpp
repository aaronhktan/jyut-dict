#include "searchhistorytab.h"

SearchHistoryTab::SearchHistoryTab(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _listView = new SearchHistoryListView{this};

    _model = new SearchHistoryListModel{_sqlHistoryUtils, this};
    _listView->setModel(_model);

    _clearAllSearchHistoryButton = new QPushButton{this};
    connect(_clearAllSearchHistoryButton, &QPushButton::clicked, this, [=]() {
        _sqlHistoryUtils->clearAllSearchHistory();
    });

    _tabLayout = new QVBoxLayout{this};
    _tabLayout->addWidget(_listView);
    _tabLayout->addWidget(_clearAllSearchHistoryButton);
    _tabLayout->setAlignment(_clearAllSearchHistoryButton, Qt::AlignHCenter);

    _sqlHistoryUtils->searchAllSearchHistory();
    translateUI();
}

void SearchHistoryTab::translateUI(void)
{
    _clearAllSearchHistoryButton->setText(tr("Clear All Search History"));
    _clearAllSearchHistoryButton->setFixedSize(
        _clearAllSearchHistoryButton->minimumSizeHint());
}
