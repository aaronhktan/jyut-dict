#ifndef SENTENCESCROLLAREA_H
#define SENTENCESCROLLAREA_H

#include "components/sentenceview/sentencescrollareawidget.h"
#include "logic/sentence/sourcesentence.h"

#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

// The SentenceScrollArea is the "detail" view
// It displays an SourceSentence object in the user interface

class SentenceScrollArea : public QScrollArea
{
public:
    explicit SentenceScrollArea(QWidget *parent = nullptr);
    ~SentenceScrollArea() override;

    void setSourceSentence(const SourceSentence &sentence);
private:
    void resizeEvent(QResizeEvent *event) override;

    SentenceScrollAreaWidget *_scrollAreaWidget;
};

#endif // SENTENCESCROLLAREA_H
