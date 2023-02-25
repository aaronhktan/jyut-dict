#ifndef RELATEDSECTION_H
#define RELATEDSECTION_H

#include "components/related/relatedbutton.h"
#include "logic/entry/entry.h"
#include "logic/search/searchparameters.h"

#include <QEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QSettings>
#include <QWidget>

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
    void searchQuery(const QString &query, const SearchParameters &parameters);

public slots:
    void updateStyleRequested(void);
    void searchQueryRequested(const QString &query,
                              const SearchParameters &parameters);
};

#endif // RELATEDSECTION_H
