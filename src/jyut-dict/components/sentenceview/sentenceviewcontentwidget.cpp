#include "sentenceviewcontentwidget.h"

SentenceViewContentWidget::SentenceViewContentWidget(QWidget *parent)
    : QWidget(parent)
{
    _entryContentLayout = new QVBoxLayout{this};
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(0);

    _sentenceSection = new SentenceViewSentenceCardSection{this};

    _entryContentLayout->addWidget(_sentenceSection);

    // These are a workaround for flickering in Qt when resizing a widget
    // By hiding a widget before it is resized and then showing it after,
    // the flickering is removed.
    QObject::connect(_sentenceSection,
                     &SentenceViewSentenceCardSection::addingCards,
                     this,
                     &SentenceViewContentWidget::hideSentenceSection);

    QObject::connect(_sentenceSection,
                     &SentenceViewSentenceCardSection::finishedAddingCards,
                     this,
                     &SentenceViewContentWidget::showSentenceSection);
}

void SentenceViewContentWidget::setSourceSentence(const SourceSentence &sentence)
{
    _sentenceSection->setSourceSentence(sentence);
}

void SentenceViewContentWidget::hideSentenceSection(void)
{
    _sentenceSection->setVisible(false);
}

void SentenceViewContentWidget::showSentenceSection(void)
{
    _sentenceSection->setVisible(true);
}

void SentenceViewContentWidget::updateStyleRequested(void)
{
    _sentenceSection->updateStyleRequested();
}
