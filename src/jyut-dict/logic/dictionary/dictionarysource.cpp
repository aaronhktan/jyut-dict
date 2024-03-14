#include "logic/dictionary/dictionarysource.h"

#include <algorithm>
#include <shared_mutex>

namespace DictionarySourceUtils {

std::unordered_map<std::string, std::string> name_to_short_name;
std::shared_mutex map_mutex;

std::string getSourceShortString(const std::string &source)
{
    std::shared_lock lock{map_mutex};
    auto result = name_to_short_name.find(source);

    if (result == name_to_short_name.end()) {
        return "";
    }

    return result->second;
}

bool addSource(const std::string &sourcename, const std::string &shortsourcename)
{
    std::lock_guard lock{map_mutex};
    return name_to_short_name.insert({sourcename, shortsourcename}).second;
}

bool removeSource(const std::string &sourcename)
{
    std::lock_guard lock{map_mutex};
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
