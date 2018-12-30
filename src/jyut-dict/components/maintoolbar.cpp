#include "maintoolbar.h"

MainToolBar::MainToolBar(QWidget *parent) : QToolBar(parent)
{
    toolBarWidget = new QWidget;
    toolBarLayout = new QHBoxLayout;

    searchBar = new SearchLineEdit(this);
    toolBarLayout->addWidget(searchBar);

    toolBarWidget->setLayout(toolBarLayout);

    addWidget(toolBarWidget);
    setMovable(false);
    setFloatable(false);
}

MainToolBar::~MainToolBar()
{
    delete toolBarLayout;
    delete searchBar;
}
