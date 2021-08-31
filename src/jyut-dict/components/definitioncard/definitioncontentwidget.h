#ifndef DEFINITIONCONTENTWIDGET_H
#define DEFINITIONCONTENTWIDGET_H

#include "logic/entry/entry.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include <string>
#include <vector>

// The DefinitionContentWidget shows all the definitions
// It contains a collection of QLabels, one for each definition
// and its associated "number" (i.e. first definition, second definition, etc.)
// It also displays sentences and examples associated with each definition.

class DefinitionContentWidget : public QWidget
{
public:
    explicit DefinitionContentWidget(QWidget *parent = nullptr);
    ~DefinitionContentWidget() override;

    void changeEvent(QEvent *event) override;

    void setEntry(std::vector<Definition::Definition> definitions);

private:
    void setStyle(bool use_dark);

    void cleanupLabels();

    bool _paletteRecentlyChanged = false;

    QGridLayout *_definitionLayout;
    std::vector<QLabel *> _definitionNumberLabels;
    std::vector<QLabel *> _definitionLabels;
    std::vector<QLabel *> _exampleLabels;
    std::vector<QLabel *> _examplePronunciationLabels;
    std::vector<QLabel *> _exampleTranslationLabels;
    std::vector<QLabel *> _spaceLabels;
};

#endif // DEFINITIONCONTENTWIDGET_H
