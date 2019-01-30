#ifndef SEARCHOPTIONSMEDIATOR_H
#define SEARCHOPTIONSMEDIATOR_H

#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/searchparameters.h"

// Concrete implementation of SearchOptionsMediator interface
// See ISearchOptionsMediator.h for more details

class SearchOptionsMediator : public ISearchOptionsMediator
{
public:
    SearchOptionsMediator();

    void registerLineEdit(ISearchLineEdit *_searchEdit) override;

    void setParameters(SearchParameters parameters) override;
    SearchParameters getParameters() override;

private:
    ISearchLineEdit *_searchEdit;
    SearchParameters _parameters;
};

#endif // SEARCHOPTIONSMEDIATOR_H
