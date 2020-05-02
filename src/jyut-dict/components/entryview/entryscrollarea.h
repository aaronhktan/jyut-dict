#ifndef ENTRYSCROLLAREA_H
#define ENTRYSCROLLAREA_H

#include "components/entryview/entryscrollareawidget.h"
#include "logic/entry/entry.h"

#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

// The EntryScrollArea is the "detail" view
// It displays an Entry object in the user interface

// The layout of the EntryScrollArea is:
// EntryHeader - shows word in chinese, pronunciation guides
// EntryContentWidget - contains all the definitions and headers for each source

class EntryScrollArea : public QScrollArea
{
public:
    explicit EntryScrollArea(QWidget *parent = nullptr);
    ~EntryScrollArea() override;

    void setEntry(const Entry &entry);
private:
    void resizeEvent(QResizeEvent *event) override;

    // Widget that contains elements to scroll
    EntryScrollAreaWidget *_scrollAreaWidget;
};

#endif // ENTRYSCROLLAREA_H
