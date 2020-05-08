#ifndef SENTENCESCROLLAREAWIDGET_H
#define SENTENCESCROLLAREAWIDGET_H

#include "components/sentenceview/sentenceviewcontentwidget.h"
#include "components/sentenceview/sentenceviewheaderwidget.h"
#include "logic/sentence/sourcesentence.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

// The SentenceScrollAreaWidget is the widget that contains other widgets
// for the SentenceScrollArea to pan and view.

class SentenceScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SentenceScrollAreaWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setSourceSentence(const SourceSentence &sentence);
private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QVBoxLayout *_scrollAreaLayout;

    SentenceViewHeaderWidget *_sentenceViewHeaderWidget;
    SentenceViewContentWidget *_sentenceViewContentWidget;
};

#endif // SENTENCESCROLLAREAWIDGET_H
