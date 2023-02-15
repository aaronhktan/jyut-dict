#ifndef ENTRYACTIONWIDGET_H
#define ENTRYACTIONWIDGET_H

#include "components/layouts/flowlayout.h"

#include "logic/database/sqluserdatautils.h"
#include "logic/entry/entry.h"
#include "logic/search/isearchobserver.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QWidget>

// The EntryActionWidget provides some actions related to the EntryView.
// This includes saving the entry to a list of saved words, and
// exporting an image of the entry.

class EntryActionWidget : public QWidget,
                          public ISearchObserver
{
    Q_OBJECT
public:
    explicit EntryActionWidget(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                               QWidget *parent = nullptr);
    ~EntryActionWidget(void) override;

    void callback(bool entryExists, const Entry &entry) override;

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void refreshBookmarkButton(void);
    void addEntryToFavourites(const Entry &entry);
    void removeEntryFromFavourites(const Entry &entry);

    void shareAction(void);

    bool _paletteRecentlyChanged = false;

    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    std::unique_ptr<QSettings> _settings;
    Entry _entry;

    FlowLayout *_layout;
    QPushButton *_bookmarkButton;
    QPushButton *_shareButton;
    QPushButton *_openInNewWindowButton;

signals:
    void callbackTriggered(bool entryExists, const Entry &entry);
    void openInNewWindowAction(void);

public slots:
};

#endif // ENTRYACTIONWIDGET_H
