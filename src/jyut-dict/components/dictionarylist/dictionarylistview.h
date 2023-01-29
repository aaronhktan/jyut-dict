#ifndef DICTIONARYLISTVIEW_H
#define DICTIONARYLISTVIEW_H

#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif
#include <QWidget>

// The DictionaryListView displays the dictionaries that a user has installed

class DictionaryListView : public QListView
{
    Q_OBJECT
public:
    explicit DictionaryListView(QWidget *parent = nullptr);

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

#endif // DICTIONARYLISTVIEW_H
