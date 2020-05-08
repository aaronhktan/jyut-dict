#ifndef DICTIONARYLISTDELEGATE_H
#define DICTIONARYLISTDELEGATE_H

#include <QModelIndex>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

// The DictionaryListDelegate is responsible for painting dictionaries
// in a listview.

class DictionaryListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DictionaryListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
signals:

public slots:
};

#endif // DICTIONARYLISTDELEGATE_H
