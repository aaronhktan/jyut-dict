#include "entryscrollarea.h"

#include "components/entryview/entryscrollareawidget.h"
#include "logic/entry/definitionsset.h"
#include "logic/entry/entry.h"
#include "logic/sentence/sentenceset.h"

#include <QScrollBar>

#define ENTIRE_WIDTH -1

EntryScrollArea::EntryScrollArea(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                                 std::shared_ptr<SQLDatabaseManager> manager,
                                 QWidget *parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    _scrollAreaWidget = new EntryScrollAreaWidget{sqlUserUtils, manager, this};

    setWidget(_scrollAreaWidget);
    setWidgetResizable(true); // IMPORTANT! This makes the scrolling widget resize correctly.
#ifdef Q_OS_LINUX
    setMinimumWidth(250);
#else
    setMinimumWidth(350);
#endif

    connect(this,
            &EntryScrollArea::forwardSearchBarTextChange,
            _scrollAreaWidget,
            &EntryScrollAreaWidget::forwardSearchBarTextChange);

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
    _scrollAreaWidget->setVisible(false);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
    _scrollAreaWidget->setVisible(true);
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
