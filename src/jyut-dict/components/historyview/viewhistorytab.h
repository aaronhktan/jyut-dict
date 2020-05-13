#ifndef VIEWHISTORYTAB_H
#define VIEWHISTORYTAB_H

#include "components/historyview/viewhistorylistmodel.h"
#include "components/historyview/viewhistorylistview.h"
#include "logic/database/sqluserhistoryutils.h"

#include <QEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

// The ViewHistoryTab is a widget that contains the ViewHistoryListView
// as well as a button that allows the user to clear the viewed entry history

class ViewHistoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit ViewHistoryTab(std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils,
                            QWidget *parent = nullptr);

    void changeEvent(QEvent *event);

private:
    void setupUI(void);
    void translateUI(void);

    void setStyle(bool use_dark);

    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;

    QVBoxLayout *_tabLayout;

    ViewHistoryListView *_listView;
    ViewHistoryListModel *_model;

    QPushButton *_clearAllViewHistoryButton;
};

#endif // VIEWHISTORYTAB_H
