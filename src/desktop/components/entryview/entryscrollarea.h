#ifndef ENTRYSCROLLAREA_H
#define ENTRYSCROLLAREA_H

#include "logic/search/searchparameters.h"

#include <QScrollArea>

class Entry;
class SQLDatabaseManager;
class SQLUserDataUtils;
class EntryScrollAreaWidget;

class QVBoxLayout;
class QResizeEvent;

// The EntryScrollArea is the "detail" view for a selected Entry
// (The actual logic is in EntryScrollAreaWidget.cpp)

class EntryScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit EntryScrollArea(std::shared_ptr<SQLUserDataUtils> sqlUserUtils,
                             std::shared_ptr<SQLDatabaseManager> manager,
                             QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;

    void setEntry(const Entry &entry);

private:
    void resizeEvent(QResizeEvent *event) override;

    QTimer *_enableUIUpdateTimer;
    QTimer *_updateUITimer;
    // Unlike EntryViewSentenceCardSection, this is set to true by default
    // because there are situations where setEntry() is called _without_
    // the stallEntryUIUpdate slot being called (which would set this variable
    // to true), such as the Favourites window.
    bool _enableUIUpdate = true;

    EntryScrollAreaWidget *_scrollAreaWidget;

signals:
    void stallSentenceUIUpdate(void);

    void favouriteCurrentEntry(void);
    void shareCurrentEntry(void);
    void openCurrentEntryInNewWindow(void);
    void magnifyCurrentEntry(void);
    void viewAllSentences(void);

    void searchEntriesBeginning(void);
    void searchEntriesContaining(void);
    void searchEntriesEnding(void);

    void searchQuery(const QString &query, const SearchParameters &parameters);

public slots:
    void stallEntryUIUpdate(void);
    void updateStyleRequested(void);

    void favouriteCurrentEntryRequested(void);
    void shareCurrentEntryRequested(void);
    void openCurrentEntryInNewWindowRequested(void);
    void magnifyCurrentEntryRequested(void);
    void viewAllSentencesRequested(void);

    void searchEntriesBeginningRequested(void);
    void searchEntriesContainingRequested(void);
    void searchEntriesEndingRequested(void);

    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters);
};

#endif // ENTRYSCROLLAREA_H
