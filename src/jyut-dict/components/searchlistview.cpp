#include "searchlistview.h"

#include "logic/search/sqlsearch.h"

SearchListView::SearchListView(QWidget *parent)
    : QListView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(250);

    _search = new SQLSearch();
    _search->registerObserver(this);
}

void SearchListView::callback(std::vector<Entry> entries)
{
    for (auto entry : entries) {
        std:: cout << entry << std::endl;
    }
}
