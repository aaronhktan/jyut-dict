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
    QAbstractListModel *_model;
    QStyledItemDelegate *_delegate;
};

#endif // DICTIONARYLISTVIEW_H
