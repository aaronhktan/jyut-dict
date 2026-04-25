#include "sourcelistview.h"

#include "components/sourcelist/sourcelistdelegate.h"
#include "components/sourcelist/sourcelistmodel.h"

#include <QGuiApplication>

#ifdef Q_OS_WIN
#include <QScrollBar>
#endif

SourceListView::SourceListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(150);
    
    _model = new SourceListModel{this};
    setModel(_model);
    
    _delegate = new SourceListDelegate{this};
    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(qApp,
            &QGuiApplication::applicationStateChanged,
            this,
            &SourceListView::paintWithApplicationState);
}

// On Windows, because of a bug in Qt (see QTBUG-7232), every time mouse
// is scrolled, listview advances by by three items. Override the wheelEvent to
// modify this undesired behaviour until fixed by Qt.
#ifdef Q_OS_WIN
void SourceListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif

void SourceListView::paintWithApplicationState()
{
    viewport()->update();
}

#ifdef Q_OS_MAC
void SourceListView::mousePressEvent(QMouseEvent *event)
{
    // Don't change item selection on mouse press.
    // The window size changes before the user releases the mouse, which causes
    // the item selection to change, and bounces the user to another item that
    // they didn't select.
    (void) (event);
}
#endif
