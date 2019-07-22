#include "mainsplitter.h"

#include "components/resultlistview.h"
#include "logic/entry/entry.h"

#include <QList>

MainSplitter::MainSplitter(QWidget *parent) : QSplitter(parent)
{
    _definitionScrollArea = new DefinitionScrollArea;
    _resultListView = new ResultListView();

    addWidget(_resultListView);
    addWidget(_definitionScrollArea);

    connect(_resultListView->selectionModel(),
       SIGNAL(currentChanged(QModelIndex, QModelIndex)),
       this, SLOT(handleSelectionChanged(QModelIndex)));

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#ifdef Q_OS_WIN
    setStyleSheet("QSplitter::handle { background-color: #b9b9b9; }");
#elif defined(Q_OS_DARWIN)
    setStyleSheet("QSplitter::handle { background-color: none; }");
#else
    setStyleSheet("QSplitter::handle { background-color: lightgray; }");
#endif
}

MainSplitter::~MainSplitter()
{

}

void MainSplitter::handleSelectionChanged(const QModelIndex &selection)
{
    Entry entry = qvariant_cast<Entry>(selection.data());
    if (entry.getSimplified() == "Welcome!") {
        return;
    }

    _definitionScrollArea->setEntry(entry);
}
