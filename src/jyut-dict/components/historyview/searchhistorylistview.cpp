#include "searchhistorylistview.h"

#include "components/historyview/searchhistorylistdelegate.h"

#include <QGuiApplication>

#ifdef Q_OS_WIN
#include <QScrollBar>
#endif

SearchHistoryListView::SearchHistoryListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);

    _delegate = new SearchHistoryListDelegate{this};
    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &SearchHistoryListView::paintWithApplicationState);
}

// On Windows, because of a bug in Qt (see QTBUG-7232), every time mouse
// is scrolled, listview advances by by three items. Override the wheelEvent to
// modify this undesired behaviour until fixed by Qt.
#ifdef Q_OS_WIN
void SearchHistoryListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif

void SearchHistoryListView::paintWithApplicationState()
{
    viewport()->update(); // Forces repaint of viewing area.
}
