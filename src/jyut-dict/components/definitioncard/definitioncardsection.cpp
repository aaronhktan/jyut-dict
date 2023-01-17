#include "definitioncardsection.h"

DefinitionCardSection::DefinitionCardSection(QWidget *parent) : QWidget(parent)
{
    _definitionCardsLayout = new QVBoxLayout{this};
    _definitionCardsLayout->setContentsMargins(0, 0, 0, 0);
    _definitionCardsLayout->setSpacing(11);
}

void DefinitionCardSection::setEntry(const Entry &entry)
{
    emit addingCards();
    cleanup();
    for (auto &definitionsSet : entry.getDefinitionsSets()) {
        _definitionCards.push_back(new DefinitionCardWidget{this});
        _definitionCards.back()->setEntry(definitionsSet);

        _definitionCardsLayout->addWidget(_definitionCards.back());
    }
    emit finishedAddingCards();
}

void DefinitionCardSection::cleanup(void)
{
    for (auto card : _definitionCards) {
        _definitionCardsLayout->removeWidget(card);
        delete card;
    }
    _definitionCards.clear();
}

void DefinitionCardSection::updateStyleRequested(void)
{
    for (auto &card : _definitionCards) {
        card->updateStyleRequested();
    }
}
