#include "searchoptionsmediator.h"

#include <components/mainwindow/searchoptionsradiogroupbox.h>

SearchOptionsMediator::SearchOptionsMediator()
{

}

SearchOptionsMediator::~SearchOptionsMediator()
{

}

void SearchOptionsMediator::registerLineEdit(ISearchLineEdit *searchEdit)
{
    _searchEdit = searchEdit;
}

void SearchOptionsMediator::setParameters(SearchParameters parameters)
{
    _parameters = parameters;
    _searchEdit->updateParameters(parameters);
    _searchEdit->search();
}

SearchParameters SearchOptionsMediator::getParameters()
{
    return _parameters;
}
