#ifndef EXAMPLERESULTLISTDELEGATE_H
#define EXAMPLERESULTLISTDELEGATE_H

#include <QSettings>
#include <QStyledItemDelegate>

#include <memory>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class QWidget;

// The ExampleResultListDelegate is responsible for painting elements in the
// ExampleResultListView (basically, a bunch of example objects)
// It also provides a sizehint for each element

class ExampleResultListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ExampleResultListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    std::unique_ptr<QSettings> _settings;
};

#endif // EXAMPLERESULTLISTDELEGATE_H
