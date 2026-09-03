#ifndef SENTENCECARDWIDGET_H
#define SENTENCECARDWIDGET_H

#include "logic/sentence/sourcesentence.h"

#include <QWidget>

#include <optional>
#include <span>

class SentenceHeaderWidget;
class SentenceContentWidget;
class SourceSentence;

class QEvent;
class QVBoxLayout;

// A SentenceCardWidget contains a header (showing that this card is used for
// sentences), and content (showing the actual content of the sentences)

class SentenceCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SentenceCardWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void displaySentences(std::span<const SourceSentence> sentences);
    void displaySentences(const SentenceSet &set);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::optional<std::vector<SourceSentence>> _sourceSentences = std::nullopt;

    QVBoxLayout *_sentenceCardLayout;
    SentenceHeaderWidget *_sentenceHeaderWidget;
    SentenceContentWidget *_sentenceContentWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // SENTENCECARDWIDGET_H
