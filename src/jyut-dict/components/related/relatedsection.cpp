#include "relatedsection.h"

#include <QTimer>

RelatedSection::RelatedSection(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
}

void RelatedSection::setVisible(bool visible)
{
    if (visible) {
        // Only have margin at top of this widget if is visible
        _relatedLayout->setContentsMargins(0, 11, 0, 0);
    } else {
        _relatedLayout->setContentsMargins(0, 0, 0, 0);
    }
    QWidget::setVisible(visible);
}

void RelatedSection::setEntry(const Entry &entry)
{
    _searchBeginningButton->setEntry(entry);
    _searchContainingButton->setEntry(entry);
    _searchEndingButton->setEntry(entry);
}

void RelatedSection::setupUI()
{
    _relatedLayout = new QGridLayout{this};
    _relatedLayout->setContentsMargins(0, 0, 0, 0);
    _relatedLayout->setSpacing(11);

    _searchBeginningButton = new RelatedButton{RelatedType::SearchBeginning,
                                               this};
    _searchContainingButton = new RelatedButton{RelatedType::SearchContaining,
                                                this};
    _searchEndingButton = new RelatedButton{RelatedType::SearchEnding, this};

    _relatedLayout->addItem(new QSpacerItem{0, 0, QSizePolicy::MinimumExpanding},
                            0,
                            0,
                            1,
                            1);
    _relatedLayout->addWidget(_searchBeginningButton, 0, 1, 1, 1);
    _relatedLayout->addWidget(_searchContainingButton, 0, 2, 1, 1);
    _relatedLayout->addWidget(_searchEndingButton, 0, 3, 1, 1);
    _relatedLayout->addItem(new QSpacerItem{0, 0, QSizePolicy::MinimumExpanding},
                            0,
                            4,
                            1,
                            1);

    _relatedLayout->setColumnStretch(1, 1);
    _relatedLayout->setColumnStretch(2, 1);
    _relatedLayout->setColumnStretch(3, 1);

    connect(_searchBeginningButton,
            &RelatedButton::searchQuery,
            this,
            &RelatedSection::searchQueryRequested);
    connect(_searchContainingButton,
            &RelatedButton::searchQuery,
            this,
            &RelatedSection::searchQueryRequested);
    connect(_searchEndingButton,
            &RelatedButton::searchQuery,
            this,
            &RelatedSection::searchQueryRequested);

    // Call base class setVisible to avoid virtual function disabling during
    // construction warning
    QWidget::setVisible(false);
}

void RelatedSection::searchQueryRequested(QString query,
                                          SearchParameters parameters)
{
    emit searchQuery(query, parameters);
}
