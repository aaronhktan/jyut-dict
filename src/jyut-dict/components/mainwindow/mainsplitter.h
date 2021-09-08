#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include "components/entryview/entryscrollarea.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserdatautils.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/entry/entry.h"
#include "logic/search/sqlsearch.h"

#include <QAbstractListModel>
#include <QEvent>
#include <QModelIndex>
#include <QSplitter>
#include <QTimer>
#include <QWidget>

// The MainSplitter contains a "master" listview and a "detail" scrollarea
//
// It handles the model changed signal that the master listview emits,
// and passes the data to the detail scrollarea.

class MainSplitter : public QSplitter
{
Q_OBJECT

public:
    explicit MainSplitter(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                          std::shared_ptr<SQLDatabaseManager> manager,
                          std::shared_ptr<SQLSearch> sqlSearch,
                          std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                          QWidget *parent = nullptr);
    ~MainSplitter() override;

    void changeEvent(QEvent *event) override;

    void setFocusToResults(void);
    void openCurrentSelectionInNewWindow(void);

private:
    void translateUI(void);

    void setStyle(bool use_dark);
    bool _paletteRecentlyChanged = false;

    void prepareEntry(Entry &entry, bool addToHistory);

    bool _addToHistory = true;

    QTimer *_addToHistoryTimer;

    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    std::shared_ptr<SQLDatabaseManager> _manager;
    std::shared_ptr<SQLSearch> _search;
    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;

    EntryScrollArea *_entryScrollArea;
    QAbstractListModel *_model;
    QListView *_resultListView;

public slots:
    void forwardViewHistoryItem(Entry &entry);

private slots:
    void prepareEntry(Entry &entry);

    void handleModelReset(void);
    void handleClick(const QModelIndex &selection);
    void handleDoubleClick(const QModelIndex &selection);

signals:
    void forwardSearchBarTextChange(void);
};

#endif // MAINSPLITTER_H
