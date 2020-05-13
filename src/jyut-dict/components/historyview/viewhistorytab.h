#ifndef VIEWHISTORYTAB_H
#define VIEWHISTORYTAB_H

#include "components/historyview/viewhistorylistmodel.h"
#include "components/historyview/viewhistorylistview.h"
#include "logic/database/sqluserhistoryutils.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class ViewHistoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit ViewHistoryTab(
        std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils,
        QWidget *parent = nullptr);

private:
    void translateUI(void);

    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;

    QVBoxLayout *_tabLayout;

    ViewHistoryListView *_listView;
    ViewHistoryListModel *_model;

    QPushButton *_clearAllViewHistoryButton;
};

#endif // VIEWHISTORYTAB_H
