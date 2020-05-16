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

    virtual void registerObserver(ISearchObserver *observer)
    {
        (void) (observer);
    }
    virtual void deregisterObserver(ISearchObserver *observer)
    {
        (void) (observer);
    }
    virtual void notifyObservers(const std::vector<Entry> &results,
                                 bool emptyQuery)
    {
        (void) (results);
        (void) (emptyQuery);
    }
    virtual void notifyObservers(const std::vector<SourceSentence> &results,
                                 bool emptyQuery)
    {
        (void) (results);
        (void) (emptyQuery);
    }
    virtual void notifyObservers(
        const std::vector<std::pair<std::string, int>> &results, bool emptyQuery)
    {
        (void) (results);
        (void) (emptyQuery);
    }

    virtual void notifyObservers(bool entryExists, Entry entry)
    {
        (void) (entryExists);
        (void) (entry);
    }
};

#endif // ISEARCHOBSERVABLE_H
