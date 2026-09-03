#include "sourcemetadata.h"

SourceMetadata::SourceMetadata() {}

SourceMetadata::SourceMetadata(const std::string &name,
                               const std::string &version,
                               const std::string &description,
                               const std::string &legal,
                               const std::string &link,
                               const std::string &updateURL,
                               const std::string &other)
    : _name{name}
    , _version{version}
    , _description{description}
    , _legal{legal}
    , _link{link}
    , _updateURL{updateURL}
    , _other{other}
{

}

const std::string &SourceMetadata::getName() const
{
    return _name;
}

const std::string &SourceMetadata::getVersion() const
{
    return _version;
}

const std::string &SourceMetadata::getDescription() const
{
    return _description;
}

const std::string &SourceMetadata::getLegal() const
{
    return _legal;
}

const std::string &SourceMetadata::getLink() const
{
    return _link;
}

const std::string &SourceMetadata::getUpdateURL() const
{
    return _updateURL;
}

const std::string &SourceMetadata::getOther() const
{
    return _other;
}
