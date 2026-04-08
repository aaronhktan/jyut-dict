#ifndef SOURCELISTDELEGATE_H
#define SOURCELISTDELEGATE_H

#include <QModelIndex>
#include <QPainter>
#include <QSettings>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QWidget>

// The SourceListDelegate is responsible for painting sources
// in a listview.

class SourceListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SourceListDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    std::unique_ptr<QSettings> _settings;
};

#endif // SOURCELISTDELEGATE_H
