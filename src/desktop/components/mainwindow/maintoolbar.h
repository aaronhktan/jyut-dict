#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include "logic/database/sqluserhistoryutils.h"
#include "logic/search/isearchoptionsmediator.h"

#include <QSettings>
#include <QToolBar>

#include <memory>

class SearchLineEdit;
class SearchOptionsRadioGroupBox;
class SQLSearch;

class QAction;
class QEvent;
class QFocusEvent;
class QGridLayout;
class QMenu;
class QToolButton;
class QWidget;

// The MainToolBar is an object that contains all widgets in the toolbar
// This includes the searchbar, and buttons that launch various windows

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
    void changeSearchParameters(const SearchParameters params) const;

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
    void forwardSearchHistoryItem(const SearchTermHistoryItem &pair) const;
    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters) const;
    void searchRequested(void) const;
    void dictationRequested(void) const;
    void handwritingRequested(void) const;
    void updateStyleRequested(void);
};

#endif // MAINTOOLBAR_H
