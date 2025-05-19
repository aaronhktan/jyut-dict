#include "searchoptionsmediator.h"

void SearchOptionsMediator::registerLineEdit(ISearchLineEdit *lineEdit)
{
    _lineEdit = lineEdit;
}

void SearchOptionsMediator::registerOptionSelector(
    ISearchOptionsSelector *selector)
{
    _selector = selector;
}

void SearchOptionsMediator::setParameters(SearchParameters parameters)
{
    if (_parameters == parameters) {
        _lineEdit->search();
        return;
    }

    _parameters = parameters;

    if (_lineEdit) {
        _lineEdit->updateParameters(parameters);
        _lineEdit->search();
    }
    if (_selector) {
        _selector->setOption(parameters);
    }
}

SearchParameters SearchOptionsMediator::getParameters()
{
    return _parameters;
}
