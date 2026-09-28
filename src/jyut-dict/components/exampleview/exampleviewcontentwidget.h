#ifndef EXAMPLEVIEWCONTENTWIDGET_H
#define EXAMPLEVIEWCONTENTWIDGET_H

#include <QWidget>

class ExampleViewExampleCardSection;
class Example;

class QVBoxLayout;

// The ExampleViewContentWidget displays data about an Example (that is not in its header)
// It contains an ExampleSection that displays cards for examples

class ExampleViewContentWidget : public QWidget
{
public:
    explicit ExampleViewContentWidget(QWidget *parent = nullptr);

    void setExample(const Example &example);

private:
    QVBoxLayout *_entryContentLayout;
    ExampleViewExampleCardSection *_exampleSection;

public slots:
    void hideExampleSection(void);
    void showExampleSection(void);
    void updateStyleRequested(void);
};

#endif // EXAMPLECONTENTWIDGET_H
