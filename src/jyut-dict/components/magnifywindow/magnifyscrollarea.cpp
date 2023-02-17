#include "magnifyscrollarea.h"

#include "logic/entry/entry.h"

#include <QCoreApplication>
#include <QScrollBar>

#include <QDebug>
MagnifyScrollArea::MagnifyScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    _scrollAreaWidget = new MagnifyScrollAreaWidget{this};

    setWidget(_scrollAreaWidget);
    setWidgetResizable(
        true); // IMPORTANT! This makes the scrolling widget resize correctly.

    setMinimumSize(QSize{500, 500});
}

void MagnifyScrollArea::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void MagnifyScrollArea::setEntry(const Entry &entry)
{
    _scrollAreaWidget->setEntry(entry);
    _scrollAreaWidget->setMaximumWidth(
        width()
        - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0));
}

void MagnifyScrollArea::resizeEvent(QResizeEvent *event)
{
    event->accept();
    _scrollAreaWidget->setMaximumWidth(
        width()
        - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0));
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}

void MagnifyScrollArea::updateStyleRequested(void)
{
    QEvent event{QEvent::PaletteChange};
    QCoreApplication::sendEvent(_scrollAreaWidget, &event);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}
