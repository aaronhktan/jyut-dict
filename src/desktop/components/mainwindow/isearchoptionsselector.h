#ifndef ISEARCHOPTIONSSELECTOR_H
#define ISEARCHOPTIONSSELECTOR_H

#include "logic/search/searchparameters.h"

// Interface to select/display current search settings

class ISearchOptionsSelector
{
public:
    virtual ~ISearchOptionsSelector() = default;

    virtual void setOption(SearchParameters parameters) = 0;
    virtual void optionSelected(SearchParameters parameters) = 0;
};

#endif // ISEARCHOPTIONSSELECTOR_H
