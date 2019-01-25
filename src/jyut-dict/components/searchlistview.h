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

#ifdef Q_OS_WIN
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    QAbstractListModel *_model;
    QStyledItemDelegate *_delegate;

signals:

public slots:
};

#endif // SEARCHLISTVIEW_H
