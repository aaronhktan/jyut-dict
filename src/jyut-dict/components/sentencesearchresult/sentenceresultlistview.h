#ifndef SENTENCERESULTLISTVIEW_H
#define SENTENCERESULTLISTVIEW_H

#include <QEvent>
#include <QListView>
#include <QStyledItemDelegate>
#ifdef Q_OS_WIN
#include <QWheelEvent>
#endif
#include <QWidget>

// The SentenceResultListView displays results of a search
// It populates itself with a QAbstractListModel
// And paints itself with a QStyledItemDelegate

class SentenceResultListView : public QListView
{
    Q_OBJECT

public:
    explicit SentenceResultListView(QWidget *parent = nullptr);

#ifdef Q_OS_WIN
    void wheelEvent(QWheelEvent *event) override;
#endif

private:
    QStyledItemDelegate *_delegate;

public slots:
    void paintWithApplicationState();
};

#endif // SENTENCERESULTLISTVIEW_H
