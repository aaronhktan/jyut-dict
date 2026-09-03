#ifndef EXAMPLERESULTLISTVIEW_H
#define EXAMPLERESULTLISTVIEW_H

#include <QListView>

class QEvent;
class QStyledItemDelegate;
#ifdef Q_OS_WIN
class QWheelEvent;
#endif

// The ExampleResultListView displays results of a search
// It populates itself with a QAbstractListModel
// And paints itself with a QStyledItemDelegate

class ExampleResultListView : public QListView
{
    Q_OBJECT
public:
    explicit ExampleResultListView(QWidget *parent = nullptr);

#ifdef Q_OS_WIN
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    QStyledItemDelegate *_delegate;

public slots:
    void paintWithApplicationState();
};

#endif // EXAMPLERESULTLISTVIEW_H
