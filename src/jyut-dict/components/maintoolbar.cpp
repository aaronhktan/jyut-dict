#include "maintoolbar.h"

MainToolBar::MainToolBar(QWidget *parent) : QToolBar(parent)
{
    toolBarWidget = new QWidget;
    toolBarLayout = new QHBoxLayout;

    searchBar = new SearchLineEdit(this);
    toolBarLayout->addWidget(searchBar);

    toolBarWidget->setLayout(toolBarLayout);

    this->addWidget(toolBarWidget);
    this->setMovable(false);
    this->setFloatable(false);
}

MainToolBar::~MainToolBar()
{
}
