#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "components/isearchlineedit.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchoptionsmediator.h"

#include <QAction>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineEdit>

class SearchLineEdit : public QLineEdit, public ISearchLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(ISearchOptionsMediator *mediator, QWidget *parent = nullptr);
    ~SearchLineEdit() override;

    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void updateParameters(SearchParameters parameters) override;
    void search() override;
private:
    ISearchOptionsMediator *_mediator;
    ISearch *_search;

    QAction *_clearLineEdit;

    SearchParameters _parameters;

signals:

public slots:
    void checkClearVisibility();
};

#endif // SEARCHLINEEDIT_H
