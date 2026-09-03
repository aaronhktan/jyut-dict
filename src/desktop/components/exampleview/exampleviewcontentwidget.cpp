#include "exampleviewcontentwidget.h"

#include "components/exampleview/exampleviewexamplecardsection.h"
#include "logic/example/example.h"

#include <QVBoxLayout>

ExampleViewContentWidget::ExampleViewContentWidget(QWidget *parent)
    : QWidget{parent}
    , _entryContentLayout{new QVBoxLayout{this}}
    , _exampleSection{new ExampleViewExampleCardSection{this}}
{
    _entryContentLayout->setContentsMargins(0, 0, 0, 0);
    _entryContentLayout->setSpacing(0);

    _entryContentLayout->addWidget(_exampleSection);

    // These are a workaround for flickering in Qt when resizing a widget
    // By hiding a widget before it is resized and then showing it after,
    // the flickering is removed.
    QObject::connect(_exampleSection,
                     &ExampleViewExampleCardSection::addingCards,
                     this,
                     &ExampleViewContentWidget::hideExampleSection);

    QObject::connect(_exampleSection,
                     &ExampleViewExampleCardSection::finishedAddingCards,
                     this,
                     &ExampleViewContentWidget::showExampleSection);
}

void ExampleViewContentWidget::setExample(const Example &example)
{
    _exampleSection->setExample(example);
}

void ExampleViewContentWidget::hideExampleSection(void)
{
    _exampleSection->setVisible(false);
}

void ExampleViewContentWidget::showExampleSection(void)
{
    _exampleSection->setVisible(true);
}

void ExampleViewContentWidget::updateStyleRequested(void)
{
    _exampleSection->updateStyleRequested();
}
