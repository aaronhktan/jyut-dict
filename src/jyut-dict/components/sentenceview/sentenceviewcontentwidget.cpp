#include "sentenceviewcontentwidget.h"

SentenceViewContentWidget::SentenceViewContentWidget(QWidget *parent)
    : QWidget(parent)
{
    _entryContentLayout = new QVBoxLayout{this};
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(0);

    _sentenceSection = new SentenceViewSentenceCardSection{this};

    _entryContentLayout->addWidget(_sentenceSection);

    QObject::connect(_sentenceSection,
                     &SentenceViewSentenceCardSection::addingCards,
                     this,
                     &SentenceViewContentWidget::hideSentenceSection);

    QObject::connect(_sentenceSection,
                     &SentenceViewSentenceCardSection::finishedAddingCards,
                     this,
                     &SentenceViewContentWidget::showSentenceSection);
}

SentenceViewContentWidget::~SentenceViewContentWidget()
{

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

