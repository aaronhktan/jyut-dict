#ifndef RESULTLISTVIEW_H
#define RESULTLISTVIEW_H

#include "components/resultlistdelegate.h"
#include "logic/entry/entry.h"

#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif
#include <QWidget>

#include <vector>

// The ResultListView displays results of a search
// It populates itself with a QAbstractListModel
// And paints itself with a QStyledItemDelegate

class ResultListView : public QListView
{
    Q_OBJECT

public:
    explicit ResultListView(QWidget *parent = nullptr);

#ifdef Q_OS_WIN
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    QAbstractListModel *_model;
    QStyledItemDelegate *_delegate;
};

#endif // RESULTLISTVIEW_H
