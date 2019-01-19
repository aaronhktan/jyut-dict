#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include "components/searchlineedit.h"
#include "components/searchoptionsradiogroupbox.h"
#include "logic/search/isearchoptionsmediator.h"

#include <QHBoxLayout>
#include <QToolBar>

class MainToolBar : public QToolBar
{
public:
    explicit MainToolBar(QWidget *parent = nullptr);
    ~MainToolBar();

private:
    QHBoxLayout *_toolBarLayout;
    QWidget *_toolBarWidget;

    SearchLineEdit *_searchBar;
    SearchOptionsRadioGroupBox *_optionsBox;
    ISearchOptionsMediator *_searchOptions;
};

#endif // MAINTOOLBAR_H
