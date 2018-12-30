#include "searchlistview.h"

SearchListView::SearchListView(QWidget *parent) : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(250);
}
