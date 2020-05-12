#ifndef SEARCHHISTORYTAB_H
#define SEARCHHISTORYTAB_H

#include "components/historyview/searchhistorylistmodel.h"
#include "components/historyview/searchhistorylistview.h"
#include "logic/database/sqluserhistoryutils.h"

#include <QWidget>

class SearchHistoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit SearchHistoryTab(
        std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils,
        QWidget *parent = nullptr);

private:
    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;

    SearchHistoryListView *_listView;
    SearchHistoryListModel *_model;
};

#endif // SEARCHHISTORYTAB_H
