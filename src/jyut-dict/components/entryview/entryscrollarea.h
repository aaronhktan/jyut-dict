#ifndef ENTRYSCROLLAREA_H
#define ENTRYSCROLLAREA_H

#include "components/entryview/entryscrollareawidget.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"

#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

// The EntryScrollArea is the "detail" view
// It displays an Entry object in the user interface

// The layout of the EntryScrollArea is:
// EntryHeader - shows word in chinese, pronunciation guides
// EntryContentWidget - contains all the definitions and sentences related
//                      to that entry.

class EntryScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit EntryScrollArea(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                             std::shared_ptr<SQLDatabaseManager> manager,
                             QWidget *parent = nullptr);

    void setEntry(const Entry &entry);
private:
    void resizeEvent(QResizeEvent *event) override;

    QTimer *_enableUIUpdateTimer;
    QTimer *_updateUITimer;
    // Unlike EntryViewSentenceCardSection, this is set to true by default
    // because there are situations where setEntry() is called _without_
    // the stallEntryUIUpdate slot being called (which would set this variable
    // to true), such as the Favourites window.
    bool _enableUIUpdate = true;

    EntryScrollAreaWidget *_scrollAreaWidget;

public slots:
    void stallEntryUIUpdate(void);

signals:
    void stallSentenceUIUpdate(void);
};

#endif // ENTRYSCROLLAREA_H
