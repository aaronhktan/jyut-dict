#include "entrycontentwidget.h"

EntryContentWidget::EntryContentWidget(std::shared_ptr<SQLDatabaseManager> manager,
                                       QWidget *parent)
    : QWidget(parent)
{
    _entryContentLayout = new QVBoxLayout{this};
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(0);

    _definitionSection = new DefinitionCardSection{this};
    _sentenceSection = new EntryViewSentenceCardSection{manager, this};

    _entryContentLayout->addWidget(_definitionSection);
    _entryContentLayout->addWidget(_sentenceSection);

    connect(_definitionSection,
            &DefinitionCardSection::addingCards,
            this,
            &EntryContentWidget::hideDefinitionSection);

    connect(_definitionSection,
            &DefinitionCardSection::finishedAddingCards,
            this,
            &EntryContentWidget::showDefinitionSection);

    connect(_sentenceSection,
            &EntryViewSentenceCardSection::addingCards,
            this,
            &EntryContentWidget::hideSentenceSection);

    connect(_sentenceSection,
            &EntryViewSentenceCardSection::finishedAddingCards,
            this,
            &EntryContentWidget::showSentenceSection);

#ifndef Q_OS_LINUX
    connect(this,
            &EntryContentWidget::stallUIUpdate,
            _sentenceSection,
            &EntryViewSentenceCardSection::stallUIUpdate);
#endif
}

EntryContentWidget::~EntryContentWidget()
{

}

void EntryContentWidget::setEntry(const Entry &entry)
{
    _definitionSection->setEntry(entry);
    _sentenceSection->setEntry(entry);
}

void EntryContentWidget::hideDefinitionSection(void)
{
    _definitionSection->setVisible(false);
}

void EntryContentWidget::showDefinitionSection(void)
{
    _definitionSection->setVisible(true);
}


void EntryContentWidget::hideSentenceSection(void)
{
    _sentenceSection->setVisible(false);
}

void EntryContentWidget::showSentenceSection(void)
{
    _sentenceSection->setVisible(true);
}
