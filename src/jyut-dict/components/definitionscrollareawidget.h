#ifndef DEFINITIONSCROLLAREAWIDGET_H
#define DEFINITIONSCROLLAREAWIDGET_H

#include "components/definitionwidget.h"
#include "components/entryheaderwidget.h"
#include "components/loadingwidget.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

// The DefinitionScrollAreaWidget is the widget that contains other widgets
// for the DefinitionScrollArea to pan and view.

class DefinitionScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefinitionScrollAreaWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);
private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QVBoxLayout *_scrollAreaLayout;

    EntryHeaderWidget *_entryHeaderWidget;
    DefinitionWidget *_definitionWidget;
    LoadingWidget *_loadingWidget;

signals:

public slots:
};

#endif // DEFINITIONSCROLLAREAWIDGET_H
