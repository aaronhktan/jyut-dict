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
            if (word.length() > 0 && word[0] != delimiter) {
                result.push_back(word);
            }
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
            std::string substr = string.substr(previous, current - previous);
            if (substr != delimiter) {
                result.push_back(substr);
            }
            previous = current + delimiter.length();
        }
        result.push_back(string.substr(previous));
    }
}
