#ifndef ENTRYCONTENTWIDGET_H
#define ENTRYCONTENTWIDGET_H

#include "logic/entry/entry.h"
#include "logic/search/searchparameters.h"

#include <QWidget>

#include <optional>

class SQLDatabaseManager;
class RelatedSection;
class DefinitionCardSection;
class EntryViewSentenceCardSection;

class QVBoxLayout;

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
    std::optional<Entry> _entry = std::nullopt;

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
