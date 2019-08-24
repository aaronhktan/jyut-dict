#ifndef IUPDATECHECKER_H
#define IUPDATECHECKER_H

#include <string>

// Interface for release checking

class IUpdateChecker
{
public:
    virtual void checkForNewUpdate(void) = 0;

    virtual ~IUpdateChecker() = default;
};

#endif // IUPDATECHECKER_H
