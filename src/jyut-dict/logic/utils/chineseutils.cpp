#include "chineseutils.h"

#include "logic/utils/utils.h"

#include <iostream>
#include <sstream>
#include <unordered_set>

namespace ChineseUtils {

const static std::unordered_set<std::string> specialCharacters = {
    ".",  "。", ",",  "，", "！", "？", "%",  "－", "…",  "⋯",  ".",
    "·",  "\"", "“",  "”",  "$",  "｜", "：", "１", "２", "３", "４",
    "５", "６", "７", "８", "９", "０", " ",  "!",  "(",  ")",  "|",
    "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "0",
};

std::string applyColours(const std::string original,
                         const std::vector<uint8_t> &tones,
                         const std::vector<std::string> &jyutpingToneColours,
                         const std::vector<std::string> &pinyinToneColours,
                         const EntryColourPhoneticType type)
{
    std::string coloured_string;
    auto data = QString::fromStdString(original)
                    .normalized(QString::NormalizationForm_C)
                    .toStdU32String();
    size_t pos = 0;
    for (const auto codepoint : data) {
        std::string originalStr
            = QString::fromStdU32String(std::u32string{codepoint}).toStdString();

        // Skip same character string; they have no colour
        // However, increment to the next tone position
        // since they represent characters that are the same between simplified
        // and traditional and therefore have tones
        if (codepoint
            == QString::fromStdString(Utils::SAME_CHARACTER_STRING)
                   .toStdU32String()[0]) {
            coloured_string += originalStr;
            pos++;
            continue;
        }

        // Skip any special characters
        // but do not increment to next tone position,
        // since special characters do not have any tones associated with them
        auto isSpecialCharacter = specialCharacters.find(originalStr)
                                  != specialCharacters.end();
        bool isIdeograph
            = (codepoint >= 0x4E00
               && codepoint <= 0x9FFF) // CJK Unified Ideographs
              || (codepoint >= 0x3400
                  && codepoint <= 0x4DBF) // CJK Unified Ideographs Extension A
              || (codepoint >= 0x20000
                  && codepoint <= 0x2A6DF) // CJK Unified Ideographs Extension B
              || (codepoint >= 0x2A700
                  && codepoint <= 0x2B73F) // CJK Unified Ideographs Extension C
              || (codepoint >= 0x2B740
                  && codepoint <= 0x2B81F) // CJK Unified Ideographs Extension D
              || (codepoint >= 0x2B820
                  && codepoint <= 0x2CEAF) // CJK Unified Ideographs Extension E
              || (codepoint >= 0x2CEB0
                  && codepoint <= 0x2EBEF); // CJK Unified Ideographs Extension F
        if (isSpecialCharacter || !isIdeograph) {
            coloured_string += originalStr;
            continue;
        }

        // Get the tone...
        int tone = 0;
        try {
            tone = tones.at(pos);
        } catch ([[maybe_unused]] const std::out_of_range &e) {
            coloured_string += originalStr;
            continue;
        }

        // ... and apply tone colour formatting to the string
        switch (type) {
        case EntryColourPhoneticType::CANTONESE: {
            coloured_string += "<font color=\""
                               + jyutpingToneColours.at(
                                   static_cast<size_t>(tone))
                               + "\">";
            break;
        }
        case EntryColourPhoneticType::MANDARIN: {
            coloured_string += "<font color=\""
                               + pinyinToneColours.at(static_cast<size_t>(tone))
                               + "\">";
            break;
        }
        case EntryColourPhoneticType::NONE: {
            // Should never get here
            coloured_string += "<font>";
            break;
        }
        }
        coloured_string += originalStr;
        coloured_string += "</font>";

        pos++;
    }

    return coloured_string;
}

std::string compareStrings(const std::string &original,
                           const std::string &comparison)
{
    std::string result;
    std::u32string convertedOriginal = QString::fromStdString(original)
                                           .normalized(
                                               QString::NormalizationForm_C)
                                           .toStdU32String();
    std::u32string convertedComparison = QString::fromStdString(comparison)
                                             .normalized(
                                                 QString::NormalizationForm_C)
                                             .toStdU32String();

    if (convertedOriginal.size() != convertedComparison.size()) {
        return result;
    }

    for (size_t i = 0; i < convertedOriginal.size(); i++) {
        std::string currentCharacter
            = QString::fromStdU32String(std::u32string{convertedComparison[i]})
                  .toStdString();

        auto isSpecialCharacter = specialCharacters.find(currentCharacter)
                                  != specialCharacters.end();
        if (isSpecialCharacter
            || convertedOriginal[i] != convertedComparison[i]) {
            result += currentCharacter;
            continue;
        }

        result += Utils::SAME_CHARACTER_STRING;
    }

    return result;
}

std::string constructRomanisationQuery(const std::vector<std::string> &words,
                                       const char *delimiter)
{
    if (words.empty()) {
        return "";
    }

    std::string trimmed_word;
    std::string space;
    bool added_delimiter = false;

    std::ostringstream string;
    for (size_t i = 0; i < words.size() - 1; i++) {
        Utils::trim(words[i], trimmed_word);
        if (std::isdigit(words[i].back())) {
            string << space << words[i];
            space = " ";
            added_delimiter = false;
        } else if (trimmed_word == "*" || trimmed_word == "?") {
            if ((words[i] == "*" || words[i] == "?" || words[i] == "* "
                 || words[i] == "? ")
                && (added_delimiter)) {
                // Replace delimiter with GLOB wildcard if GLOB wildcard
                // was attached to end of previous word (i.e. wildcard did not
                // start with a space)
                string.seekp(-1, std::ios_base::cur);
            }
            string << words[i];
            // GLOB characters handle their own spacing, so don't insert any
            // extra spaces.
            space = "";
            added_delimiter = false;
        } else {
            string << space << words[i] << delimiter;
            space = " ";
            added_delimiter = true;
        }
    }

    Utils::trim(words.back(), trimmed_word);
    if (std::isdigit(words.back().back())) {
        string << space << words.back();
    } else if (trimmed_word == "*" || trimmed_word == "?") {
        if ((words.back() == "*" || words.back() == "?" || words.back() == "* "
             || words.back() == "? ")
            && (added_delimiter)) {
            string.seekp(-1, std::ios_base::cur);
        }
        string << words.back();
    } else {
        string << space << words.back() << delimiter;
    }
    return string.str();
}

} // namespace ChineseUtils
