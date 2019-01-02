#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QAction>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget *parent = nullptr);

    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    QAction *_clearLineEdit;

signals:

public slots:
};

#endif // SEARCHLINEEDIT_H
