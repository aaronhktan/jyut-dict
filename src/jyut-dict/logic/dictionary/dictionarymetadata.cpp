#include "dictionarymetadata.h"

DictionaryMetadata::DictionaryMetadata()
{

}

DictionaryMetadata::DictionaryMetadata(const std::string &name,
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

const std::string &DictionaryMetadata::getName() const
{
    return _name;
}

const std::string &DictionaryMetadata::getVersion() const
{
    return _version;
}

const std::string &DictionaryMetadata::getDescription() const
{
    return _description;
}

const std::string &DictionaryMetadata::getLegal() const
{
    return _legal;
}

const std::string &DictionaryMetadata::getLink() const
{
    return _link;
}

const std::string &DictionaryMetadata::getUpdateURL() const
{
    return _updateURL;
}

const std::string &DictionaryMetadata::getOther() const
{
    return _other;
}
