#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include "components/entrydelegate.h"
#include "logic/entry/entry.h"

#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif
#include <QWidget>

#include <vector>

// The SearchListView displays results of a search
// It populates itself with a QAbstractListModel
// And paints itself with a QStyledItemDelegate

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
};

#endif // SEARCHLISTVIEW_H
