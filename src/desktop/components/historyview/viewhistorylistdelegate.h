#ifndef VIEWHISTORYLISTDELEGATE_H
#define VIEWHISTORYLISTDELEGATE_H

#include <QSettings>
#include <QStyledItemDelegate>

#include <memory>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class QWidget;

// The ViewHistoryListDelegate is responsible for painting elements in the
// ViewHistoryListView
// It also provides a sizehint for each element

class ViewHistoryListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ViewHistoryListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;

private:
    std::unique_ptr<QSettings> _settings;
};

#endif // VIEWHISTORYLISTDELEGATE_H
