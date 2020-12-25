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
#include <QHBoxLayout>
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
    ~MainToolBar() override;

    void changeEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

    void selectAllEvent(void);
    void changeOptionEvent(const Utils::ButtonOptionIndex option);

    void setOpenHistoryAction(QAction *action);
    void setOpenFavouritesAction(QAction *action);
    void setOpenSettingsAction(QAction *action);

private:
    void setupUI(void);

    void setStyle(bool use_dark);
    bool _paletteRecentlyChanged = false;

    QHBoxLayout *_toolBarLayout;
    QWidget *_toolBarWidget;

    SearchLineEdit *_searchBar;
    SearchOptionsRadioGroupBox *_optionsBox;
    QToolButton *_openHistoryButton;
    QToolButton *_openFavouritesButton;
    QToolButton *_openSettingsButton;
    ISearchOptionsMediator *_searchOptions;

public slots:
    void forwardSearchHistoryItem(searchTermHistoryItem &pair);

#ifndef Q_OS_LINUX
signals:
    void searchBarTextChange(void);
#endif
};

#endif // MAINTOOLBAR_H
