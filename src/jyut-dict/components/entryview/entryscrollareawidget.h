#ifndef ENTRYSCROLLAREAWIDGET_H
#define ENTRYSCROLLAREAWIDGET_H

#include "components/entryview/entryactionwidget.h"
#include "components/entryview/entrycontentwidget.h"
#include "components/entryview/entryheaderwidget.h"
#include "logic/database/sqldatabasemanager.h"

#include <QEvent>
#include <QGridLayout>
#include <QWidget>

// The DefinitionScrollAreaWidget is the widget that contains other widgets
// for the DefinitionScrollArea to pan and view.

class EntryScrollAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EntryScrollAreaWidget(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                                   std::shared_ptr<SQLDatabaseManager> manager,
                                   QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void setStyle(bool use_dark);

    bool _paletteRecentlyChanged = false;

    std::shared_ptr<SQLUserDataUtils> _sqlUserUtils;
    std::shared_ptr<SQLDatabaseManager> _manager;
    Entry _entry;
    bool _entryIsValid = false;

    QGridLayout *_scrollAreaLayout;

    EntryHeaderWidget *_entryHeaderWidget;
    EntryActionWidget *_entryActionWidget;
    EntryContentWidget *_entryContentWidget;

signals:
    void stallUISentenceUpdate(void);
    void favouriteCurrentEntry(void);
    void shareCurrentEntry(void);
    void viewAllSentences(void);
    void searchQuery(QString query, SearchParameters parameters);

public slots:
    void updateStyleRequested(void);
    void favouriteCurrentEntryRequested(void);
    void shareCurrentEntryRequested(void);
    void openInNewWindow(void);
    void openMagnifyWindow(void);
    void viewAllSentencesRequested(void);
    void searchQueryRequested(QString query, SearchParameters parameters);
};

#endif // ENTRYSCROLLAREAWIDGET_H
