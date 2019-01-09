#include "searchlistview.h"

#include "logic/search/sqlsearch.h"

SearchListView::SearchListView(QWidget *parent)
    : QListWidget(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(250);

    _search = new SQLSearch();
    _search->registerObserver(this);
}

void SearchListView::callback(std::vector<Entry> entries)
{
    cleanup();
    for (auto entry : entries) {
        QListWidgetItem *item;
        item = new QListWidgetItem(this);
        SearchListWidget *widget;
        widget = new SearchListWidget(entry);
        setItemWidget(item, widget);
        item->setSizeHint(QSize(this->width(), widget->minimumSizeHint().height()));
        addItem(item);
    }
}

void SearchListView::resizeEvent(QResizeEvent *event)
{
    event->accept();
//    for (auto widget : _widgets) {
//        widget->resize(width(), widget->sizeHint().height());
//    }
}

void SearchListView::cleanup()
{
    clear();
}
