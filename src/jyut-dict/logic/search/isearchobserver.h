#ifndef ISEARCHOBSERVER_H
#define ISEARCHOBSERVER_H

#include <logic/entry/entry.h>
#include <logic/search/searchparameters.h>

#include <utility>
#include <vector>

// Interface for Observer class for search
// When informed of new results, calls callback()

class ISearchObserver
{
public:
    virtual ~ISearchObserver() = default;

    virtual void detectedLanguage(SearchParameters) {}
    virtual void callback(const std::vector<Entry> &, bool) {}
    virtual void callback(const std::vector<SourceSentence> &, bool) {}
    virtual void callback(const std::vector<std::pair<std::string, long>> &,
                          bool)
    {}

    virtual void callback(bool, const Entry &) {}
};

#endif // ISEARCHOBSERVER_H
