#include "entrycontentwidget.h"

EntryContentWidget::EntryContentWidget(QWidget *parent) : QWidget(parent)
{
    _definitionSectionsLayout = new QVBoxLayout{this};
    _definitionSectionsLayout->setContentsMargins(0, 0, 0, 0);
    _definitionSectionsLayout->setSpacing(15);
}

EntryContentWidget::EntryContentWidget(Entry &entry, QWidget *parent)
    : EntryContentWidget(parent)
{
    setEntry(entry);
}

EntryContentWidget::~EntryContentWidget()
{

}

void EntryContentWidget::setEntry(const Entry &entry)
{
    cleanup();
    for (auto definitionsSet : entry.getDefinitionsSets()) {
        _definitionSections.push_back(new DefinitionCardWidget{this});
        _definitionSections.back()->setEntry(definitionsSet);

        _definitionSectionsLayout->addWidget(_definitionSections.back());
    }

    // Force layout to update after adding widgets; fixes some layout issues.
    _definitionSectionsLayout->activate();
}

void EntryContentWidget::cleanup()
{
    for (auto section : _definitionSections) {
        _definitionSectionsLayout->removeWidget(section);
        delete section;
    }
    _definitionSections.clear();
}
