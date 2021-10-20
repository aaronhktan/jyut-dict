#include "sentenceviewsentencecardsection.h"

SentenceViewSentenceCardSection::SentenceViewSentenceCardSection(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
}

void SentenceViewSentenceCardSection::setSourceSentence(const SourceSentence &sentence)
{
    cleanup();

    std::vector<SentenceSet> sentenceSets = sentence.getSentenceSets();

    // This prevents an extra space from being added at the bottom when there
    // is nothing to display in the sentence card section.
    if (sentenceSets.empty()) {
        _sentenceCardsLayout->setContentsMargins(0, 0, 0, 0);
    } else {
        _sentenceCardsLayout->setContentsMargins(0, 11, 0, 0);
    }

    emit addingCards();
    for (const auto &set : sentenceSets) {
        if (set.isEmpty()) {
            continue;
        }

        _sentenceCards.push_back(new SentenceCardWidget{this});
        _sentenceCards.back()->displaySentences(set);

        _sentenceCardsLayout->addWidget(_sentenceCards.back(), Qt::AlignHCenter);
    }
    emit finishedAddingCards();
}

void SentenceViewSentenceCardSection::setupUI(void)
{
    _sentenceCardsLayout = new QVBoxLayout{this};
    _sentenceCardsLayout->setContentsMargins(0, 0, 0, 0);
    _sentenceCardsLayout->setSpacing(11);
}

void SentenceViewSentenceCardSection::cleanup(void)
{
    for (auto card : _sentenceCards) {
        _sentenceCardsLayout->removeWidget(card);
        delete card;
    }
    _sentenceCards.clear();
}
