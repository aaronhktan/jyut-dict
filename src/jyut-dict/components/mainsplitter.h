#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H

#include "components/definitionscrollarea.h"
#include "components/searchlistview.h"

#include <QSplitter>

class MainSplitter : public QSplitter
{
    Q_OBJECT

public:
    explicit MainSplitter(QWidget *parent = nullptr);
    ~MainSplitter();

private:
    DefinitionScrollArea *definitionScrollArea;
    SearchListView *searchListView;

signals:

public slots:
};

#endif // MAINSPLITTER_H
