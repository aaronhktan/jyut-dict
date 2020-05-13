#ifndef SEARCHHISTORYLISTDELEGATE_H
#define SEARCHHISTORYLISTDELEGATE_H

#include <QModelIndex>
#include <QPainter>
#include <QSettings>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

#include <memory>

// The SearchHistoryListDelegate is responsible for painting elements in the
// SearchHistoryListView
// It also provides a sizehint for each element

class SearchHistoryListDelegate : public QStyledItemDelegate
{
Q_OBJECT

public:
    explicit SearchHistoryListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;

private:
    std::unique_ptr<QSettings> _settings;
};

#endif // SEARCHHISTORYLISTDELEGATE_H
