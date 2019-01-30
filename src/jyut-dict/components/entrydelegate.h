#ifndef ENTRYDELEGATE_H
#define ENTRYDELEGATE_H

#include <QPainter>
#include <QStyledItemDelegate>

// The EntryDelegate is responsible for painting elements in the searchlistview
// It also provides a sizehint for each element

class EntryDelegate : public QStyledItemDelegate
{
public:
    EntryDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;
};

#endif // ENTRYDELEGATE_H
