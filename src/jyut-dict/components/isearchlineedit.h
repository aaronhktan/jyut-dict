#ifndef ISEARCHLINEEDIT_H
#define ISEARCHLINEEDIT_H

#include "logic/search/searchparameters.h"

class ISearchLineEdit {
public:
    virtual void updateParameters(SearchParameters parameters) = 0;
    virtual void search() = 0;
};

#endif // ISEARCHLINEEDIT_H
