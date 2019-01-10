#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include "components/entrydelegate.h"
#include "components/searchlistwidget.h"

#include "logic/entry/entry.h"

#include <QListView>
#include <QResizeEvent>

#include <vector>

class SearchListView : public QListView
{
    Q_OBJECT

public:
    explicit SearchListView(QWidget *parent = nullptr);

private:
    void resizeEvent(QResizeEvent *event) override;

    void cleanup();

    QAbstractListModel *_model;
    QStyledItemDelegate *_delegate;
    std::vector<SearchListWidget *> _widgets;

signals:

public slots:
};

#endif // SEARCHLISTVIEW_H
