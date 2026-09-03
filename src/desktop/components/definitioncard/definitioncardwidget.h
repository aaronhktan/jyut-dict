#ifndef DEFINITIONCARDWIDGET_H
#define DEFINITIONCARDWIDGET_H

#include <QWidget>

class DefinitionContentWidget;
class DefinitionHeaderWidget;
class DefinitionsSet;

class QEvent;
class QVBoxLayout;

// The DefinitionCardWidget is a group that displays a set of definitions
// It contains a header (that displays the source), and the definition content

class DefinitionCardWidget : public QWidget
{
public:
    explicit DefinitionCardWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setDefinitions(const DefinitionsSet &definitionsSet);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QVBoxLayout *_definitionAreaLayout;
    DefinitionHeaderWidget *_definitionHeaderWidget;
    DefinitionContentWidget *_definitionContentWidget;

public slots:
    void updateStyleRequested(void);
};

#endif // DEFINITIONCARDWIDGET_H
