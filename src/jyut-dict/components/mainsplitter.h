#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include "components/definitionscrollarea.h"
#include "components/searchlistview.h"

#include <QSplitter>

class MainSplitter : public QSplitter
{
public:
    explicit MainSplitter(QWidget *parent = nullptr);
    ~MainSplitter();

private:
    DefinitionScrollArea *definitionScrollArea;
    SearchListView *searchListView;
};

#endif // MAINSPLITTER_H
