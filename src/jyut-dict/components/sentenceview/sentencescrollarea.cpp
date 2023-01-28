#include "sentencescrollarea.h"

#include <QScrollBar>

SentenceScrollArea::SentenceScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    _scrollAreaWidget = new SentenceScrollAreaWidget{this};

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

void SentenceScrollArea::setSourceSentence(const SourceSentence &sentence)
{
    _scrollAreaWidget->setSourceSentence(sentence);
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              _scrollAreaWidget->sizeHint().height());
}

void SentenceScrollArea::resizeEvent(QResizeEvent *event)
{
    _scrollAreaWidget->resize(width()
                                  - (verticalScrollBar()->isVisible()
                                         ? verticalScrollBar()->width()
                                         : 0),
                              event->size().height());
    event->accept();
}

void SentenceScrollArea::updateStyleRequested(void)
{
    _scrollAreaWidget->updateStyleRequested();
}
