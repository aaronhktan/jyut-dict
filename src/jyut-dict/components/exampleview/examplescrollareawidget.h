#ifndef EXAMPLESCROLLAREAWIDGET_H
#define EXAMPLESCROLLAREAWIDGET_H

#include "logic/example/example.h"

#include <QSettings>
#include <QWidget>

#include <optional>

class ExampleViewContentWidget;
class ExampleViewHeaderWidget;

class QEvent;
class QVBoxLayout;

// The ExampleScrollAreaWidget is the widget that contains other widgets
// for the ExampleScrollArea to pan and view.

class ExampleScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleScrollAreaWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setExample(const Example &example);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;
    std::optional<Example> _example;

    QVBoxLayout *_scrollAreaLayout;

    ExampleViewHeaderWidget *_exampleViewHeaderWidget;
    ExampleViewContentWidget *_exampleViewContentWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // EXAMPLESCROLLAREAWIDGET_H
