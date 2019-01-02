#ifndef DEFINITIONWIDGET_H
#define DEFINITIONWIDGET_H

#include "components/definitionsectionwidget.h"
#include "logic/entry/entry.h"

#include <QVBoxLayout>
#include <QWidget>

class DefinitionWidget : public QWidget
{
public:
    explicit DefinitionWidget(QWidget *parent = nullptr);
    explicit DefinitionWidget(Entry& entry, QWidget *parent = nullptr);

    void setEntry(Entry& entry);

private:
    QVBoxLayout *_definitionSectionsLayout;
    std::vector<DefinitionSectionWidget *> _definitionSections;
};

#endif // DEFINITIONWIDGET_H
