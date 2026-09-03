#ifndef RESULTLISTDELEGATE_H
#define RESULTLISTDELEGATE_H

#include <QSettings>
#include <QStyledItemDelegate>

#include <memory>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class QWidget;

// The ResultListDelegate is responsible for painting elements in the ResultListView
// It also provides a sizehint for each element

class ResultListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ResultListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;

private:
    std::unique_ptr<QSettings> _settings;
};

#endif // RESULTLISTDELEGATE_H
