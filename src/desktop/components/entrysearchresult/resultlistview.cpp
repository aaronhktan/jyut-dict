#include "resultlistview.h"

#include "components/entrysearchresult/resultlistdelegate.h"

#include <QEvent>
#include <QGuiApplication>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif
#ifdef Q_OS_WIN
#include <QScrollBar>
#endif
#include <QStyledItemDelegate>

ResultListView::ResultListView(QWidget *parent)
    : QListView{parent}
    , _delegate{new ResultListDelegate{this}}
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(275);

    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &ResultListView::paintWithApplicationState);
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

void ResultListView::paintWithApplicationState()
{
    viewport()->update();         // Forces repaint of viewing area
    scheduleDelayedItemsLayout(); // Forces items to resize themselves
}
