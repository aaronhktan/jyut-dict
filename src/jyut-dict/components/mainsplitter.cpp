#include "mainsplitter.h"

#include <QList>

MainSplitter::MainSplitter(QWidget *parent) : QSplitter(parent)
{
    definitionScrollArea = new DefinitionScrollArea;
    searchListView = new SearchListView;

    addWidget(searchListView);
    addWidget(definitionScrollArea);
    setHandleWidth(0);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
    setStyleSheet("QSplitter::handle \
                   { \
                          background-color: rgb(0, 0, 0); \
                   }");
}

MainSplitter::~MainSplitter()
{
    delete definitionScrollArea;
    delete searchListView;
}
