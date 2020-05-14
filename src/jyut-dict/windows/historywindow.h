#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include "components/historyview/searchhistorytab.h"
#include "components/historyview/viewhistorytab.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/entry/entry.h"

#include <QVBoxLayout>
#include <QTabWidget>

#include <string>
#include <utility>

// The HistoryWindow contains a TabWidget, allowing a user to view
// history of past searches and history of viewed entries.

class HistoryWindow : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryWindow(std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                           QWidget *parent = nullptr);

private:
    QVBoxLayout *_windowLayout;
    QTabWidget *_tabWidget;

    QWidget *_searchHistoryTab;
    QWidget *_viewHistoryTab;

signals:
    void searchHistoryClicked(searchTermHistoryItem &pair);

public slots:
    void forwardSearchTermItem(searchTermHistoryItem &pair);
};

#endif // HISTORYWINDOW_H
