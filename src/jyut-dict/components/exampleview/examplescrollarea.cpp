#include "examplescrollarea.h"

#include "components/exampleview/examplescrollareawidget.h"
#include "logic/example/example.h"

#include <QResizeEvent>
#include <QScrollBar>
#include <QVBoxLayout>

ExampleScrollArea::ExampleScrollArea(QWidget *parent)
    : QScrollArea{parent}
    , _scrollAreaWidget{new ExampleScrollAreaWidget{this}}
{
    setFrameShape(QFrame::NoFrame);

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

void ExampleScrollArea::setExample(const Example &example)
{
    _scrollAreaWidget->setExample(example);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}

void ExampleScrollArea::resizeEvent(QResizeEvent *event)
{
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              event->size().height());
    event->accept();
}

void ExampleScrollArea::updateStyleRequested(void)
{
    _scrollAreaWidget->updateStyleRequested();
}
