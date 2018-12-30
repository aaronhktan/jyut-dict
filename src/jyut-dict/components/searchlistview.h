#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include <QListView>

class SearchListView : public QListView
{
    Q_OBJECT

public:
    explicit SearchListView(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // SEARCHLISTVIEW_H
