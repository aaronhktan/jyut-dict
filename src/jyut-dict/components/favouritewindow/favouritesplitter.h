#ifndef FAVOURITESPLITTER_H
#define FAVOURITESPLITTER_H

#include "components/entryview/entryscrollarea.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserdatautils.h"
#include "logic/entry/entry.h"

#include <QAbstractListModel>
#include <QEvent>
#include <QModelIndex>
#include <QSplitter>
#include <QWidget>

// The FavouriteSplitter contains a "master" listview and a "detail" scrollarea
//
// It handles the model changed signal that the master listview emits,
// and passes the data to the detail scrollarea.
//
// It also subscribes to the sqlUserUtils to provide updates to the scroll area
// on the status of whether the current entry is saved.

class FavouriteSplitter : public QSplitter
{
Q_OBJECT

public:
    explicit FavouriteSplitter(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                               std::shared_ptr<SQLDatabaseManager> manager,
                               QWidget *parent = nullptr);
    ~FavouriteSplitter() override;

    void changeEvent(QEvent *event) override;

    void openCurrentSelectionInNewWindow(void);

private:
    void setupUI();
    void translateUI();

#ifdef Q_OS_WIN
    void setStyle(bool use_dark);
    bool _paletteRecentlyChanged = false;
#endif

    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    std::shared_ptr<SQLDatabaseManager> _manager;

    QAbstractListModel *_model;
    EntryScrollArea *_entryScrollArea;
    QListView *_resultListView;

private slots:
    void prepareEntry(Entry &entry);

    void handleClick(const QModelIndex &selection);
    void handleDoubleClick(const QModelIndex &selection);
};

#endif // FAVOURITESPLITTER_H
