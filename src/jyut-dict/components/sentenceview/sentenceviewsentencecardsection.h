#ifndef SENTENCEVIEWSENTENCECARDSECTION_H
#define SENTENCEVIEWSENTENCECARDSECTION_H

#include "components/sentencecard/sentencecardwidget.h"

#include <QVBoxLayout>
#include <QWidget>

#include <vector>

// The SentenceViewSentenceCardSection displays several SentenceCardWidgets,
// one for each SentenceSet in the SourceSentence.

class SentenceViewSentenceCardSection : public QWidget
{
    Q_OBJECT
public:
    explicit SentenceViewSentenceCardSection(QWidget *parent = nullptr);

    void setSourceSentence(const SourceSentence &sentence);

private:
    void setupUI(void);
    void cleanup(void);

    std::vector<SourceSentence> _sentences;

    QVBoxLayout *_sentenceCardsLayout;
    std::vector<SentenceCardWidget *> _sentenceCards;

signals:
    void addingCards();
    void finishedAddingCards();

public slots:
    void updateStyleRequested(void);
};

#endif // SENTENCEVIEWSENTENCECARDSECTION_H
