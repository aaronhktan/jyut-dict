#include "sentenceviewcontentwidget.h"

SentenceViewContentWidget::SentenceViewContentWidget(QWidget *parent)
    : QWidget(parent)
{
    _entryContentLayout = new QVBoxLayout{this};
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(0);

    _sentenceSection = new SentenceViewSentenceCardSection{this};

    _entryContentLayout->addWidget(_sentenceSection);
}

SentenceViewContentWidget::~SentenceViewContentWidget()
{

}

void SentenceViewContentWidget::setSourceSentence(const SourceSentence &sentence)
{
    _sentenceSection->setSourceSentence(sentence);

    // Force layout to update after adding widgets; fixes some layout issues.
    _entryContentLayout->activate();
}
