#include "mainsplitter.h"

MainSplitter::MainSplitter(QWidget *parent) : QSplitter(parent)
{
    definitionScrollArea = new DefinitionScrollArea;
    searchListView = new SearchListView;

    this->addWidget(searchListView);
    this->addWidget(definitionScrollArea);
    this->setHandleWidth(0);
    this->setCollapsible(0, false);
    this->setCollapsible(1, false);
    this->setStyleSheet("QSplitter::handle \
                         { \
                                background-color: rgb(0, 0, 0); \
                         }");
}

MainSplitter::~MainSplitter()
{
}
