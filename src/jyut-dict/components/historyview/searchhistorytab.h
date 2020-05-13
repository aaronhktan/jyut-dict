#ifndef SEARCHHISTORYTAB_H
#define SEARCHHISTORYTAB_H

#include "components/historyview/searchhistorylistmodel.h"
#include "components/historyview/searchhistorylistview.h"
#include "logic/database/sqluserhistoryutils.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class SearchHistoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit SearchHistoryTab(
        std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils,
        QWidget *parent = nullptr);

private:
    void translateUI(void);

    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;

    QVBoxLayout *_tabLayout;

    SearchHistoryListView *_listView;
    SearchHistoryListModel *_model;

    QPushButton *_clearAllSearchHistoryButton;
};

#endif // SEARCHHISTORYTAB_H
