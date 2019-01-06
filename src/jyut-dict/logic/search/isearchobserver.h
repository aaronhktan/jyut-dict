#ifndef ISEARCHOBSERVER_H
#define ISEARCHOBSERVER_H

#include <logic/entry/entry.h>

#include <vector>

class ISearchObserver
{
public:
    virtual void callback(std::vector<Entry>) = 0;
};

#endif // ISEARCHOBSERVER_H
