#ifndef ENTRYACTIONWIDGET_H
#define ENTRYACTIONWIDGET_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/database/sqluserdatautils.h"
#include "logic/entry/entry.h"
#include "logic/search/isearchobserver.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

class EntryActionWidget : public QWidget,
                          public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryActionWidget(std::shared_ptr<SQLDatabaseManager> manager,
                               QWidget *parent = nullptr);

    void callback(bool entryExists) override;

    void changeEvent(QEvent *event) override;

    void setEntry(Entry entry);

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void refreshBookmarkButton(void);
    void addEntryToFavourites(Entry entry);
    void removeEntryFromFavourites(Entry entry);

    void shareAction(void);

    bool _paletteRecentlyChanged = false;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<SQLUserDataUtils> _utils;
    Entry _entry;

    QHBoxLayout *_layout;
    QPushButton *_bookmarkButton;
    QPushButton *_shareButton;
    QPushButton *_openFavouritesButton;

signals:
    void callbackTriggered(bool entryExists);

public slots:
};

#endif // ENTRYACTIONWIDGET_H
