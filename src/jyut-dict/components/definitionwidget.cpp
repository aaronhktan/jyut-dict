#include "definitionwidget.h"

DefinitionWidget::DefinitionWidget(QWidget *parent) : QWidget(parent)
{
    _definitionSectionsLayout = new QVBoxLayout{this};
    _definitionSectionsLayout->setContentsMargins(0, 0, 0, 0);
    _definitionSectionsLayout->setSpacing(25);
}

DefinitionWidget::DefinitionWidget(Entry &entry, QWidget *parent)
    : DefinitionWidget(parent)
{
    setEntry(entry);
}

DefinitionWidget::~DefinitionWidget()
{

}

void DefinitionWidget::setEntry(const Entry &entry)
{
    cleanup();
    for (auto definitionsSet : entry.getDefinitionsSets()) {
        _definitionSections.push_back(new DefinitionSectionWidget{this});
        _definitionSections.back()->setEntry(definitionsSet);

        _definitionSectionsLayout->addWidget(_definitionSections.back());
    }

    // Force layout to update after adding widgets; fixes some layout issues.
    _definitionSectionsLayout->activate();
}

void DefinitionWidget::cleanup()
{
    for (auto section : _definitionSections) {
        _definitionSectionsLayout->removeWidget(section);
        delete section;
    }
    _definitionSections.clear();
}
