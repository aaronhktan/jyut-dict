#include "searchlistview.h"

#include "components/entrylistmodel.h"
#include "logic/search/sqlsearch.h"

#include "tzfile.h"
#include <chrono>

SearchListView::SearchListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(250);

    _model = new EntryListModel({}, this);
    setModel(_model);

    _delegate = new EntryDelegate(this);
    setItemDelegate(_delegate);
}

void SearchListView::resizeEvent(QResizeEvent *event)
{
    event->accept();
    for (auto widget : _widgets) {
        widget->resize(width(), widget->sizeHint().height());
    }
}

void SearchListView::cleanup()
{
    _widgets.clear();
}
