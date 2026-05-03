#ifndef SEARCHHISTORYTAB_H
#define SEARCHHISTORYTAB_H

#include "logic/database/sqluserhistoryutils.h"

#include <QWidget>

class SearchHistoryListModel;
class SearchHistoryListView;

class QEvent;
class QPushButton;
class QVBoxLayout;

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

    void handleClick(const QModelIndex &selection);

    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;

    bool _paletteRecentlyChanged = false;

    QVBoxLayout *_tabLayout;

    SearchHistoryListView *_listView;
    SearchHistoryListModel *_model;

    QPushButton *_clearAllSearchHistoryButton;

signals:
    void searchHistoryClicked(const SearchTermHistoryItem &pair);

public slots:
    void updateStyleRequested(void);
};

#endif // SEARCHHISTORYTAB_H
