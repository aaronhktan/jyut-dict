#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include "components/searchlistwidget.h"

#include "logic/entry/entry.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/isearchobservable.h"

#include <QListWidget>
#include <QResizeEvent>

#include <vector>

class SearchListView : public QListWidget, public ISearchObserver
{
    Q_OBJECT

public:
    explicit SearchListView(QWidget *parent = nullptr);

    void callback(std::vector<Entry> entries) override;

private:
    void resizeEvent(QResizeEvent *event) override;

    void cleanup();

    ISearchObservable *_search;

signals:

public slots:
};

#endif // SEARCHLISTVIEW_H
