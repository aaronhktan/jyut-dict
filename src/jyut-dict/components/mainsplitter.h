#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include "components/definitionscrollarea.h"
#include "components/searchlistview.h"

#include "logic/search/sqlsearch.h"

#include <QSplitter>

class MainSplitter : public QSplitter
{
Q_OBJECT

public:
    explicit MainSplitter(QWidget *parent = nullptr);
    ~MainSplitter();

private:
    DefinitionScrollArea *_definitionScrollArea;
    SearchListView *_searchListView;

private slots:
    void handleSelectionChanged(const QModelIndex& selection);
};

#endif // MAINSPLITTER_H
