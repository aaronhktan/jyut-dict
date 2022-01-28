#ifndef DICTIONARYMETADATA_H
#define DICTIONARYMETADATA_H

#include <QObject>

#include <string>

// The DictionaryMetadata class contains metadata about a particular source.

class DictionaryMetadata
{
public:
    DictionaryMetadata();
    DictionaryMetadata(const std::string &name,
                       const std::string &version,
                       const std::string &description,
                       const std::string &legal,
                       const std::string &link,
                       const std::string &other);

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
