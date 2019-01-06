#ifndef ISEARCHOBSERVABLE_H
#define ISEARCHOBSERVABLE_H

#include "logic/search/isearchobserver.h"

class ISearchObservable
{
public:
    virtual void registerObserver(ISearchObserver* observer) = 0;
    virtual void deregisterObserver(ISearchObserver* observer) = 0;
    virtual void notifyObservers() = 0;
};

#endif // ISEARCHOBSERVABLE_H
