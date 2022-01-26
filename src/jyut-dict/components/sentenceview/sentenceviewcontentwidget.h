#ifndef SENTENCEVIEWCONTENTWIDGET_H
#define SENTENCEVIEWCONTENTWIDGET_H

#include "components/sentenceview/sentenceviewsentencecardsection.h"
#include "logic/sentence/sourcesentence.h"

#include <QVBoxLayout>
#include <QWidget>

#include <vector>

// The SentenceViewContentWidget displays data about an Sentence (that is not in its header)
// It contains a SentenceSection that displays cards for sentences

class SentenceViewContentWidget : public QWidget
{
public:
    explicit SentenceViewContentWidget(QWidget *parent = nullptr);

    void setSourceSentence(const SourceSentence &sentence);

private:
    QVBoxLayout *_entryContentLayout;
    SentenceViewSentenceCardSection *_sentenceSection;

public slots:
    void hideSentenceSection(void);
    void showSentenceSection(void);
};

#endif // SENTENCECONTENTWIDGET_H
