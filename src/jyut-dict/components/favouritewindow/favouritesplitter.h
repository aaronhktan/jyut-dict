#ifndef FAVOURITESPLITTER_H
#define FAVOURITESPLITTER_H

#include "components/entryview/entryscrollarea.h"
#include "logic/analytics/analytics.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserdatautils.h"
#include "logic/entry/entry.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QSplitter>
#include <QWidget>

// The MainSplitter contains a "master" listview and a "detail" scrollarea
//
// It handles the model changed signal that the master listview emits,
// and passes the data to the detail scrollarea.

class FavouriteSplitter : public QSplitter
{
Q_OBJECT

public:
    explicit FavouriteSplitter(std::shared_ptr<SQLDatabaseManager> manager,
                               QWidget *parent = nullptr);
    ~FavouriteSplitter() override;

    void openCurrentSelectionInNewWindow(void);

private:
    std::shared_ptr<SQLUserDataUtils> _utils;
    std::shared_ptr<SQLDatabaseManager> _manager;
    Analytics *_analytics;

    QAbstractListModel *_model;
    EntryScrollArea *_entryScrollArea;
    QListView *_resultListView;

private slots:
    void prepareEntry(Entry &entry);

    void handleClick(const QModelIndex &selection);
    void handleDoubleClick(const QModelIndex &selection);
};

#endif // FAVOURITESPLITTER_H
