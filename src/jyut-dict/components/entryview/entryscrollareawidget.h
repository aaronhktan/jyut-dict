#ifndef ENTRYSCROLLAREAWIDGET_H
#define ENTRYSCROLLAREAWIDGET_H

#include "components/entryview/entrycontentwidget.h"
#include "components/entryview/entryheaderwidget.h"
#include "components/sentencecard/loadingwidget.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

// The DefinitionScrollAreaWidget is the widget that contains other widgets
// for the DefinitionScrollArea to pan and view.

class EntryScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EntryScrollAreaWidget(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);
private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    QVBoxLayout *_scrollAreaLayout;

    EntryHeaderWidget *_entryHeaderWidget;
    EntryContentWidget *_entryContentWidget;
    LoadingWidget *_loadingWidget;

signals:

public slots:
};

#endif // ENTRYSCROLLAREAWIDGET_H
