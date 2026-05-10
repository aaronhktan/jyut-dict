#include "sentenceresultlistview.h"

#include "components/sentencesearchresult/sentenceresultlistdelegate.h"

#include <QEvent>
#include <QGuiApplication>
#include <QStyledItemDelegate>

#ifdef Q_OS_WIN
#include <QScrollBar>
#include <QWheelEvent>
#endif

SentenceResultListView::SentenceResultListView(QWidget *parent)
    : QListView{parent}
    , _delegate{new SentenceResultListDelegate{this}}
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(250);

    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &SentenceResultListView::paintWithApplicationState);
}

// On Windows, because of a bug in Qt (see QTBUG-7232), every time mouse
// is scrolled, listview advances by by three items. Override the wheelEvent to
// modify this undesired behaviour until fixed by Qt.
#ifdef Q_OS_WIN
void SentenceResultListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif

void SentenceResultListView::paintWithApplicationState()
{
    viewport()->update();         // Forces repaint of viewing area
    scheduleDelayedItemsLayout(); // Forces items to resize themselves
}
