#ifndef DEFINITIONCARDWIDGET_H
#define DEFINITIONCARDWIDGET_H

#include "components/definitioncard/definitioncontentwidget.h"
#include "components/definitioncard/definitionheaderwidget.h"
#include "logic/entry/definitionsset.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

// The DefinitionCardWidget is a group that displays a set of definitions
// It contains a header (that displays the source), and the definition content

class DefinitionCardWidget : public QWidget
{
public:
    explicit DefinitionCardWidget(QWidget *parent = nullptr);
    ~DefinitionCardWidget() override;

    void changeEvent(QEvent *event) override;

    void setEntry(const DefinitionsSet &definitionsSet);

private:
    void translateUI();
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;
    std::string _source;

    QVBoxLayout *_definitionAreaLayout;
    DefinitionHeaderWidget *_definitionHeaderWidget;
    DefinitionContentWidget *_definitionWidget;
};

#endif // DEFINITIONCARDWIDGET_H
