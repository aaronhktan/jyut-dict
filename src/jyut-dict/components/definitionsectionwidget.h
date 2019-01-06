#ifndef DEFINITIONSECTIONWIDGET_H
#define DEFINITIONSECTIONWIDGET_H

#include "components/definitioncontentwidget.h"
#include "components/definitionheaderwidget.h"
#include "logic/entry/definitionsset.h"

#include <QVBoxLayout>
#include <QWidget>

class DefinitionSectionWidget : public QWidget
{
public:
    explicit DefinitionSectionWidget(QWidget *parent = nullptr);
    ~DefinitionSectionWidget();

    void setEntry(DefinitionsSet& definitionsSet);

private:
    QVBoxLayout *_definitionAreaLayout;
    DefinitionHeaderWidget *_definitionHeaderWidget;
    DefinitionContentWidget *_definitionWidget;
};

#endif // DEFINITIONSECTIONWIDGET_H
