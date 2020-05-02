#ifndef ENTRYCONTENTWIDGET_H
#define ENTRYCONTENTWIDGET_H

#include "components/definitioncard/definitioncardwidget.h"
#include "logic/entry/entry.h"

#include <QVBoxLayout>
#include <QWidget>

#include <vector>

// The EntryContentWidget displays data about an Entry (that is not in its header)
// It contains multiple DefinitionCardWidgets, one for each dictionary source

class EntryContentWidget : public QWidget
{
public:
    explicit EntryContentWidget(QWidget *parent = nullptr);
    explicit EntryContentWidget(Entry &entry, QWidget *parent = nullptr);
    ~EntryContentWidget() override;

    void setEntry(const Entry &entry);

private:
    void cleanup();

    QVBoxLayout *_definitionSectionsLayout;
    std::vector<DefinitionCardWidget *> _definitionSections;
};

#endif // ENTRYCONTENTWIDGET_H
