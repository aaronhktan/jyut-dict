#ifndef FAVOURITESPLITTER_H
#define FAVOURITESPLITTER_H

#include "logic/search/searchparameters.h"

#include <QSplitter>

class Entry;
class EntryScrollArea;
class SQLUserDataUtils;
class SQLDatabaseManager;

class QAbstractListModel;
class QEvent;
class QListView;
class QModelIndex;

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

    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void openCurrentSelectionInNewWindow(void);

private:
    void setupUI();
    void translateUI();

    void prepareEntry(Entry &entry) const;

#ifdef Q_OS_WIN
    void setStyle(bool use_dark);
    bool _paletteRecentlyChanged = false;
#endif

    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    std::shared_ptr<SQLDatabaseManager> _manager;

    QAbstractListModel *_model;
    EntryScrollArea *_entryScrollArea;
    QListView *_resultListView;

signals:
    void searchQuery(const QString &query, const SearchParameters &parameters);

public slots:
    void updateStyleRequested();

    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters);

private slots:
    void prepareEntry(Entry &entry);

    void handleClick(const QModelIndex &selection);
    void handleDoubleClick(const QModelIndex &selection);
};

#endif // FAVOURITESPLITTER_H
