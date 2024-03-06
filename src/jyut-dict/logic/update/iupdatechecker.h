#ifndef IUPDATECHECKER_H
#define IUPDATECHECKER_H

#include <QObject>

// Interface for release checking

class IUpdateChecker
{
public:
    virtual void checkForNewUpdate(void) = 0;

    virtual ~IUpdateChecker() = default;

signals:
    virtual void foundUpdate(bool updateAvailable,
                             std::string versionNumber,
                             std::string url,
                             std::string description)
        = 0;
};

#endif // IUPDATECHECKER_H
