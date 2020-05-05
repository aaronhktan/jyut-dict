#ifndef SENTENCESCROLLAREA_H
#define SENTENCESCROLLAREA_H

#include "components/sentenceview/sentencescrollareawidget.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/sentence/sourcesentence.h"

#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

// The EntryScrollArea is the "detail" view
// It displays an Entry object in the user interface

// The layout of the EntryScrollArea is:
// EntryHeader - shows word in chinese, pronunciation guides
// EntryContentWidget - contains all the definitions and headers for each source

class SentenceScrollArea : public QScrollArea
{
public:
    explicit SentenceScrollArea(QWidget *parent = nullptr);
    ~SentenceScrollArea() override;

    void setSourceSentence(const SourceSentence &sentence);
private:
    void resizeEvent(QResizeEvent *event) override;

    // Widget that contains elements to scroll
    SentenceScrollAreaWidget *_scrollAreaWidget;
};

#endif // SENTENCESCROLLAREA_H
