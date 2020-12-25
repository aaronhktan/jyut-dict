#include "chineseutils.h"

#include "logic/utils/utils.h"
#include "logic/settings/settings.h"

#include <codecvt>
#include <iomanip>
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
    std::string &original,
    std::vector<int> &tones,
    std::vector<std::string> &jyutpingToneColours,
    std::vector<std::string> &pinyinToneColours,
    EntryColourPhoneticType type)
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
        } catch (const std::out_of_range &e) {
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

std::string compareStrings(std::string &original,
                           std::string &comparison)
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

std::string createPrettyPinyin(std::string &pinyin)
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
            std::string replacement = search->second.at(static_cast<size_t>(tone) - 1);
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
}
