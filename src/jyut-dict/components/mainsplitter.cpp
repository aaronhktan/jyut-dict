#include "mainsplitter.h"

#include "logic/entry/entry.h"

#include <QList>

MainSplitter::MainSplitter(QWidget *parent) : QSplitter(parent)
{
    _definitionScrollArea = new DefinitionScrollArea;
    _searchListView = new SearchListView();

    addWidget(_searchListView);
    addWidget(_definitionScrollArea);

    connect(_searchListView->selectionModel(),
       SIGNAL(currentChanged(QModelIndex, QModelIndex)),
       this, SLOT(handleSelectionChanged(QModelIndex)));


    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#ifdef __APPLE__
    setStyleSheet("QSplitter::handle \
                   { \
                          background-color: rgb(0, 0, 0); \
                   }");
#elif __WIN32__
    setStyleSheet("QSplitter::handle \
                   { \
                          background-color: #b9b9b9; \
                   }");
#endif
}

MainSplitter::~MainSplitter()
{
    delete _definitionScrollArea;
    delete _searchListView;
}

void MainSplitter::handleSelectionChanged(const QModelIndex& selection)
{
    Entry entry = qvariant_cast<Entry>(selection.data());
    if (entry.getSimplified() == "Welcome!") {
        return;
    }

    _definitionScrollArea->setEntry(entry);
}
