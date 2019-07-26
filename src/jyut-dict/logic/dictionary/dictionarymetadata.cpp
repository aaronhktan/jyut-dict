#include "dictionarymetadata.h"

DictionaryMetadata::DictionaryMetadata()
{

}

DictionaryMetadata::DictionaryMetadata(std::string name,
                                       std::string version,
                                       std::string description,
                                       std::string legal,
                                       std::string link,
                                       std::string other)
    : _name{name},
    _version{version},
    _description{description},
    _legal{legal},
    _link{link},
    _other{other}
{

}

DictionaryMetadata::DictionaryMetadata(const DictionaryMetadata &metadata)
    : _name{metadata.getName()}
    , _version{metadata.getVersion()}
    , _description{metadata.getDescription()}
    , _legal{metadata.getLegal()}
    , _link{metadata.getLink()}
    , _other{metadata.getOther()}
{

}

DictionaryMetadata::DictionaryMetadata(const DictionaryMetadata &&metadata)
    : _name{metadata.getName()}
    , _version{metadata.getVersion()}
    , _description{metadata.getDescription()}
    , _legal{metadata.getLegal()}
    , _link{metadata.getLink()}
    , _other{metadata.getOther()}
{

}

DictionaryMetadata &DictionaryMetadata::operator=(
    const DictionaryMetadata &metadata)
{
    if (&metadata == this) {
        return *this;
    }

    _name = metadata.getName();
    _version = metadata.getVersion();
    _description = metadata.getDescription();
    _legal = metadata.getLegal();
    _link = metadata.getLink();
    _other = metadata.getOther();

    return *this;
}

DictionaryMetadata &DictionaryMetadata::operator=(
    const DictionaryMetadata &&metadata)
{
    if (&metadata == this) {
        return *this;
    }

    _name = metadata.getName();
    _version = metadata.getVersion();
    _description = metadata.getDescription();
    _legal = metadata.getLegal();
    _link = metadata.getLink();
    _other = metadata.getOther();

    return *this;
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
