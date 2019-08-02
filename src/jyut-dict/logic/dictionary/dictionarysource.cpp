#include "logic/dictionary/dictionarysource.h"

namespace DictionarySourceUtils {

std::string getSourceShortString(std::string source)
{
    auto result = name_to_short_name.find(source);

    if (result == name_to_short_name.end()) {
        return "";
    }

    return result->second;
}

bool addSource(std::string sourcename, std::string shortsourcename)
{
    return name_to_short_name.insert({sourcename, shortsourcename}).second;
}

bool removeSource(std::string sourcename)
{
    auto index = std::find_if(name_to_short_name.begin(),
                           name_to_short_name.end(),
                           [sourcename](
                               std::pair<std::string, std::string> source) {
                               return source.first == sourcename;
                           });
    if (index == name_to_short_name.end()) {
        return false;
    }

    name_to_short_name.erase(index);

    return true;
}
}
