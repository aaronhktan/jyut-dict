#ifndef SEARCHHISTORYTAB_H
#define SEARCHHISTORYTAB_H

#include "components/historyview/searchhistorylistmodel.h"
#include "components/historyview/searchhistorylistview.h"
#include "logic/database/sqluserhistoryutils.h"

#include <QEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

// The SearchHistoryTab is a widget that contains the SearchHistoryListView
// as well as a button that allows the user to clear the search history

class SearchHistoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit SearchHistoryTab(
        std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils,
        QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;

    QVBoxLayout *_tabLayout;

    SearchHistoryListView *_listView;
    SearchHistoryListModel *_model;

    QPushButton *_clearAllSearchHistoryButton;
};

#endif // SEARCHHISTORYTAB_H
