#include "searchlistview.h"

SearchListView::SearchListView(QWidget *parent) : QListView(parent)
{
    this->setFrameShape(QFrame::NoFrame);
}

SearchListView::~SearchListView()
{

}
