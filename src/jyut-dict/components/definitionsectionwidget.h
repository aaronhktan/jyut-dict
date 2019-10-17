#ifndef DEFINITIONSECTIONWIDGET_H
#define DEFINITIONSECTIONWIDGET_H

#include "components/definitioncontentwidget.h"
#include "components/definitionheaderwidget.h"
#include "logic/entry/definitionsset.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

// The DefinitionSectionWidget is a group that displays a set of definitions
// It contains a header (that displays the source), and the definition content

class DefinitionSectionWidget : public QWidget
{
public:
    explicit DefinitionSectionWidget(QWidget *parent = nullptr);
    ~DefinitionSectionWidget() override;

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

#endif // DEFINITIONSECTIONWIDGET_H
