#ifndef EXAMPLEVIEWEXAMPLECARDSECTION_H
#define EXAMPLEVIEWEXAMPLECARDSECTION_H

#include "logic/example/example.h"

#include <QVBoxLayout>
#include <QWidget>

#include <vector>

class ExampleCardWidget;

// The ExampleViewExampleCardSection displays several ExampleCardWidgets,
// one for each TranslationSet in the Example.

class ExampleViewExampleCardSection : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleViewExampleCardSection(QWidget *parent = nullptr);

    void setExample(const Example &example);

private:
    void setupUI(void);
    void cleanup(void);

    std::vector<Example> _examples;

    QVBoxLayout *_exampleCardsLayout;
    std::vector<ExampleCardWidget *> _exampleCards;

signals:
    void addingCards();
    void finishedAddingCards();

public slots:
    void updateStyleRequested(void);
};

#endif // EXAMPLEVIEWEXAMPLECARDSECTION_H
