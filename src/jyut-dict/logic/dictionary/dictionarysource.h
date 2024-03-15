#ifndef DICTIONARYSOURCE_H
#define DICTIONARYSOURCE_H

#include <QObject>

#include <string>
#include <unordered_map>

// The name_to_short_name unordered_map contains mappings for long names to
// nicknames of sources.

// It should be populated upon startup by reading sources in the database,
// and modified whenever sources are added or removed.

namespace DictionarySourceUtils {

extern std::unordered_map<std::string, std::string> name_to_short_name;

std::string getSourceShortString(const std::string &source);
bool addSource(const std::string &sourcename,
               const std::string &shortsourcename);
bool removeSource(const std::string &sourcename);

}

#endif // DICTIONARYSOURCE_H
