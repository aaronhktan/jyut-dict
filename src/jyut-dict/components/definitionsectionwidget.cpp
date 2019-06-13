#include "definitionsectionwidget.h"

DefinitionSectionWidget::DefinitionSectionWidget(QWidget *parent) : QWidget(parent)
{
    _definitionAreaLayout = new QVBoxLayout(this);
    _definitionAreaLayout->setContentsMargins(0, 0, 0, 0);
    _definitionAreaLayout->setSpacing(11);
    setLayout(_definitionAreaLayout);

    _definitionHeaderWidget = new DefinitionHeaderWidget();
    _definitionWidget = new DefinitionContentWidget();
}

DefinitionSectionWidget::~DefinitionSectionWidget()
{

}

void DefinitionSectionWidget::setEntry(const DefinitionsSet &definitionsSet)
{
    std::string source = definitionsSet.getSourceShortString();
    _definitionHeaderWidget->setSectionTitle("DEFINITIONS (" + source + ")");

    _definitionWidget->setEntry(definitionsSet.getDefinitions());

    _definitionAreaLayout->addWidget(_definitionHeaderWidget);
    _definitionAreaLayout->addWidget(_definitionWidget);
}
