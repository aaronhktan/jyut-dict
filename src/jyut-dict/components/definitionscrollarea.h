#ifndef DEFINITIONSCROLLAREA_H
#define DEFINITIONSCROLLAREA_H

#include "components/definitionwidget.h"
#include "components/entryheaderwidget.h"
#include "logic/entry/entry.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>

// The DefinitionScrollArea is the "detail" view
// It displays an Entry object in the user interface

// The layout of the DefinitionScrollArea is:
// EntryHeader - shows word in chinese, pronunciation guides
// Definition Widget - contains all the definitions and headers for each source

class DefinitionScrollArea : public QScrollArea
{
public:
    explicit DefinitionScrollArea(QWidget *parent = nullptr);

    void setEntry(Entry& entry);

    ~DefinitionScrollArea() override;

private:
    void resizeEvent(QResizeEvent *event) override;

    EntryHeaderWidget *_entryHeaderWidget;
    DefinitionWidget *_definitionWidget;

    // Entire Scroll Area
    QVBoxLayout *_scrollAreaLayout;
    QWidget *_scrollAreaWidget;

    void testEntry();
};

#endif // DEFINITIONSCROLLAREA_H
