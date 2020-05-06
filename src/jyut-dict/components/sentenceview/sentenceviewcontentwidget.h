#ifndef SENTENCEVIEWCONTENTWIDGET_H
#define SENTENCEVIEWCONTENTWIDGET_H

#include "components/sentenceview/sentenceviewsentencecardsection.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/sentence/sourcesentence.h"

#include <QVBoxLayout>
#include <QWidget>

#include <vector>

// The EntryContentWidget displays data about an Entry (that is not in its header)
// It contains multiple DefinitionCardWidgets, one for each dictionary source

class SentenceViewContentWidget : public QWidget
{
public:
    explicit SentenceViewContentWidget(QWidget *parent = nullptr);
    ~SentenceViewContentWidget() override;

    void setSourceSentence(const SourceSentence &sentence);

private:
    QVBoxLayout *_entryContentLayout;
    SentenceViewSentenceCardSection *_sentenceSection;

public slots:
    void hideSentenceSection(void);
    void showSentenceSection(void);
};

#endif // SENTENCECONTENTWIDGET_H
