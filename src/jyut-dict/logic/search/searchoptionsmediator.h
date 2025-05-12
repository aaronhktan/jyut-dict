#ifndef SEARCHOPTIONSMEDIATOR_H
#define SEARCHOPTIONSMEDIATOR_H

#include "logic/search/isearchoptionsmediator.h"
#include "logic/search/searchparameters.h"

// Concrete implementation of SearchOptionsMediator interface
// See ISearchOptionsMediator.h for more details

class SearchOptionsMediator : public ISearchOptionsMediator
{
public:
    SearchOptionsMediator() = default;

    void registerLineEdit(ISearchLineEdit *_searchEdit) override;
    void registerOptionSelector(ISearchOptionsSelector *_selector) override;

    void setParameters(SearchParameters parameters) override;
    SearchParameters getParameters() override;

private:
    ISearchLineEdit *_lineEdit = nullptr;
    ISearchOptionsSelector *_selector = nullptr;
    SearchParameters _parameters;
};

#endif // SEARCHOPTIONSMEDIATOR_H
