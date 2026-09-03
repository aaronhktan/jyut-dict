#ifndef SOURCEMETADATA_H
#define SOURCEMETADATA_H

#include <QObject>

#include <string>

// The SourceMetadata class contains metadata about a particular source.

class SourceMetadata
{
public:
    SourceMetadata();
    SourceMetadata(const std::string &name,
                   const std::string &version,
                   const std::string &description,
                   const std::string &legal,
                   const std::string &link,
                   const std::string &updateURL,
                   const std::string &other);

    const std::string &getName() const;
    const std::string &getVersion() const;
    const std::string &getDescription() const;
    const std::string &getLegal() const;
    const std::string &getLink() const;
    const std::string &getUpdateURL() const;
    const std::string &getOther() const;

private:
    std::string _name;
    std::string _version;
    std::string _description;
    std::string _legal;
    std::string _link;
    std::string _updateURL;
    std::string _other;
};

Q_DECLARE_METATYPE(SourceMetadata)

#endif // SOURCEMETADATA_H
