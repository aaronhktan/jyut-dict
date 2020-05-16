#ifndef VIEWHISTORYLISTVIEW_H
#define VIEWHISTORYLISTVIEW_H

#include <QEvent>
#include <QListView>
#include <QStyledItemDelegate>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif
#include <QWidget>

// The ViewHistoryListView displays results of a search
// It populates itself with a QAbstractListModel
// And paints itself with a QStyledItemDelegate

class ViewHistoryListView : public QListView
{
    Q_OBJECT

public:
    explicit ViewHistoryListView(QWidget *parent = nullptr);

#ifdef Q_OS_WIN
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    QStyledItemDelegate *_delegate;

public slots:
    void paintWithApplicationState();
};

#endif // VIEWHISTORYLISTVIEW_H
