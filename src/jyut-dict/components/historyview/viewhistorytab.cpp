#include "viewhistorytab.h"

#include <QSizePolicy>

ViewHistoryTab::ViewHistoryTab(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _listView = new ViewHistoryListView{this};

    _model = new ViewHistoryListModel{_sqlHistoryUtils, this};
    _listView->setModel(_model);

    _clearAllViewHistoryButton = new QPushButton{this};
    connect(_clearAllViewHistoryButton, &QPushButton::clicked, this, [=]() {
        _sqlHistoryUtils->clearAllViewHistory();
    });

    _tabLayout = new QVBoxLayout{this};
    _tabLayout->addWidget(_listView);
    _tabLayout->addWidget(_clearAllViewHistoryButton);
    _tabLayout->setAlignment(_clearAllViewHistoryButton, Qt::AlignHCenter);

    _sqlHistoryUtils->searchAllViewHistory();
    translateUI();
}

void ViewHistoryTab::translateUI(void)
{
    _clearAllViewHistoryButton->setText(tr("Clear All View History"));
    _clearAllViewHistoryButton->setFixedSize(
        _clearAllViewHistoryButton->minimumSizeHint());
}
