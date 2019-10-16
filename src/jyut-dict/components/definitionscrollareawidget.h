#ifndef DEFINITIONSCROLLAREAWIDGET_H
#define DEFINITIONSCROLLAREAWIDGET_H

#include "components/definitionwidget.h"
#include "components/entryheaderwidget.h"

#include <QVBoxLayout>
#include <QWidget>

class DefinitionScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefinitionScrollAreaWidget(QWidget *parent = nullptr);

    void setEntry(const Entry &entry);
private:
    QVBoxLayout *_scrollAreaLayout;

    EntryHeaderWidget *_entryHeaderWidget;
    DefinitionWidget *_definitionWidget;

signals:

public slots:
};

#endif // DEFINITIONSCROLLAREAWIDGET_H
