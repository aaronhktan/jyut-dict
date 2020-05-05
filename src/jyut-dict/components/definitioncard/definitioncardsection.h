#ifndef DEFINITIONCARDSECTION_H
#define DEFINITIONCARDSECTION_H

#include "components/definitioncard/definitioncardwidget.h"
#include "logic/entry/entry.h"

#include <QVBoxLayout>
#include <QWidget>

class DefinitionCardSection : public QWidget
{
    Q_OBJECT
public:
    explicit DefinitionCardSection(QWidget *parent = nullptr);

    void setEntry(const Entry &entry);

private:
    void cleanup(void);

    QVBoxLayout *_definitionCardsLayout;
    std::vector<DefinitionCardWidget *> _definitionCards;

signals:
    void addingCards(void);
    void finishedAddingCards(void);
};

#endif // DEFINITIONCARDSECTION_H
