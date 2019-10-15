#include "definitionsectionwidget.h"

DefinitionSectionWidget::DefinitionSectionWidget(QWidget *parent) : QWidget(parent)
{
    _definitionAreaLayout = new QVBoxLayout{this};
    _definitionAreaLayout->setContentsMargins(0, 0, 0, 0);
    _definitionAreaLayout->setSpacing(11);
    setLayout(_definitionAreaLayout);

    _definitionHeaderWidget = new DefinitionHeaderWidget{this};
    _definitionWidget = new DefinitionContentWidget{this};

    setStyleSheet("QWidget { "
                  " background-color: #323232; "
                  " border-radius: 10px; "
                  "}");
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
