#include "definitionwidget.h"

DefinitionWidget::DefinitionWidget(QWidget *parent) : QWidget(parent)
{
    _definitionSectionsLayout = new QVBoxLayout(this);
    _definitionSectionsLayout->setContentsMargins(0, 0, 0, 0);
    _definitionSectionsLayout->setSpacing(25);
}

DefinitionWidget::DefinitionWidget(Entry& entry, QWidget *parent)
    : DefinitionWidget(parent)
{
    setEntry(entry);
}

void DefinitionWidget::setEntry(Entry& entry)
{
    cleanup();
    for (auto definitionsSet : entry.getDefinitionsSets()) {
        _definitionSections.push_back(new DefinitionSectionWidget());
        _definitionSections.back()->setEntry(definitionsSet);

        _definitionSectionsLayout->addWidget(_definitionSections.back());
    }
}

void DefinitionWidget::cleanup()
{
    for (auto section : _definitionSections) {
        _definitionSectionsLayout->removeWidget(section);
        _definitionSections.erase(std::remove(_definitionSections.begin(),
                                              _definitionSections.end(),
                                              section), _definitionSections.end());
        delete section;
    }
}

DefinitionWidget::~DefinitionWidget()
{
    delete _definitionSectionsLayout;
    for (auto definitionSection : _definitionSections) {
        delete definitionSection;
    }
}
