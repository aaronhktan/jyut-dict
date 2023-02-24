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
                                QWidget *parent = nullptr);

    void setEntry(const Entry &entry);

private:
    QVBoxLayout *_entryContentLayout;
    DefinitionCardSection *_definitionSection;
    EntryViewSentenceCardSection *_sentenceSection;
    RelatedSection *_relatedSection;

signals:
    void stallSentenceUIUpdate(void);
    void viewAllSentences(void);

public slots:
    void hideDefinitionSection(void);
    void showDefinitionSection(void);

    void hideSentenceSection(void);
    void showSentenceSection(void);

    void hideRelatedSection(void);
    void showRelatedSection(void);

    void updateStyleRequested(void);

    void viewAllSentencesRequested(void);
};

#endif // ENTRYCONTENTWIDGET_H
