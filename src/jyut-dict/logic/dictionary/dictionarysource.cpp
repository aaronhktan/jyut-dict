#include "logic/entry/dictionarysource.h"

namespace DictionarySourceUtils {
std::string getSourceLongString(DictionarySource source)
{
    auto result = dictionarysource_to_string.find(source);

    if (result == dictionarysource_to_string.end()) {
        return "";
    }

    return result->second;
}

std::string getSourceShortString(DictionarySource source)
{
    auto result = dictionarysource_to_short_string.find(source);

    if (result == dictionarysource_to_short_string.end()) {
        return "";
    }

    return result->second;
}
}
