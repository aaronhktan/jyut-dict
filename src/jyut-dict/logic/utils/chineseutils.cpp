#include "chineseutils.h"

#include "logic/utils/utils.h"
#include "logic/settings/settings.h"

#include <codecvt>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace ChineseUtils {

#ifdef _MSC_VER
static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
#else
static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
#endif

const static std::unordered_set<std::string> specialCharacters = {
    "，",
    "%",
    "－",
    "…",
    "·",
};

const static std::unordered_map<std::string, std::vector<std::string>> replacementMap
    = {
        {"a", {"ā", "á", "ǎ", "à", "a"}},
        {"e", {"ē", "é", "ě", "è", "e"}},
        {"i", {"ī", "í", "ǐ", "ì", "i"}},
        {"o", {"ō", "ó", "ǒ", "ò", "o"}},
        {"u", {"ū", "ú", "ǔ", "ù", "u"}},
        {"ü", {"ǖ", "ǘ", "ǚ", "ǜ", "ü"}},
};

std::string applyColours(
    const std::string original,
    const std::vector<int> &tones,
    const std::vector<std::string> &jyutpingToneColours,
    const std::vector<std::string> &pinyinToneColours,
    const EntryColourPhoneticType type)
{
    std::string coloured_string;
#ifdef _MSC_VER
    std::wstring converted_original = converter.from_bytes(original);
#else
    std::u32string converted_original = converter.from_bytes(original);
#endif
    size_t pos = 0;
    for (auto character : converted_original) {
        std::string originalCharacter = converter.to_bytes(character);

        // Skip same character string; they have no colour
        // However, increment to the next tone position
        // since they represent characters that are the same between simplified
        // and traditional and therefore have tones
        if (character == converter.from_bytes(Utils::SAME_CHARACTER_STRING)[0]) {
            coloured_string += originalCharacter;
            pos++;
            continue;
        }

        // Skip any special characters
        // but do not increment to next tone position,
        // since special characters do not have any tones associated with them
        auto isSpecialCharacter = specialCharacters.find(originalCharacter) != specialCharacters.end();
        auto isAlphabetical = std::find_if(originalCharacter.begin(),
                                           originalCharacter.end(),
                                           isalpha)
                              != originalCharacter.end();
        if (isSpecialCharacter || isAlphabetical) {
            coloured_string += converter.to_bytes(character);
            continue;
        }

        // Get the tone...
        int tone = 0;
        try {
            tone = tones.at(pos);
        } catch (const std::out_of_range &/*e*/) {
            coloured_string += originalCharacter;
            //            std::cerr << "Couldn't get tone for character"
            //                      << converter.to_bytes(character)
            //                      << " in character" << original
            //                      << " Error:" << e.what() << std::endl;
            continue;
        }

        // ... and apply tone colour formatting to the string
        switch (type) {
        case EntryColourPhoneticType::JYUTPING: {
            coloured_string += "<font color=\""
                               + jyutpingToneColours.at(
                                   static_cast<size_t>(tone))
                               + "\">";
            break;
        }
        case EntryColourPhoneticType::PINYIN: {
            coloured_string += "<font color=\""
                               + pinyinToneColours.at(
                                   static_cast<size_t>(tone))
                               + "\">";
            break;
        }
        case EntryColourPhoneticType::NONE: {
            // Should never get here
            coloured_string += "<font>";
            break;
        }
        }
        coloured_string += originalCharacter;
        coloured_string += "</font>";

        pos++;
    }

    return coloured_string;
}

std::string compareStrings(const std::string original,
                           const std::string comparison)
{
    std::string result;

#ifdef _MSC_VER
    std::wstring convertedOriginal = converter.from_bytes(original);
    std::wstring convertedComparison = converter.from_bytes(comparison);
#else
    std::u32string convertedOriginal = converter.from_bytes(original);
    std::u32string convertedComparison = converter.from_bytes(comparison);
#endif
    if (convertedOriginal.size() != convertedComparison.size()) {
        return result;
    }

    for (size_t i = 0; i < convertedOriginal.size(); i++) {
        std::string currentCharacter = converter.to_bytes(convertedComparison[i]);

        auto isSpecialCharacter = specialCharacters.find(currentCharacter) != specialCharacters.end();
        if (isSpecialCharacter
            || convertedOriginal[i] != convertedComparison[i]) {
            result += currentCharacter;
            continue;
        }

        if (std::find_if(currentCharacter.begin(), currentCharacter.end(), isalpha) != currentCharacter.end()) {
            result += currentCharacter;
            continue;
        }

        result += Utils::SAME_CHARACTER_STRING;
    }

    return result;
}

std::string createPrettyPinyin(const std::string pinyin)
{
    std::string result;

    // Create a vector of each space-separated value in pinyin
    std::vector<std::string> syllables;
    Utils::split(pinyin, ' ', syllables);
    if (syllables.empty()) {
        return pinyin;
    }

    for (auto syllable : syllables) {
        // Skip the punctuation, they have no tone
        if (specialCharacters.find(syllable) != specialCharacters.end()) {
            result += syllable + " ";
            continue;
        }

        // Extract the tone from the syllable
        size_t tone_location = syllable.find_first_of("012345");
        if (tone_location == std::string::npos) {
            result += syllable + " ";
            continue;
        }
        int tone = syllable.at(tone_location) - '0';
        // Filter out invalid tone: set to neutral if tone doesn't work
        if (tone <= 0 || tone > 5) {
            tone = 5;
        }

        // Convert u: to ü
        size_t location = syllable.find("u:");
        if (location != std::string::npos) {
            syllable.erase(location, 2);
            syllable.insert(location, "ü");
        }

        // The rule for pinyin diacritic location is:
        // - If a, e, or o exists in the syllable, it takes the diacritic.
        // - Otherwise, the last u, ü, or i takes it.
        location = syllable.find_first_of("aeo");
        size_t character_size = 1;

        if (location == std::string::npos) {
            location = syllable.find("ü");
            // ü is stored as two bytes, so change the number of characters that
            // we need to delete if we find a ü
            character_size = location == std::string::npos ? 1 : 2;
        }

        if (location == std::string::npos) {
            location = syllable.find_last_of("ui");
        }

        if (location == std::string::npos) {
            result += syllable + " ";
            continue;
        }

        // replacementMap maps a character to its replacements with diacritics.
        auto search = replacementMap.find(syllable.substr(location, character_size));
        if (search != replacementMap.end()) {
            std::string replacement = search->second.at(
                static_cast<size_t>(tone) - 1);
            syllable.erase(location, character_size);
            syllable.insert(location, replacement);
        } else {
            result += syllable + " ";
            continue;
        }

        // Remove the tone from the pinyin
        tone_location = syllable.find_first_of("012345");
        syllable.erase(tone_location, 1);
        result += syllable + " ";
    }

    // Remove trailing space
    result.erase(result.end() - 1);

    return result;
}

std::string constructRomanisationQuery(const std::vector<std::string> &words,
                                       const char *delimiter,
                                       const bool surroundWithQuotes)
{
    const char *quotes = surroundWithQuotes ? "\"": "";
    std::ostringstream string;
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (std::isdigit(words[i].back())) {
            string << quotes << words[i] << quotes << " ";
        } else {
            string << quotes << words[i] << quotes << delimiter << " ";
        }
    }

    if (std::isdigit(words.back().back())) {
        string << quotes << words.back() << quotes;
    } else {
        string << quotes << words.back() << quotes << delimiter;
    }
    return string.str();
}

std::vector<std::string> segmentPinyin(const QString string)
{
    std::vector<std::string> words;

    std::unordered_set<std::string> initials = {"b", "p", "m",  "f",  "d",  "t",
                                                "n", "l", "g",  "k",  "h",  "j",
                                                "q", "x", "zh", "ch", "sh", "r",
                                                "z", "c", "s"};
    std::unordered_set<std::string> finals
        = {"a",   "e",   "ai",   "ei",   "ao",   "ou", "an", "ang", "en",
           "ang", "eng", "ong",  "er",   "i",    "ia", "ie", "iao", "iu",
           "ian", "in",  "iang", "ing",  "iong", "u",  "ua", "uo",  "uai",
           "ui",  "uan", "un",   "uang", "u",    "u:", "ue", "u:e", "o"};

    // Keep track of indices for current segmented word; [start_index, end_index)
    // Greedily try to expand end_index by checking for valid sequences
    // of characters
    int start_index = 0;
    int end_index = 0;
    bool word_started = false;

    while (end_index < string.length()) {
        bool next_iteration = false;
        // Ignore separation characters; these are special.
        QString stringToExamine = string.mid(end_index, 1).toLower();
        if (stringToExamine == " " || stringToExamine == "'") {
            if (word_started) { // Add any incomplete word to the vector
                QString previous_initial = string.mid(start_index,
                                                      end_index - start_index);
                words.push_back(previous_initial.toStdString());
                start_index = end_index;
                word_started = false;
            }
            start_index++;
            end_index++;
            continue;
        }

        // First, check for initials
        // If initial is valid, then extend the end_index for length of initial
        // cluster of consonants.
        for (int length = 2; length > 0; length--) {
            stringToExamine = string.mid(end_index, length).toLower();
            auto searchResult = initials.find(stringToExamine.toStdString());
            if (searchResult != initials.end()
                && stringToExamine.length() == length) {
                end_index += length;
                next_iteration = true;
                word_started = true;
                break;
            }
        }

        if (next_iteration) {
            continue;
        }

        // Then, check for finals
        // If final is valid, then extend end_index for length of final.
        // Check for number at end of word as well (this represents tone number).
        //
        // Then add the substring from [start_index, end_index) to vector
        // and reset start_index, so we can start searching after the end_index.
        for (int length = 4; length > 0; length--) {
            stringToExamine = string.mid(end_index, length).toLower();
            auto searchResult = finals.find(stringToExamine.toStdString());
            if (searchResult != finals.end()
                && stringToExamine.length() == length) {
                end_index += length;
                if (end_index < string.length()) {
                    if (string.at(end_index).isDigit()) {
                        end_index++;
                    }
                }
                QString word = string.mid(start_index, end_index - start_index);
                words.push_back(word.toStdString());
                start_index = end_index;
                next_iteration = true;
                word_started = false;
                break;
            }
        }

        if (next_iteration) {
            continue;
        }

        end_index++;
    }

    // Then add whatever's left in the search term, minus whitespace.
    QString lastWord = string.mid(start_index, end_index - start_index)
                           .simplified();
    if (!lastWord.isEmpty() && lastWord != "'") {
        words.push_back(lastWord.toStdString());
    }

    return words;
}

std::vector<std::string> segmentJyutping(const QString string)
{
    std::vector<std::string> words;

    std::unordered_set<std::string> initials = {"b",  "p", "m",  "f",  "d",
                                                "t",  "n", "l",  "g",  "k",
                                                "ng", "h", "gw", "kw", "w",
                                                "z",  "c", "s",  "j",  "m"};
    std::unordered_set<std::string> finals
        = {"aa",   "aai", "aau", "aam", "aan", "aang", "aap", "aat",
           "aak",  "ai",  "au",  "am",  "an",  "ang",  "ap",  "at",
           "ak",   "e",   "ei",  "eu",  "em",  "eng",  "ep",  "ek",
           "i",    "iu",  "im",  "in",  "ing", "ip",   "it",  "ik",
           "o",    "oi",  "ou",  "on",  "ong", "ot",   "ok",  "u",
           "ui",   "un",  "ung", "ut",  "uk",  "oe",   "eoi", "eon",
           "oeng", "eot", "oek", "yu",  "yun", "yut",  "m",   "ng"};

    // Keep track of indices for current segmented word; [start_index, end_index)
    // Greedily try to expand end_index by checking for valid sequences
    // of characters
    int start_index = 0;
    int end_index = 0;
    bool initial_found = false;

    while (end_index < string.length()) {
        bool next_iteration = false;

        // Ignore separation characters; these are special.
        QString stringToExamine = string.mid(end_index, 1).toLower();
        if (stringToExamine == " " || stringToExamine == "'") {
            if (initial_found) { // Add any incomplete word to the vector
                QString previous_initial = string.mid(start_index,
                                                      end_index - start_index);
                words.push_back(previous_initial.toStdString());
                start_index = end_index;
                initial_found = false;
            }
            start_index++;
            end_index++;
            continue;
        }

        // Check for digit
        // Digits are only valid after a final (which should be handled in the
        // final-checking code
        // OR after an initial (that is also a final), like m or ng.
        // This block checks for the latter case.
        if (stringToExamine.at(0).isDigit()) {
            if (initial_found) {
                QString previous_initial = string.mid(start_index,
                                                      end_index - start_index);
                auto searchResult = finals.find(previous_initial.toStdString());
                if (searchResult != finals.end()) {
                    // Confirmed, last initial found was also a final
                    end_index++;
                    previous_initial = string.mid(start_index,
                                                  end_index - start_index);
                    words.push_back(previous_initial.toStdString());
                    start_index = end_index;
                    initial_found = false;
                    continue;
                }
            }
        }

        // First, check for initials
        // If initial is valid, then extend the end_index for length of initial
        // cluster of consonants.
        for (int length = 2; length > 0; length--) {
            stringToExamine = string.mid(end_index, length).toLower();
            auto searchResult = initials.find(stringToExamine.toStdString());

            if (searchResult == initials.end()
                || stringToExamine.length() != length) {
                continue;
            }

            // Multiple initials in a row are only valid if previous "initial"
            // was actually a final (like m or ng)
            if (initial_found) {
                QString previous_initial = string.mid(start_index,
                                                      end_index - start_index);
                searchResult = finals.find(previous_initial.toStdString());
                if (searchResult != finals.end()) {
                    words.push_back(previous_initial.toStdString());
                    start_index = end_index;
                    initial_found = false;
                }
            }

            end_index += length;
            next_iteration = true;
            initial_found = true;
        }

        if (next_iteration) {
            continue;
        }

        // Then, check for finals
        // If final is valid, then extend end_index for length of final.
        // Check for number at end of word as well (this represents tone number).
        //
        // Then add the substring from [start_index, end_index) to vector
        // and reset start_index, so we can start searching after the end_index.
        for (int length = 4; length > 0; length--) {
            stringToExamine = string.mid(end_index, length).toLower();
            auto searchResult = finals.find(stringToExamine.toStdString());
            if (searchResult != finals.end()
                && stringToExamine.length() == length) {
                end_index += length;
                if (end_index < string.length()) {
                    if (string.at(end_index).isDigit()) {
                        end_index++;
                    }
                }
                QString word = string.mid(start_index, end_index - start_index);
                words.push_back(word.toStdString());
                start_index = end_index;
                next_iteration = true;
                initial_found = false;
                break;
            }
        }

        if (next_iteration) {
            continue;
        }

        end_index++;
    }

    // Then add whatever's left in the search term, minus whitespace.
    QString lastWord = string.mid(start_index, end_index - start_index)
                           .simplified();
    if (!lastWord.isEmpty() && lastWord != "'") {
        words.push_back(lastWord.toStdString());
    }

    return words;
}

}
