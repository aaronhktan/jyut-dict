#ifndef SENTENCECARDWIDGET_H
#define SENTENCECARDWIDGET_H

#include "components/sentencecard/sentenceheaderwidget.h"
#include "components/sentencecard/sentencecontentwidget.h"
#include "logic/sentence/sourcesentence.h"

#include <QEvent>
#include <QWidget>

#include <vector>

class SentenceCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SentenceCardWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void displaySentences(const std::vector<SourceSentence> &sentences);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::vector<SourceSentence> _sourceSentences;
    std::string _source;

    QVBoxLayout *_sentenceCardLayout;
    SentenceHeaderWidget *_sentenceHeaderWidget;
    SentenceContentWidget *_sentenceContentWidget;
};

#endif // SENTENCECARDWIDGET_H
