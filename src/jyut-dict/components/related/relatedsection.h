#ifndef RELATEDSECTION_H
#define RELATEDSECTION_H

#include "logic/search/searchparameters.h"

#include <QSettings>
#include <QWidget>

class Entry;
class RelatedButton;

class QEvent;
class QGridLayout;
class QPushButton;

class RelatedSection : public QWidget
{
    Q_OBJECT
public:
    explicit RelatedSection(QWidget *parent = nullptr);

    void setVisible(bool visible) override;

    void setEntry(const Entry &entry);

private:
    void setupUI();

    QGridLayout *_relatedLayout;
    RelatedButton *_searchBeginningButton;
    RelatedButton *_searchContainingButton;
    RelatedButton *_searchEndingButton;

signals:
    void searchEntriesBeginning(void);
    void searchEntriesContaining(void);
    void searchEntriesEnding(void);

    void searchQuery(const QString &query, const SearchParameters &parameters);

public slots:
    void updateStyleRequested(void);

    void searchEntriesBeginningRequested(void);
    void searchEntriesContainingRequested(void);
    void searchEntriesEndingRequested(void);

    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters);
};

#endif // RELATEDSECTION_H
