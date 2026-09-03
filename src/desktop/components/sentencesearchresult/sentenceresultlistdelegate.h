#ifndef SENTENCERESULTLISTDELEGATE_H
#define SENTENCERESULTLISTDELEGATE_H

#include <QSettings>
#include <QStyledItemDelegate>

#include <memory>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class QWidget;

// The SentenceResultListDelegate is responsible for painting elements in the
// SentenceResultListView (basically, a bunch of sentence objects)
// It also provides a sizehint for each element

class SentenceResultListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SentenceResultListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    std::unique_ptr<QSettings> _settings;
};

#endif // SENTENCERESULTLISTDELEGATE_H
