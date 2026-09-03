#ifndef ISEARCHOBSERVABLE_H
#define ISEARCHOBSERVABLE_H

#include "logic/entry/entry.h"
#include "logic/search/isearchobserver.h"

#include <utility>
#include <vector>

// Interface for Observable class for search
// Allows registering/deregistering observers
// And notifies them when new search results are available

class ISearchObservable
{
public:
    virtual ~ISearchObservable() = default;

    virtual void registerObserver(ISearchObserver *) {}
    virtual void deregisterObserver(ISearchObserver *) {}
    virtual void notifyObservers(SearchParameters) {}
    virtual void notifyObservers(const std::vector<Entry> &, bool) {}
    virtual void notifyObservers(const std::vector<SourceSentence> &, bool) {}
    virtual void notifyObservers(
        const std::vector<std::pair<std::string, int>> &, bool)
    {}
    virtual void notifyObservers(bool, const Entry &) {}
};

#endif // ISEARCHOBSERVABLE_H
