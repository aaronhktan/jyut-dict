#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "components/isearchlineedit.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/sqlsearch.h"

#include <QAction>
#include <QEvent>
#include <QFocusEvent>
#include <QLineEdit>
#include <QWidget>

#include <memory>

// The SearchLineEdit is the main search bar

class SearchLineEdit : public QLineEdit, public ISearchLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(ISearchOptionsMediator *mediator,
                            std::shared_ptr<SQLSearch> manager,
                            QWidget *parent = nullptr);
    ~SearchLineEdit() override;

    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void updateParameters(SearchParameters parameters) override;
    void search() override;

private:
    void checkClearVisibility();
    void translateUI();
    void setStyle(bool use_dark);

    ISearchOptionsMediator *_mediator;
    std::shared_ptr<ISearch> _search;

    QAction *_searchLineEdit;
    QAction *_clearLineEdit;

    SearchParameters _parameters;

    bool _paletteRecentlyChanged = false;
};

#endif // SEARCHLINEEDIT_H
