#ifndef SEARCHHISTORYLISTVIEW_H
#define SEARCHHISTORYLISTVIEW_H

#include <QListView>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif

class QEvent;
class QStyledItemDelegate;
#ifdef Q_OS_WIN
class QWheelEvent;
#endif

// The ResultListView displays results of a search
// It populates itself with a QAbstractListModel
// And paints itself with a QStyledItemDelegate

class SearchHistoryListView : public QListView
{
    Q_OBJECT
public:
    explicit SearchHistoryListView(QWidget *parent = nullptr);

#ifdef Q_OS_WIN
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    QStyledItemDelegate *_delegate;

public slots:
    void paintWithApplicationState();
};

#endif // SEARCHHISTORYLISTVIEW_H
