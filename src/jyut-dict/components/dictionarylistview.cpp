#include "dictionarylistview.h"

#include "components/dictionarylistdelegate.h"
#include "components/dictionarylistmodel.h"

#ifdef Q_OS_WIN
#include <QScrollBar>
#endif

DictionaryListView::DictionaryListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(150);

    _model = new DictionaryListModel{this};
    setModel(_model);

    _delegate = new DictionaryListDelegate{this};
    setItemDelegate(_delegate);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

// On Windows, because of a bug in Qt (see QTBUG-7232), every time mouse
// is scrolled, listview advances by by three items. Override the wheelEvent to
// modify this undesired behaviour until fixed by Qt.
#ifdef Q_OS_WIN
void DictionaryListView::wheelEvent(QWheelEvent *event)
{
    int singleStep = verticalScrollBar()->singleStep();
    singleStep = qMin(singleStep, 10);
    verticalScrollBar()->setSingleStep(singleStep);
    QAbstractItemView::wheelEvent(event);
}
#endif
