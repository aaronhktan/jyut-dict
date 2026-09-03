#ifndef IUPDATECHECKER_H
#define IUPDATECHECKER_H

#include <QObject>

#include <optional>
#include <variant>

// Interface for release checking

class IUpdateChecker
{
public:
    struct AppManifestMetadata
    {
        bool updateAvailable;
        std::optional<std::string> versionNumber;
        std::optional<std::string> url;
        std::optional<std::string> description;
    };

    struct SourceManifestMetadata
    {
        std::string sourceName;
        std::string versionNumber;
        std::string url;
        std::string checksum;
        std::optional<std::string> description;
    };

    using UpdateVariant
        = std::variant<AppManifestMetadata, std::vector<SourceManifestMetadata>>;

    virtual void checkForNewUpdate() = 0;

    virtual ~IUpdateChecker() = default;

signals:
    virtual void foundUpdate(const UpdateVariant &v) = 0;
};

Q_DECLARE_METATYPE(const IUpdateChecker::SourceManifestMetadata *)

#endif // IUPDATECHECKER_H
