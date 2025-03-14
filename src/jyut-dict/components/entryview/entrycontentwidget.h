#ifndef ENTRYCONTENTWIDGET_H
#define ENTRYCONTENTWIDGET_H

#include "components/definitioncard/definitioncardsection.h"
#include "components/entryview/entryviewsentencecardsection.h"
#include "components/related/relatedsection.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"

#include <QVBoxLayout>
#include <QWidget>

// The EntryContentWidget displays data about an Entry (that is not in its header)
// It contains section for definition cards, and another section for
// sentence cards.

class EntryContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EntryContentWidget(std::shared_ptr<SQLDatabaseManager> manager,
                                bool showRelatedSection = true,
                                QWidget *parent = nullptr);

    void setEntry(const Entry &entry);

private:
    Entry _entry;
    bool _entryIsValid = false;

    QVBoxLayout *_entryContentLayout;
    DefinitionCardSection *_definitionSection;
    EntryViewSentenceCardSection *_sentenceSection;
    RelatedSection *_relatedSection;

signals:
    void stallSentenceUIUpdate(void);

    void viewAllSentences(void);

    void searchEntriesBeginning(void);
    void searchEntriesContaining(void);
    void searchEntriesEnding(void);

    void searchQuery(const QString &query, const SearchParameters &parameters);

public slots:
    void hideDefinitionSection(void);
    void showDefinitionSection(void);

    void hideSentenceSection(void);
    void showSentenceSection(void);

    void hideRelatedSection(void);
    void showRelatedSection(void);

    void updateStyleRequested(void);

    void viewAllSentencesRequested(void);

    void searchEntriesBeginningRequested(void);
    void searchEntriesContainingRequested(void);
    void searchEntriesEndingRequested(void);

    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters);
};

#endif // ENTRYCONTENTWIDGET_H
