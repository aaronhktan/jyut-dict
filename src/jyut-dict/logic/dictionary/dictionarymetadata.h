#ifndef DICTIONARYMETADATA_H
#define DICTIONARYMETADATA_H

#include <QObject>

#include <string>

// The DictionaryMetadata class contains metadata about a particular source.

class DictionaryMetadata
{
public:
    DictionaryMetadata();
    DictionaryMetadata(std::string name,
                       std::string version,
                       std::string description,
                       std::string legal,
                       std::string link,
                       std::string other);
    DictionaryMetadata(const DictionaryMetadata &metadata);
    DictionaryMetadata(const DictionaryMetadata &&metadata);

    DictionaryMetadata &operator=(const DictionaryMetadata &metadata);
    DictionaryMetadata &operator=(const DictionaryMetadata &&metadata);

    std::string getName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getLegal() const;
    std::string getLink() const;
    std::string getOther() const;

private:
    std::string _name;
    std::string _version;
    std::string _description;
    std::string _legal;
    std::string _link;
    std::string _other;
};

Q_DECLARE_METATYPE(DictionaryMetadata)

#endif // DICTIONARYMETADATA_H
