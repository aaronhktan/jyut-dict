#ifndef ISEARCHOPTIONSMEDIATOR_H
#define ISEARCHOPTIONSMEDIATOR_H

#include "components/isearchlineedit.h"
#include "logic/search/searchparameters.h"

// Interface for mediator between search parameter input and output
// The parameters are set with setParameters, then
// the lineedit is informed of changes to search parameters

class ISearchOptionsMediator {
public:
    virtual void registerLineEdit(ISearchLineEdit *_searchEdit) = 0;

    virtual void setParameters(SearchParameters parameters) = 0;
    virtual SearchParameters getParameters() = 0;
};

#endif // ISEARCHOPTIONSMEDIATOR_H
