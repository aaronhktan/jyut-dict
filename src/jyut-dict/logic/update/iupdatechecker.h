#ifndef IUPDATECHECKER_H
#define IUPDATECHECKER_H

#include <QObject>

#include <optional>
#include <variant>

// Interface for release checking

class IUpdateChecker
{
public:
    struct AppUpdateAvailability
    {
        bool updateAvailable;
        std::optional<std::string> versionNumber;
        std::optional<std::string> url;
        std::optional<std::string> description;
    };

    struct SourceUpdateAvailability
    {
        bool updateAvailable;
        std::optional<std::string> sourceName;
        std::optional<std::string> versionNumber;
        std::optional<std::string> url;
        std::optional<std::string> description;
    };

    using UpdateVariant = std::variant<AppUpdateAvailability,
                                       std::vector<SourceUpdateAvailability>>;

    virtual void checkForNewUpdate(void) = 0;

    virtual ~IUpdateChecker() = default;

signals:
    virtual void foundUpdate(const UpdateVariant &v) = 0;
};

#endif // IUPDATECHECKER_H
