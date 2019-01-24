#include "maintoolbar.h"

#include "logic/search/searchoptionsmediator.h"

MainToolBar::MainToolBar(QWidget *parent) : QToolBar(parent)
{
    _toolBarWidget = new QWidget;
    _toolBarLayout = new QHBoxLayout;

    _searchOptions = new SearchOptionsMediator();

    _searchBar = new SearchLineEdit(_searchOptions, this);
    _searchOptions->registerLineEdit(_searchBar);
    _optionsBox = new SearchOptionsRadioGroupBox(_searchOptions, this);

    _toolBarLayout->addWidget(_searchBar);
    _toolBarLayout->addWidget(_optionsBox);
//    _toolBarLayout->addStretch();

    _toolBarWidget->setLayout(_toolBarLayout);

    addWidget(_toolBarWidget);
    setMovable(false);
    setFloatable(false);

#ifdef __WIN32
    setStyleSheet("QToolBar { background-color: white; }");
#endif
}

MainToolBar::~MainToolBar()
{
    delete _toolBarLayout;
    delete _searchBar;
    delete _optionsBox;
}
