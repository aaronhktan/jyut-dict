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
public:
    explicit EntryScrollArea(std::shared_ptr<SQLDatabaseManager> manager,
                             QWidget *parent = nullptr);
    ~EntryScrollArea() override;

    void setEntry(const Entry &entry);
private:
    void resizeEvent(QResizeEvent *event) override;

    EntryScrollAreaWidget *_scrollAreaWidget;
};

#endif // ENTRYSCROLLAREA_H
