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
    explicit EntryActionWidget(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                               QWidget *parent = nullptr);
    ~EntryActionWidget(void) override;

    void callback(bool entryExists, Entry entry) override;

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

    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    Entry _entry;

    QHBoxLayout *_layout;
    QPushButton *_bookmarkButton;
    QPushButton *_shareButton;

signals:
    void callbackTriggered(bool entryExists, Entry entry);

public slots:
};

#endif // ENTRYACTIONWIDGET_H
