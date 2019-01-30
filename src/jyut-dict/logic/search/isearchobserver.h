#ifndef ISEARCHOBSERVER_H
#define ISEARCHOBSERVER_H

#include <logic/entry/entry.h>

#include <vector>

// Interface for Observer class for search
// When informed of new results, calls callback()

class ISearchObserver
{
public:
    virtual void callback(std::vector<Entry>) = 0;
};

#endif // ISEARCHOBSERVER_H
