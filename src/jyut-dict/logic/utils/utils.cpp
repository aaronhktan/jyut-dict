#include "utils.h"

#include <sstream>

namespace Utils {
    void split(const std::string &string,
               const char delimiter,
               std::vector<std::string> &result) {
        result.clear();
        std::stringstream ss(string);
        std::string word;

        while (std::getline(ss, word, delimiter)) {
            result.push_back(word);
        }
    }
}
