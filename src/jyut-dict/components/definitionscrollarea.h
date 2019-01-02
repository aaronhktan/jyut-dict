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

class DefinitionScrollArea : public QScrollArea
{
public:
    explicit DefinitionScrollArea(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

    void setEntry(Entry& entry);

    ~DefinitionScrollArea() override;

private:
    EntryHeaderWidget *_entryHeaderWidget;
    DefinitionWidget *_definitionWidget;

    // Entire Scroll Area
    QVBoxLayout *_scrollAreaLayout;
    QWidget *_scrollAreaWidget;

    void testEntry();
};

#endif // DEFINITIONSCROLLAREA_H
