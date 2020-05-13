#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include "components/historyview/searchhistorytab.h"
#include "components/historyview/viewhistorytab.h"
#include "logic/database/sqluserhistoryutils.h"

#include <QVBoxLayout>
#include <QTabWidget>

class HistoryWindow : public QWidget
{
public:
    explicit HistoryWindow(std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                           QWidget *parent = nullptr);

private:
    QVBoxLayout *_windowLayout;
    QTabWidget *_tabWidget;

    QWidget *_searchHistoryTab;
    QWidget *_viewHistoryTab;
};

#endif // HISTORYWINDOW_H