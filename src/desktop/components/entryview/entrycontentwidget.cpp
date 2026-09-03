#include "entrycontentwidget.h"

#include "components/definitioncard/definitioncardsection.h"
#include "components/entryview/entryviewexamplecardsection.h"
#include "components/related/relatedsection.h"
#include "logic/database/sqldatabasemanager.h"

#include <QVBoxLayout>

EntryContentWidget::EntryContentWidget(
    std::shared_ptr<SQLDatabaseManager> manager,
    bool showRelatedSection,
    QWidget *parent)
    : QWidget{parent}
{
    _entryContentLayout = new QVBoxLayout{this};
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(0);

    _definitionSection = new DefinitionCardSection{this};
    _exampleSection = new EntryViewExampleCardSection{manager, this};
    _relatedSection = new RelatedSection{this};

    _entryContentLayout->addWidget(_definitionSection);
    _entryContentLayout->addWidget(_exampleSection);
    _entryContentLayout->addWidget(_relatedSection);

    connect(_definitionSection,
            &DefinitionCardSection::addingCards,
            this,
            &EntryContentWidget::hideDefinitionSection);

    connect(_definitionSection,
            &DefinitionCardSection::finishedAddingCards,
            this,
            &EntryContentWidget::showDefinitionSection);

    connect(_exampleSection,
            &EntryViewExampleCardSection::addingCards,
            this,
            &EntryContentWidget::hideExampleSection);

    connect(_exampleSection,
            &EntryViewExampleCardSection::finishedAddingCards,
            this,
            &EntryContentWidget::showExampleSection);

    connect(_definitionSection,
            &DefinitionCardSection::addingCards,
            this,
            &EntryContentWidget::hideRelatedSection);

    if (showRelatedSection) {
        connect(_exampleSection,
                &EntryViewExampleCardSection::finishedAddingCards,
                this,
                &EntryContentWidget::showRelatedSection);

        connect(_exampleSection,
                &EntryViewExampleCardSection::noCardsAdded,
                this,
                &EntryContentWidget::showRelatedSection);
    }

    connect(this,
            &EntryContentWidget::stallExampleUIUpdate,
            _exampleSection,
            &EntryViewExampleCardSection::stallExampleUIUpdate);

    connect(this,
            &EntryContentWidget::viewAllExamples,
            _exampleSection,
            &EntryViewExampleCardSection::viewAllExamplesRequested);

    connect(this,
            &EntryContentWidget::searchEntriesBeginning,
            _relatedSection,
            &RelatedSection::searchEntriesBeginningRequested);

    connect(this,
            &EntryContentWidget::searchEntriesContaining,
            _relatedSection,
            &RelatedSection::searchEntriesContainingRequested);

    connect(this,
            &EntryContentWidget::searchEntriesEnding,
            _relatedSection,
            &RelatedSection::searchEntriesEndingRequested);

    connect(_relatedSection,
            &RelatedSection::searchQuery,
            this,
            &EntryContentWidget::searchQueryRequested);
}

void EntryContentWidget::setEntry(const Entry &entry)
{
    _entry = entry;

    _definitionSection->setEntry(entry);
    _exampleSection->setEntry(entry);
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

void EntryContentWidget::hideExampleSection(void)
{
    _exampleSection->setVisible(false);
}

void EntryContentWidget::showExampleSection(void)
{
    _exampleSection->setVisible(true);
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
    if (_entry.has_value()) {
        bool relatedSectionIsVisible = _relatedSection->isVisible();
        // For some reason, setting the entry here makes the application not
        // flash when updating the style. Setting it in the individual definition
        // cards does.
        _definitionSection->setEntry(_entry.value());
        _relatedSection->setVisible(relatedSectionIsVisible);
    }

    _definitionSection->updateStyleRequested();
    _exampleSection->updateStyleRequested();
    _relatedSection->updateStyleRequested();
}

void EntryContentWidget::viewAllExamplesRequested(void)
{
    emit viewAllExamples();
}

void EntryContentWidget::searchEntriesBeginningRequested(void)
{
    emit searchEntriesBeginning();
}

void EntryContentWidget::searchEntriesContainingRequested(void)
{
    emit searchEntriesContaining();
}

void EntryContentWidget::searchEntriesEndingRequested(void)
{
    emit searchEntriesEnding();
}

void EntryContentWidget::searchQueryRequested(const QString &query,
                                              const SearchParameters &parameters)
{
    emit searchQuery(query, parameters);
}
