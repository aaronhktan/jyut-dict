#ifndef ISEARCHLINEEDIT_H
#define ISEARCHLINEEDIT_H

#include "logic/search/searchparameters.h"

// Interface for search bars

// Parameters determine what to search (e.g. English, Simplified Chinese, etc.)
// Search causes a search to occur.

class ISearchLineEdit {
public:
    virtual ~ISearchLineEdit() = default;

    virtual void updateParameters(SearchParameters parameters) = 0;
    virtual void search(std::optional<SearchParameters>
                        = std::optional<SearchParameters>{})
        = 0;
};

#endif // ISEARCHLINEEDIT_H
