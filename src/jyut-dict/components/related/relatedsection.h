#ifndef RELATEDSECTION_H
#define RELATEDSECTION_H

#include "logic/entry/entry.h"
#include "logic/search/searchparameters.h"

#include <QPushButton>
#include <QWidget>

class RelatedSection : public QWidget
{
    Q_OBJECT
public:
    explicit RelatedSection(QWidget *parent = nullptr);

    void setEntry(const Entry &entry);

private:
    QPushButton *_searchBeginningButton;
    QPushButton *_searchContainingButton;
    QPushButton *_searchEndingButton;

signals:
    void searchQuery(QString query, SearchParameters parameters);
};

#endif // RELATEDSECTION_H
