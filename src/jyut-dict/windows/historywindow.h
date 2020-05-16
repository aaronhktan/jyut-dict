#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include "components/historyview/searchhistorytab.h"
#include "components/historyview/viewhistorytab.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/entry/entry.h"

#include <QEvent>
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

    void changeEvent(QEvent *event);

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QVBoxLayout *_windowLayout;
    QTabWidget *_tabWidget;

    QWidget *_searchHistoryTab;
    QWidget *_viewHistoryTab;

signals:
    void searchHistoryClicked(searchTermHistoryItem &pair);
    void viewHistoryClicked(Entry &entry);

public slots:
    void forwardSearchHistoryItem(searchTermHistoryItem &pair);
    void forwardViewHistoryItem(Entry &entry);
};

#endif // HISTORYWINDOW_H
