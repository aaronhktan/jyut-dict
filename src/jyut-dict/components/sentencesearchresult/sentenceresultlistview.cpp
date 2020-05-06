#include "sentenceresultlistview.h"

#include "components/sentencesearchresult/sentenceresultlistdelegate.h"
#include "logic/search/sqlsearch.h"

#include <QGuiApplication>

#ifdef Q_OS_WIN
#include <QScrollBar>
#endif

SentenceResultListView::SentenceResultListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
#ifdef Q_OS_LINUX
    setMinimumWidth(200);
#else
    setMinimumWidth(250);
#endif

    _delegate = new SentenceResultListDelegate{this};
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
void ResultListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif

void SentenceResultListView::paintWithApplicationState()
{
    viewport()->update(); // Forces repaint of viewing area.
}