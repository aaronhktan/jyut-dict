#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <logic/search/isearch.h>

#include <QAction>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    ~SearchLineEdit();

    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    QAction *_clearLineEdit;
    ISearch *_search;

signals:

public slots:
};

#endif // SEARCHLINEEDIT_H
