#include "entryscrollarea.h"

#include "components/entryview/entryscrollareawidget.h"
#include "logic/entry/definitionsset.h"
#include "logic/entry/entry.h"
#include "logic/entry/sentence.h"

#include <QScrollBar>

#define ENTIRE_WIDTH -1

EntryScrollArea::EntryScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    _scrollAreaWidget = new EntryScrollAreaWidget{this};

    setWidget(_scrollAreaWidget);
    setWidgetResizable(true); // IMPORTANT! This makes the scrolling widget resize correctly.
#ifdef Q_OS_LINUX
    setMinimumWidth(250);
#else
    setMinimumWidth(350);
#endif

    if (!parent) {
        setMinimumHeight(400);
    }
}

EntryScrollArea::~EntryScrollArea()
{
}

void EntryScrollArea::setEntry(const Entry &entry)
{
    _scrollAreaWidget->setEntry(entry);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}

void EntryScrollArea::resizeEvent(QResizeEvent *event)
{
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
    event->accept();
}
