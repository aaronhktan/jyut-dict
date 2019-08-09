#ifndef RESULTLISTDELEGATE_H
#define RESULTLISTDELEGATE_H

#include <QModelIndex>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

// The ResultListDelegate is responsible for painting elements in the ResultListView
// It also provides a sizehint for each element

class ResultListDelegate : public QStyledItemDelegate
{
public:
    explicit ResultListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;
};

#endif // RESULTLISTDELEGATE_H
