#ifndef ENTRYSCROLLAREAWIDGET_H
#define ENTRYSCROLLAREAWIDGET_H

#include "logic/database/sqldatabasemanager.h"
#include "logic/entry/entry.h"
#include "logic/search/searchparameters.h"

#include <QSettings>
#include <QWidget>

#include <optional>

class EntryActionWidget;
class EntryContentWidget;
class EntryHeaderWidget;
class SQLDatabaseManager;
class SQLUserDataUtils;

class QEvent;
class QGridLayout;

// The EntryScrollAreaWidget is the widget that contains other widgets
// for the EntryScrollArea to pan and view.

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
    std::unique_ptr<QSettings> _settings;
    std::optional<Entry> _entry = std::nullopt;

    QGridLayout *_scrollAreaLayout;

    EntryHeaderWidget *_entryHeaderWidget;
    EntryActionWidget *_entryActionWidget;
    EntryContentWidget *_entryContentWidget;

signals:
    void stallUISentenceUpdate(void);

    void favouriteCurrentEntry(void);
    void shareCurrentEntry(void);
    void viewAllSentences(void);

    void searchEntriesBeginning(void);
    void searchEntriesContaining(void);
    void searchEntriesEnding(void);

    void searchQuery(const QString &query, const SearchParameters &parameters);

public slots:
    void updateStyleRequested(void);

    void favouriteCurrentEntryRequested(void);
    void shareCurrentEntryRequested(void);
    void openInNewWindow(void);
    void openMagnifyWindow(void);
    void viewAllSentencesRequested(void);

    void searchEntriesBeginningRequested(void);
    void searchEntriesContainingRequested(void);
    void searchEntriesEndingRequested(void);

    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters);
};

#endif // ENTRYSCROLLAREAWIDGET_H
