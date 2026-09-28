#ifndef EXAMPLESCROLLAREA_H
#define EXAMPLESCROLLAREA_H

#include <QScrollArea>

class ExampleScrollAreaWidget;
class Example;

class QResizeEvent;
class QVBoxLayout;

// The ExampleScrollArea is the "detail" view
// It displays an Example object in the user interface

class ExampleScrollArea : public QScrollArea
{
public:
    explicit ExampleScrollArea(QWidget *parent = nullptr);

    void setExample(const Example &example);

private:
    void resizeEvent(QResizeEvent *event) override;

    ExampleScrollAreaWidget *_scrollAreaWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // EXAMPLESCROLLAREA_H
