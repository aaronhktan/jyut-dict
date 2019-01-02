#include "definitionsectionwidget.h"

DefinitionSectionWidget::DefinitionSectionWidget(QWidget *parent) : QWidget(parent)
{
    _definitionAreaLayout = new QVBoxLayout(this);
    _definitionAreaLayout->setContentsMargins(0, 0, 0, 0);
    _definitionAreaLayout->setSpacing(11);
    setLayout(_definitionAreaLayout);
}

void DefinitionSectionWidget::setEntry(DefinitionsSet& definitionsSet)
{
    _definitionHeaderWidget = new DefinitionHeaderWidget();
    _definitionHeaderWidget->setSectionTitle("DEFINITIONS");

    _definitionWidget = new DefinitionContentWidget();
    _definitionWidget->setEntry(definitionsSet.getDefinitions());

    _definitionAreaLayout->addWidget(_definitionHeaderWidget);
    _definitionAreaLayout->addWidget(_definitionWidget);
}
