#ifndef ISEARCHOPTIONSMEDIATOR_H
#define ISEARCHOPTIONSMEDIATOR_H

#include "components/mainwindow/isearchlineedit.h"
#include "components/mainwindow/isearchoptionsselector.h"
#include "logic/search/searchparameters.h"

// Interface for mediator between search parameter input and output
// The parameters are set with setParameters, then
// the lineedit is informed of changes to search parameters

class ISearchOptionsMediator {
public:
    virtual ~ISearchOptionsMediator() = default;

    virtual void registerLineEdit(ISearchLineEdit *_searchEdit) = 0;
    virtual void registerOptionSelector(ISearchOptionsSelector *_selector) = 0;

    virtual void setParameters(SearchParameters parameters) = 0;
    virtual SearchParameters getParameters() = 0;
};

#endif // ISEARCHOPTIONSMEDIATOR_H
