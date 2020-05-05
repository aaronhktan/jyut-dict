#ifndef ENTRYCONTENTWIDGET_H
#define ENTRYCONTENTWIDGET_H

#include "components/definitioncard/definitioncardsection.h"
#include "components/entryview/entryviewsentencecardsection.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"

#include <QVBoxLayout>
#include <QWidget>

#include <vector>

// The EntryContentWidget displays data about an Entry (that is not in its header)
// It contains multiple DefinitionCardWidgets, one for each dictionary source

class EntryContentWidget : public QWidget
{
public:
    explicit EntryContentWidget(std::shared_ptr<SQLDatabaseManager> manager,
                                QWidget *parent = nullptr);
    ~EntryContentWidget() override;

    void setEntry(const Entry &entry);

private:
    QVBoxLayout *_entryContentLayout;
    DefinitionCardSection *_definitionSection;
    EntryViewSentenceCardSection *_sentenceSection;

public slots:
    void hideDefinitionSection(void);
    void showDefinitionSection(void);

    void hideSentenceSection(void);
    void showSentenceSection(void);
};

#endif // ENTRYCONTENTWIDGET_H
