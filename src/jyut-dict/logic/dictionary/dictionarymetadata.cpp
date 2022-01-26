#include "dictionarymetadata.h"

DictionaryMetadata::DictionaryMetadata()
{

}

DictionaryMetadata::DictionaryMetadata(const std::string &name,
                                       const std::string &version,
                                       const std::string &description,
                                       const std::string &legal,
                                       const std::string &link,
                                       const std::string &other)
    : _name{name},
    _version{version},
    _description{description},
    _legal{legal},
    _link{link},
    _other{other}
{

}

std::string DictionaryMetadata::getName() const
{
    return _name;
}

std::string DictionaryMetadata::getVersion() const
{
    return _version;
}

std::string DictionaryMetadata::getDescription() const
{
    return _description;
}

std::string DictionaryMetadata::getLegal() const
{
    return _legal;
}

std::string DictionaryMetadata::getLink() const
{
    return _link;
}

std::string DictionaryMetadata::getOther() const
{
    return _other;
}
