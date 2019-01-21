#include "searchlistview.h"

#include "components/entrylistmodel.h"
#include "logic/search/sqlsearch.h"

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
