#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include "logic/entry/entry.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/isearchobservable.h"

#include <QListView>

#include <vector>

class SearchListView : public QListView, public ISearchObserver
{
    Q_OBJECT

public:
    explicit SearchListView(QWidget *parent = nullptr);

    void callback(std::vector<Entry> entries) override;

private:
    ISearchObservable *_search;

signals:

public slots:
};

#endif // SEARCHLISTVIEW_H
