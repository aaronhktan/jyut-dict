#ifndef DEFINITIONSCROLLAREA_H
#define DEFINITIONSCROLLAREA_H

#include "components/definitionscrollareawidget.h"
#include "logic/entry/entry.h"

#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

// The DefinitionScrollArea is the "detail" view
// It displays an Entry object in the user interface

// The layout of the DefinitionScrollArea is:
// EntryHeader - shows word in chinese, pronunciation guides
// Definition Widget - contains all the definitions and headers for each source

class DefinitionScrollArea : public QScrollArea
{
public:
    explicit DefinitionScrollArea(QWidget *parent = nullptr);
    ~DefinitionScrollArea() override;

    void setEntry(const Entry &entry);
private:
    void resizeEvent(QResizeEvent *event) override;

    // Widget that contains elements to scroll
    DefinitionScrollAreaWidget *_scrollAreaWidget;
};

#endif // DEFINITIONSCROLLAREA_H
