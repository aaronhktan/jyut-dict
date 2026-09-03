#include "sentencescrollarea.h"

#include "components/sentenceview/sentencescrollareawidget.h"
#include "logic/sentence/sourcesentence.h"

#include <QResizeEvent>
#include <QScrollBar>
#include <QVBoxLayout>

SentenceScrollArea::SentenceScrollArea(QWidget *parent)
    : QScrollArea{parent}
    , _scrollAreaWidget{new SentenceScrollAreaWidget{this}}
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
