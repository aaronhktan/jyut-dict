#ifndef DEFINITIONSECTIONWIDGET_H
#define DEFINITIONSECTIONWIDGET_H

#include "components/definitioncontentwidget.h"
#include "components/definitionheaderwidget.h"
#include "logic/entry/definitionsset.h"

#include <QVBoxLayout>
#include <QWidget>

// The DefinitionSectionWidget is a group that displays a set of definitions
// It contains a header (that displays the source), and the definition content

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
