#ifndef ISEARCHOBSERVER_H
#define ISEARCHOBSERVER_H

#include <logic/entry/entry.h>

#include <vector>

// Interface for Observer class for search
// When informed of new results, calls callback()

class ISearchObserver
{
public:
    virtual ~ISearchObserver() = default;

    virtual void callback(const std::vector<Entry>, bool){}
    virtual void callback(const std::vector<SourceSentence>, bool){}
    virtual void callback(bool, Entry){}
};

#endif // ISEARCHOBSERVER_H
