#ifndef SENTENCECARDWIDGET_H
#define SENTENCECARDWIDGET_H

#include "components/sentencecard/sentenceheaderwidget.h"
#include "components/sentencecard/sentencecontentwidget.h"
#include "logic/sentence/sourcesentence.h"

#include <QEvent>
#include <QWidget>

#include <vector>

// A SentenceCardWidget contains a header (showing that this card is used for
// sentences), and content (showing the actual content of the sentences)

class SentenceCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SentenceCardWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void displaySentences(const std::vector<SourceSentence> &sentences);
    void displaySentences(const SentenceSet &set);

private:
    void translateUI();
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::vector<SourceSentence> _sourceSentences;
    bool _sourceSentencesIsValid = false;
    std::string _source;

    QVBoxLayout *_sentenceCardLayout;
    SentenceHeaderWidget *_sentenceHeaderWidget;
    SentenceContentWidget *_sentenceContentWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // SENTENCECARDWIDGET_H
