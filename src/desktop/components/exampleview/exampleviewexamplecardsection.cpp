#include "exampleviewexamplecardsection.h"

#include "components/examplecard/examplecardwidget.h"

ExampleViewExampleCardSection::ExampleViewExampleCardSection(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
}

void ExampleViewExampleCardSection::setExample(const Example &example)
{
    cleanup();

    std::vector<TranslationSet> TranslationSets;
    TranslationSets.assign(example.getTranslationSets().begin(),
                           example.getTranslationSets().end());

    // This prevents an extra space from being added at the bottom when there
    // is nothing to display in the example card section.
    if (TranslationSets.empty()) {
        _exampleCardsLayout->setContentsMargins(0, 0, 0, 0);
    } else {
        _exampleCardsLayout->setContentsMargins(0, 11, 0, 0);
    }

    emit addingCards();
    for (const auto &set : TranslationSets) {
        if (set.isEmpty()) {
            continue;
        }

        _exampleCards.push_back(new ExampleCardWidget{this});
        _exampleCards.back()->displayTranslations(set);

        _exampleCardsLayout->addWidget(_exampleCards.back(), Qt::AlignHCenter);
    }
    emit finishedAddingCards();
}

void ExampleViewExampleCardSection::setupUI(void)
{
    _exampleCardsLayout = new QVBoxLayout{this};
    _exampleCardsLayout->setContentsMargins(0, 0, 0, 0);
    _exampleCardsLayout->setSpacing(11);
}

void ExampleViewExampleCardSection::cleanup(void)
{
    for (const auto &card : _exampleCards) {
        _exampleCardsLayout->removeWidget(card);
        delete card;
    }
    _exampleCards.clear();
}

void ExampleViewExampleCardSection::updateStyleRequested(void)
{
    for (auto &card : _exampleCards) {
        card->updateStyleRequested();
    }
}
