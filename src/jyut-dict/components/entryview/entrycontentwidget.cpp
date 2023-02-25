#include "entrycontentwidget.h"

EntryContentWidget::EntryContentWidget(
    std::shared_ptr<SQLDatabaseManager> manager,
    bool showRelatedSection,
    QWidget *parent)
    : QWidget(parent)
{
    _entryContentLayout = new QVBoxLayout{this};
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(0);

    _definitionSection = new DefinitionCardSection{this};
    _sentenceSection = new EntryViewSentenceCardSection{manager, this};
    _relatedSection = new RelatedSection{this};

    _entryContentLayout->addWidget(_definitionSection);
    _entryContentLayout->addWidget(_sentenceSection);
    _entryContentLayout->addWidget(_relatedSection);

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

    connect(_definitionSection,
            &DefinitionCardSection::addingCards,
            this,
            &EntryContentWidget::hideRelatedSection);

    if (showRelatedSection) {
        connect(_sentenceSection,
                &EntryViewSentenceCardSection::finishedAddingCards,
                this,
                &EntryContentWidget::showRelatedSection);

        connect(_sentenceSection,
                &EntryViewSentenceCardSection::noCardsAdded,
                this,
                &EntryContentWidget::showRelatedSection);
    }

    connect(this,
            &EntryContentWidget::stallSentenceUIUpdate,
            _sentenceSection,
            &EntryViewSentenceCardSection::stallSentenceUIUpdate);

    connect(this,
            &EntryContentWidget::viewAllSentences,
            _sentenceSection,
            &EntryViewSentenceCardSection::viewAllSentencesRequested);

    connect(_relatedSection,
            &RelatedSection::searchQuery,
            this,
            &EntryContentWidget::searchQueryRequested);
}

void EntryContentWidget::setEntry(const Entry &entry)
{
    _definitionSection->setEntry(entry);
    _sentenceSection->setEntry(entry);
    _relatedSection->setEntry(entry);
}

void EntryContentWidget::hideDefinitionSection(void)
{
    _definitionSection->setVisible(false);
}

void EntryContentWidget::showDefinitionSection(void)
{
    // This is an expensive call if there are many widgets
    // in the _definitionSection.
    // A workaround for jitter caused by this is located in EntryScrollArea.
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

void EntryContentWidget::hideRelatedSection(void)
{
    _relatedSection->setVisible(false);
}

void EntryContentWidget::showRelatedSection(void)
{
    _relatedSection->setVisible(true);
}

void EntryContentWidget::updateStyleRequested(void)
{
    _definitionSection->updateStyleRequested();
    _sentenceSection->updateStyleRequested();
    _relatedSection->updateStyleRequested();
}

void EntryContentWidget::viewAllSentencesRequested(void)
{
    emit viewAllSentences();
}

void EntryContentWidget::searchQueryRequested(const QString &query,
                                              const SearchParameters &parameters)
{
    emit searchQuery(query, parameters);
}
