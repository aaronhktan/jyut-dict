#ifndef IUPDATECHECKER_H
#define IUPDATECHECKER_H

#include <string>

class IUpdateChecker
{
public:
    virtual void checkForNewUpdate(void) = 0;

    virtual ~IUpdateChecker() = default;
};

#endif // IUPDATECHECKER_H
