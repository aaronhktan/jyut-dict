#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include "components/mainwindow/searchlineedit.h"
#include "components/mainwindow/searchoptionsradiogroupbox.h"
#include "logic/database/sqluserhistoryutils.h"
#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/sqlsearch.h"
#include "logic/utils/utils.h"

#include <QAction>
#include <QEvent>
#include <QFocusEvent>
#include <QGridLayout>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>

#include <memory>

// The MainToolBar is an object that contains all widgets in the toolbar
// This includes the searchbar,
// and any other radio buttons that change search parameters

class MainToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit MainToolBar(std::shared_ptr<SQLSearch> sqlSearch,
                         std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                         QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

    void selectAllEvent(void) const;
    void changeOptionEvent(const Utils::ButtonOptionIndex option) const;

    void setOpenHistoryAction(QAction *action) const;
    void setOpenFavouritesAction(QAction *action) const;
    void setOpenSettingsAction(QAction *action) const;

private:
    void setupUI(void);

    void setStyle(bool use_dark);
    bool _paletteRecentlyChanged = false;

    int _inactiveCount = 0;

    std::unique_ptr<QSettings> _settings;

    QGridLayout *_toolBarLayout;
    QWidget *_toolBarWidget;

    SearchLineEdit *_searchBar;
    SearchOptionsRadioGroupBox *_optionsBox;
    QToolButton *_openHistoryButton;
    QToolButton *_openFavouritesButton;
    QToolButton *_openSettingsButton;
    std::shared_ptr<ISearchOptionsMediator> _searchOptions;

signals:
    void searchBarTextChange(void);

public slots:
    void forwardSearchHistoryItem(const searchTermHistoryItem &pair) const;
    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters) const;
    void searchRequested(void) const;
    void dictationRequested(void) const;
    void updateStyleRequested(void);
};

#endif // MAINTOOLBAR_H
