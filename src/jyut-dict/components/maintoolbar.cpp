#include "maintoolbar.h"

MainToolBar::MainToolBar(QWidget *parent) : QToolBar(parent)
{
    _toolBarWidget = new QWidget;
    _toolBarLayout = new QHBoxLayout;

    _searchBar = new SearchLineEdit(this);
    _toolBarLayout->addWidget(_searchBar);

    _toolBarWidget->setLayout(_toolBarLayout);

    addWidget(_toolBarWidget);
    setMovable(false);
    setFloatable(false);
}

MainToolBar::~MainToolBar()
{
    delete _toolBarLayout;
    delete _searchBar;
}
