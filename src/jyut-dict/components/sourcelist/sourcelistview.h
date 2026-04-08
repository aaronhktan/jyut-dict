#ifndef SOURCELISTVIEW_H
#define SOURCELISTVIEW_H

#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif
#include <QWidget>

// The SourceListView displays the source that a user has installed

class SourceListView : public QListView
{
    Q_OBJECT
public:
    explicit SourceListView(QWidget *parent = nullptr);

#ifdef Q_OS_WIN
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    void paintWithApplicationState();

    QAbstractListModel *_model;
    QStyledItemDelegate *_delegate;

#ifdef Q_OS_MAC
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
#endif
};

#endif // SOURCELISTVIEW_H
