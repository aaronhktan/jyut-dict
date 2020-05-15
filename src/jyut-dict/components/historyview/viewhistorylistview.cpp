#include "viewhistorylistview.h"

#include "components/historyview/viewhistorylistdelegate.h"

#include <QGuiApplication>

#ifdef Q_OS_WIN
#include <QScrollBar>
#endif

ViewHistoryListView::ViewHistoryListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);

    _delegate = new ViewHistoryListDelegate{this};
    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &ViewHistoryListView::paintWithApplicationState);
}

// On Windows, because of a bug in Qt (see QTBUG-7232), every time mouse
// is scrolled, listview advances by by three items. Override the wheelEvent to
// modify this undesired behaviour until fixed by Qt.
#ifdef Q_OS_WIN
void ViewHistoryListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif

void ViewHistoryListView::paintWithApplicationState()
{
    viewport()->update(); // Forces repaint of viewing area.
}
