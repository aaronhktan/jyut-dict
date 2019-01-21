#include "searchlistview.h"

#include "components/entrylistmodel.h"
#include "logic/search/sqlsearch.h"

#include <QApplication>
#include <QScrollBar>

SearchListView::SearchListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(250);

    _model = new EntryListModel({}, this);
    setModel(_model);

    _delegate = new EntryDelegate(this);
    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

#if __WIN32
void SearchListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif
