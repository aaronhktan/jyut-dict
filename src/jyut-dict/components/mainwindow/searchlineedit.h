#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "components/mainwindow/isearchlineedit.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/search/isearch.h"
#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/sqlsearch.h"

#include <QAction>
#include <QEvent>
#include <QFocusEvent>
#include <QLineEdit>
#include <QTimer>
#include <QWidget>

#include <memory>

// The SearchLineEdit is the main search bar

class SearchLineEdit : public QLineEdit, public ISearchLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(ISearchOptionsMediator *mediator,
                            std::shared_ptr<SQLSearch> manager,
                            std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                            QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void updateParameters(SearchParameters parameters) override;
    void search() override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void checkClearVisibility();

    void addSearchTermToHistory(void) const;

    ISearchOptionsMediator *_mediator;
    std::shared_ptr<ISearch> _search;
    std::shared_ptr<SQLUserHistoryUtils> _sqlHistoryUtils;
    std::unique_ptr<QSettings> _settings;

    QAction *_searchLineEdit;
    QAction *_clearLineEdit;
    QTimer *_timer;

    SearchParameters _parameters;

    bool _paletteRecentlyChanged = false;
};

#endif // SEARCHLINEEDIT_H
