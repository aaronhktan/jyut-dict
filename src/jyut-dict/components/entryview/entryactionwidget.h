#ifndef ENTRYACTIONWIDGET_H
#define ENTRYACTIONWIDGET_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserdatautils.h"
#include "logic/entry/entry.h"
#include "logic/search/isearchobserver.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QWidget>

class EntryActionWidget : public QWidget,
                          public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryActionWidget(std::shared_ptr<SQLDatabaseManager> manager,
                               QWidget *parent = nullptr);

    void callback(const std::vector<Entry> entries, bool emptyQuery) override;
    void callback(bool entryExists) override;

    void setEntry(Entry entry);

private:
    void setupUI(void);

    void addEntryToFavourites(Entry entry);

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLUserDataUtils> _utils;
    Entry _entry;

    QHBoxLayout *_layout;
    QToolButton *_bookmarkButton;

signals:

public slots:
};

#endif // ENTRYACTIONWIDGET_H
