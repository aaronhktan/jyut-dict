#ifndef ISEARCHOPTIONSMEDIATOR_H
#define ISEARCHOPTIONSMEDIATOR_H

#include "components/isearchlineedit.h"
#include "logic/search/searchparameters.h"

class ISearchOptionsMediator {
public:
    virtual void registerLineEdit(ISearchLineEdit *_searchEdit) = 0;

    virtual void setParameters(SearchParameters parameters) = 0;
    virtual SearchParameters getParameters() = 0;
};

#endif // ISEARCHOPTIONSMEDIATOR_H
