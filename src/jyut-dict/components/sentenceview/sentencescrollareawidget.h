#ifndef SENTENCESCROLLAREAWIDGET_H
#define SENTENCESCROLLAREAWIDGET_H

#include "logic/sentence/sourcesentence.h"

#include <QSettings>
#include <QWidget>

#include <optional>

class SentenceViewContentWidget;
class SentenceViewHeaderWidget;

class QEvent;
class QVBoxLayout;

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

    std::unique_ptr<QSettings> _settings;
    std::optional<SourceSentence> _sentence;

    QVBoxLayout *_scrollAreaLayout;

    SentenceViewHeaderWidget *_sentenceViewHeaderWidget;
    SentenceViewContentWidget *_sentenceViewContentWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // SENTENCESCROLLAREAWIDGET_H
