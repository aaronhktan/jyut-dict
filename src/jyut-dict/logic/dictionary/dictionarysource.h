#ifndef DICTIONARYSOURCE_H
#define DICTIONARYSOURCE_H

#include <QObject>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

// The name_to_short_name unordered_map contains mappings for long names to
// nicknames of sources.

// It should be populated upon startup by reading sources in the database,
// and modified whenever sources are added or removed.

namespace DictionarySourceUtils {

static std::unordered_map<std::string, std::string> name_to_short_name{};

std::string getSourceShortString(std::string source);
bool addSource(std::string sourcename, std::string shortsourcename);
bool removeSource(std::string sourcename);

}

#endif // DICTIONARYSOURCE_H
