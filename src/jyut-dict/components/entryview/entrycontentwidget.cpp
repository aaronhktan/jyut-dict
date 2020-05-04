#include "entrycontentwidget.h"

EntryContentWidget::EntryContentWidget(std::shared_ptr<SQLDatabaseManager> manager,
                                       QWidget *parent)
    : QWidget(parent)
{
    _entryContentLayout = new QVBoxLayout{this};
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(15);

    _definitionSection = new DefinitionCardSection{this};
    _sentenceSection = new SentenceCardSection{manager, this};

    _entryContentLayout->addWidget(_definitionSection);
    _entryContentLayout->addWidget(_sentenceSection);
}

EntryContentWidget::~EntryContentWidget()
{

}

void EntryContentWidget::setEntry(const Entry &entry)
{
    _definitionSection->setEntry(entry);
    _sentenceSection->setEntry(entry);

    // Force layout to update after adding widgets; fixes some layout issues.
    _entryContentLayout->activate();
}
