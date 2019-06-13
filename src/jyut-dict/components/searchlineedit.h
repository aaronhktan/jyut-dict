#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "components/isearchlineedit.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchoptionsmediator.h"

#include <QAction>
#include <QEvent>
#include <QFocusEvent>
#include <QLineEdit>
#include <QWidget>

// The SearchLineEdit is the main search bar

class SearchLineEdit : public QLineEdit, public ISearchLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(ISearchOptionsMediator *mediator,
                            QWidget *parent = nullptr);
    ~SearchLineEdit() override;

    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;

    void updateParameters(SearchParameters parameters) override;
    void search() override;

private:
    void checkClearVisibility();
    void setStyle(bool use_dark);

    ISearchOptionsMediator *_mediator;
    ISearch *_search;

    QAction *_searchLineEdit;
    QAction *_clearLineEdit;

    SearchParameters _parameters;

    bool _paletteRecentlyChanged;
};

#endif // SEARCHLINEEDIT_H
