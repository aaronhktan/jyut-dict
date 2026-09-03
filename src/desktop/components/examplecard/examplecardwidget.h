#ifndef EXAMPLECARDWIDGET_H
#define EXAMPLECARDWIDGET_H

#include "logic/example/example.h"

#include <QWidget>

#include <optional>
#include <span>

class Example;
class ExampleHeaderWidget;
class ExampleContentWidget;

class QEvent;
class QVBoxLayout;

// A ExampleCardWidget contains a header (showing that this card is used for
// examples), and content (showing the actual content of the examples)

class ExampleCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleCardWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void displayExamples(std::span<const Example> examples);
    void displayTranslations(const TranslationSet &set);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::optional<std::vector<Example>> _examples = std::nullopt;

    QVBoxLayout *_exampleCardLayout;
    ExampleHeaderWidget *_exampleHeaderWidget;
    ExampleContentWidget *_exampleContentWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // EXAMPLECARDWIDGET_H
