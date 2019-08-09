#include "utils.h"

#include <sstream>
#include <string>

namespace Utils {
    void split(const std::string &string,
               const char delimiter,
               std::vector<std::string> &result)
    {
        result.clear();
        std::stringstream ss(string);
        std::string word;

        while (std::getline(ss, word, delimiter)) {
            result.push_back(word);
        }
    }

    void split(const std::string &string,
               const std::string delimiter,
               std::vector<std::string> &result)
    {
        result.clear();

        size_t previous = 0;
        size_t current = 0;
        while ((current = string.find(delimiter, previous))
               != std::string::npos) {
            result.push_back(string.substr(previous, current - previous));
            previous = current + delimiter.length();
        }
        result.push_back(string.substr(previous));
    }
}
