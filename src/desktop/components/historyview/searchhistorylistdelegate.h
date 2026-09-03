#ifndef SEARCHHISTORYLISTDELEGATE_H
#define SEARCHHISTORYLISTDELEGATE_H

#include <QSettings>
#include <QStyledItemDelegate>

#include <memory>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class QWidget;

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
