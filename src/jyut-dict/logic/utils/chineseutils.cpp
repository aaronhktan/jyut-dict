#include "chineseutils.h"

#include "logic/utils/utils.h"

#include <cctype>
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <regex>
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
    ".",
    ",",
    "，",
    "!",
    "！",
    "?",
    "？",
    "%",
    "－",
    "…",
    "⋯",
    ".",
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

const static std::unordered_map<std::string, std::string>
    jyutpingToYaleSpecialFinals = {
        {"aa", "a"},
        {"oe", "eu"},
        {"oeng", "eung"},
        {"oek", "euk"},
        {"eoi", "eui"},
        {"eon", "eun"},
        {"eot", "eut"},
};

const static std::unordered_map<std::string, std::vector<std::string>>
    jyutpingToYaleSpecialSyllables = {
        {"m", {"m̄", "ḿ", "m", "m̀h", "ḿh", "mh"}},
        {"ng", {"n̄g", "ńg", "ng", "ǹgh", "ńgh", "ngh"}},
};

const static std::unordered_map<std::string, std::vector<std::string>>
    yaleReplacementMap = {{"a", {"ā", "á", "a", "à", "á", "a"}},
                          {"e", {"ē", "é", "e", "è", "é", "e"}},
                          {"i", {"ī", "í", "i", "ì", "í", "i"}},
                          {"o", {"ō", "ó", "o", "ò", "ó", "o"}},
                          {"u", {"ū", "ú", "u", "ù", "ú", "u"}},
};

// The original Wiktionary module uses breves to indicate a special letter (e.g.
// ă), but the base C++ regex engine can't match against chars outside of the
// basic set. As a workaround, I'm just replacing them with other symbols.
const static std::vector<std::pair<std::string, std::string> >
    jyutpingToIPASpecialSyllables = {{"a", "@"},
                                     {"yu", "y"},
                                     {"@@", "a"},
                                     {"uk", "^k"},
                                     {"ik", "|k"},
                                     {"ou", "~u"},
                                     {"eoi", "eoy"},
                                     {"ung", "^ng"},
                                     {"ing", "|ng"},
                                     {"ei", ">i"}};

const static std::unordered_map<std::string, std::string> jyutpingToIPAInitials
    = {
        {"b", "p"},
        {"p", "pʰ"},
        {"d", "t"},
        {"t", "tʰ"},
        {"g", "k"},
        {"k", "kʰ"},
        {"ng", "ŋ"},
        {"gw", "kʷ"},
        {"kw", "kʷʰ"},
        {"zh", "t͡ʃ"},
        {"ch", "t͡ʃʰ"},
        {"sh", "ʃ"},
        {"z", "t͡s"},
        {"c", "t͡sʰ"},
};

const static std::unordered_map<std::string, std::string> jyutpingToIPANuclei = {
    {"a", "äː"},
    {"@", "ɐ"},
    {"e", "ɛː"},
    {">", "e"},
    {"i", "iː"},
    {"|", "ɪ"},
    {"o", "ɔː"},
    {"~", "o"},
    {"oe", "œ̽ː"},
    {"eo", "ɵ"},
    {"u", "uː"},
    {"^", "ʊ"},
    {"y", "yː"},
};

const static std::unordered_map<std::string, std::string> jyutpingToIPACodas = {
    {"i", "i̯"},
    {"u", "u̯"},
    {"y", "y̯"},
    {"ng", "ŋ"},
    {"p", "p̚"},
    {"t", "t̚"},
    {"k", "k̚"},
};

#if defined(Q_OS_MAC)
// Added a six-per-em space (U+2006) between adjacent tone markers, because Qt's
// kerning squishes them too close together
const static std::vector<std::string> jyutpingToIPATones
    = {"˥", "˧ ˥", "˧", "˨ ˩", "˩ ˧", "˨", "˥", "˧", "˨"};
#else
const static std::vector<std::string> jyutpingToIPATones
    = {"˥", "˧˥", "˧", "˨˩", "˩˧", "˨", "˥", "˧", "˨"};
#endif

const static std::unordered_map<std::string, std::string> zhuyinInitialMap = {
    {"b", "ㄅ"},  {"p", "ㄆ"}, {"m", "ㄇ"}, {"f", "ㄈ"},  {"d", "ㄉ"},
    {"t", "ㄊ"},  {"n", "ㄋ"}, {"l", "ㄌ"}, {"g", "ㄍ"},  {"k", "ㄎ"},
    {"h", "ㄏ"},  {"j", "ㄐ"}, {"q", "ㄑ"}, {"x", "ㄒ"},  {"z", "ㄗ"},
    {"c", "ㄘ"},  {"s", "ㄙ"}, {"r", "ㄖ"}, {"zh", "ㄓ"}, {"ch", "ㄔ"},
    {"sh", "ㄕ"},
};

const static std::unordered_map<std::string, std::string> zhuyinFinalMap
    = {{"yuan", "ㄩㄢ"}, {"iang", "ㄧㄤ"}, {"yang", "ㄧㄤ"}, {"uang", "ㄨㄤ"},
       {"wang", "ㄨㄤ"}, {"ying", "ㄧㄥ"}, {"weng", "ㄨㄥ"}, {"iong", "ㄩㄥ"},
       {"yong", "ㄩㄥ"}, {"uai", "ㄨㄞ"},  {"wai", "ㄨㄞ"},  {"yai", "ㄧㄞ"},
       {"iao", "ㄧㄠ"},  {"yao", "ㄧㄠ"},  {"ian", "ㄧㄢ"},  {"yan", "ㄧㄢ"},
       {"uan", "ㄨㄢ"},  {"wan", "ㄨㄢ"},  {"van", "ㄩㄢ"},  {"ang", "ㄤ"},
       {"yue", "ㄩㄝ"},  {"wei", "ㄨㄟ"},  {"you", "ㄧㄡ"},  {"yin", "ㄧㄣ"},
       {"wen", "ㄨㄣ"},  {"yun", "ㄩㄣ"},  {"eng", "ㄥ"},    {"ing", "ㄧㄥ"},
       {"ong", "ㄨㄥ"},  {"io", "ㄧㄛ"},   {"yo", "ㄧㄛ"},   {"ia", "ㄧㄚ"},
       {"ya", "ㄧㄚ"},   {"ua", "ㄨㄚ"},   {"wa", "ㄨㄚ"},   {"ai", "ㄞ"},
       {"ao", "ㄠ"},     {"an", "ㄢ"},     {"ie", "ㄧㄝ"},   {"ye", "ㄧㄝ"},
       {"uo", "ㄨㄛ"},   {"wo", "ㄨㄛ"},   {"ue", "ㄩㄝ"},   {"ve", "ㄩㄝ"},
       {"ei", "ㄟ"},     {"ui", "ㄨㄟ"},   {"ou", "ㄡ"},     {"iu", "ㄧㄡ"},
       {"en", "ㄣ"},     {"in", "ㄧㄣ"},   {"un", "ㄨㄣ"},   {"vn", "ㄩㄣ"},
       {"yi", "ㄧ"},     {"wu", "ㄨ"},     {"yu", "ㄩ"},     {"a", "ㄚ"},
       {"e", "ㄜ"},      {"o", "ㄛ"},      {"i", "ㄧ"},      {"u", "ㄨ"},
       {"v", "ㄩ"},      {"ê", "ㄝ"}};

const static std::vector<std::string> zhuyinTones = {"", "", "ˊ", "ˇ", "ˋ", "˙"};

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
    for (const auto &character : converted_original) {
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
                                           [](unsigned char c){ return std::isalpha(c); })
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
        case EntryColourPhoneticType::CANTONESE: {
            coloured_string += "<font color=\""
                               + jyutpingToneColours.at(
                                   static_cast<size_t>(tone))
                               + "\">";
            break;
        }
        case EntryColourPhoneticType::MANDARIN: {
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

std::string compareStrings(const std::string &original,
                           const std::string &comparison)
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

static std::string convertYaleInitial(const std::string &syllable)
{
    std::string yale_syllable{syllable};
    yale_syllable = std::regex_replace(yale_syllable, std::regex{"jy?"}, "y");
    yale_syllable = std::regex_replace(yale_syllable, std::regex{"z"}, "j");
    yale_syllable = std::regex_replace(yale_syllable, std::regex{"c"}, "ch");
    return yale_syllable;
}

static std::string convertYaleFinal(const std::string &syllable)
{
    std::string yale_syllable{syllable};

    // Attempt to isolate the part of the Jyutping syllable that is the final
    std::regex final_regex{"([aeiou][aeiou]?[iumngptk]?[g]?)([1-6])"};
    std::smatch match;
    auto regex_res = std::regex_search(syllable, match, final_regex);

    if (!regex_res) {
        std::cerr << "No final found!" << std::endl;
        return yale_syllable;
    }

    std::string final = match[1].str();
    int tone = std::stoi(match[2]);

    auto final_location = yale_syllable.find(final);

    // Some Jyutping finals have significant differences when mapped to Yale.
    // Switch it out here.
    auto replacement_final_search = jyutpingToYaleSpecialFinals.find(final);
    if (replacement_final_search != jyutpingToYaleSpecialFinals.end()) {
        yale_syllable.erase(final_location, final.length());
        yale_syllable.insert(final_location, replacement_final_search->second);
    }

    // Insert an "h" before the last consonant cluster for the light tones,
    // as they are indicated in Yale
    if (tone == 4 || tone == 5 || tone == 6) {
        yale_syllable = std::regex_replace(yale_syllable,
                                           std::regex{"([ptkmn]?g?)[123456]$"},
                                           "h$&");
    }

    // Replace the first vowel in the final with its accented version
    auto replacement_location = yale_syllable.find_first_of("aeiou");
    std::string first_vowel = yale_syllable.substr(replacement_location, 1);
    auto replacement_vowel_search = yaleReplacementMap.find(first_vowel);
    std::string replacement_vowel = (replacement_vowel_search
                                     == yaleReplacementMap.end())
                                        ? first_vowel
                                        : replacement_vowel_search->second.at(
                                            static_cast<size_t>(tone - 1));
    yale_syllable.erase(replacement_location, first_vowel.length());
    yale_syllable.insert(replacement_location, replacement_vowel);

    // Remove the tone number, as Yale doesn't use those
    yale_syllable.erase(yale_syllable.cend() - 1);

    return yale_syllable;
}

// Note that the majority of this function and the convertToIPA function
// is derivative of Wiktionary's conversion code, contained in the module
// "yue-pron" (https://en.wiktionary.org/wiki/Module:yue-pron)
std::string convertJyutpingToYale(const std::string &jyutping,
                                  bool useSpacesToSegment)
{
    if (jyutping.empty()) {
        return jyutping;
    }

    std::vector<std::string> syllables;
    if (useSpacesToSegment) {
        std::string jyutpingCopy;
        // Insert a space before and after every special character, so that the
        // IPA conversion doesn't attempt to convert special characters.
        std::u32string jyutping_utf32 = converter.from_bytes(jyutping);
        for (const auto &character : jyutping_utf32) {
            std::string character_utf8 = converter.to_bytes(character);
            if (specialCharacters.find(character_utf8)
                != specialCharacters.end()) {
                jyutpingCopy += " " + character_utf8 + " ";
            } else {
                jyutpingCopy += character_utf8;
            }
        }
        Utils::split(jyutpingCopy, ' ', syllables);
    } else {
        syllables = segmentJyutping(QString{jyutping.c_str()},
                                    /* ignoreSpecialCharacters */ false);
    }

    std::vector<std::string> yale_syllables;

    for (const auto &syllable : syllables) {
        // Skip syllables that are just punctuation
        if (specialCharacters.find(syllable) != specialCharacters.end()) {
            yale_syllables.push_back(syllable);
            continue;
        }

        // Skip syllables that don't have tone
        auto location = syllable.find_first_of("123456");
        if (location == std::string::npos) {
            yale_syllables.push_back(syllable);
            continue;
        }

        // Handle special-case syllables
        std::string syllable_without_tone = syllable.substr(0,
                                                            syllable.length()
                                                                - 1);
        int tone = std::stoi(
            syllable.substr(syllable.find_first_of("123456"), 1));
        auto search = jyutpingToYaleSpecialSyllables.find(syllable_without_tone);
        if (search != jyutpingToYaleSpecialSyllables.end()) {
            yale_syllables.emplace_back(
                search->second.at(static_cast<size_t>(tone) - 1));
            continue;
        }

        std::string yale_syllable{syllable};

        yale_syllable = convertYaleFinal(yale_syllable);
        yale_syllable = convertYaleInitial(yale_syllable);
        yale_syllables.emplace_back(yale_syllable);
    }

    std::ostringstream yale;
    for (const auto &yale_syllable : yale_syllables) {
        yale << yale_syllable << " ";
    }

    // Remove trailing space
    std::string result = yale.str();
    result.erase(result.end() - 1);

    return result;
}

std::string convertIPASyllable(const std::string &syllable)
{
    std::string initial;
    std::string nucleus;
    std::string coda;
    std::string tone;

    std::regex syllable_regex{"([bcdfghjklmnpqrstvwxyz]?[bcdfghjklmnpqrstvwxyz]"
                              "?)([a@e>i|o~u^y][eo]?)([iuymngptk]?g?)([1-9])"};
    std::smatch match;

    auto regex_res = std::regex_match(syllable, match, syllable_regex);

    if (!regex_res) {
        // No valid Jyutping found
        return syllable;
    }

    // Get equivalent to matched initial
    if (match[1].length()) {
        if (jyutpingToIPAInitials.find(match[1])
            != jyutpingToIPAInitials.end()) {
            initial = jyutpingToIPAInitials.at(match[1]);
        } else {
            initial = match[1];
        }
    }

    // Get equivalent to matched nucleus
    if (match[2].length()) {
        if (jyutpingToIPANuclei.find(match[2]) != jyutpingToIPANuclei.end()) {
            nucleus = jyutpingToIPANuclei.at(match[2]);
        } else {
            nucleus = match[2];
        }
    }

    // Get equivalent to matched final
    if (match[3].length()) {
        if (jyutpingToIPACodas.find(match[3]) != jyutpingToIPACodas.end()) {
            coda = jyutpingToIPACodas.at(match[3]);
        } else {
            coda = match[3];
        }
    }

    // Get equivalent to matched tone
    if (match[4].length()) {
        tone = jyutpingToIPATones.at(
            static_cast<size_t>(std::stoi(match[4]) - 1));
    }

#if defined(Q_OS_MAC)
    // Added a thin space (U+2009) before tone to better distinguish unreleased
    // stop marker and tone markers
    std::string ipa_syllable = initial + nucleus + coda + " " + tone;
#else
    std::string ipa_syllable = initial + nucleus + coda + tone;
#endif
    return ipa_syllable;
}

std::string convertJyutpingToIPA(const std::string &jyutping,
                                 bool useSpacesToSegment)
{
    if (jyutping.empty()) {
        return jyutping;
    }

    std::vector<std::string> syllables;
    if (useSpacesToSegment) {
        std::string jyutpingCopy;
        // Insert a space before and after every special character, so that the
        // IPA conversion doesn't attempt to convert special characters.
        std::u32string jyutping_utf32 = converter.from_bytes(jyutping);
        for (const auto &character : jyutping_utf32) {
            std::string character_utf8 = converter.to_bytes(character);
            if (specialCharacters.find(character_utf8)
                != specialCharacters.end()) {
                jyutpingCopy += " " + character_utf8 + " ";
            } else {
                jyutpingCopy += character_utf8;
            }
        }
        Utils::split(jyutpingCopy, ' ', syllables);
    } else {
        syllables = segmentJyutping(QString{jyutping.c_str()},
                                    /* ignoreSpecialCharacters */ false);
    }

    std::vector<std::string> ipa_syllables;

    for (const auto &syllable : syllables) {
        // Skip syllables that are just punctuation
        if (specialCharacters.find(syllable) != specialCharacters.end()) {
            ipa_syllables.push_back(syllable);
            continue;
        }

        // Skip syllables that don't have tone
        auto location = syllable.find_first_of("123456");
        if (location == std::string::npos) {
            ipa_syllables.push_back(syllable);
            continue;
        }

        // Do some pre-processing
        std::string ipa_syllable{syllable};
        ipa_syllable = std::regex_replace(ipa_syllable,
                                          std::regex{"([zcs])yu"},
                                          "$1hyu");
        ipa_syllable = std::regex_replace(ipa_syllable,
                                          std::regex{"([zc])oe"},
                                          "$1hoe");
        ipa_syllable = std::regex_replace(ipa_syllable,
                                          std::regex{"([zc])eo"},
                                          "$1heo");

        // Convert special syllables
        std::smatch match;
        if (std::regex_match(ipa_syllable,
                             match,
                             std::regex{"^(h?)([mn]g?)([1-6])$"})) {
            int tone = std::stoi(match[3]);
            ipa_syllable = std::regex_replace(ipa_syllable,
                                              std::regex{"m"},
                                              "m̩");
            ipa_syllable = std::regex_replace(ipa_syllable,
                                              std::regex{"ng"},
                                              "ŋ̍");
            ipa_syllable = std::regex_replace(
                ipa_syllable,
                std::regex{"[1-6]"},
#if defined(Q_OS_MAC)
                // Only macOS needs this space to fix weird kerning
                " " +
#endif
                    jyutpingToIPATones.at(static_cast<size_t>(tone - 1)));
        }

        // Replace checked tones
        if (std::regex_search(ipa_syllable,
                              match,
                              std::regex{"([ptk])([136])"})) {
            std::replace(ipa_syllable.begin(), ipa_syllable.end(), '1', '7');
            std::replace(ipa_syllable.begin(), ipa_syllable.end(), '3', '8');
            std::replace(ipa_syllable.begin(), ipa_syllable.end(), '6', '9');
        }

        // Do some more preprocessing
        for (const auto &pair : jyutpingToIPASpecialSyllables) {
            ipa_syllable = std::regex_replace(ipa_syllable,
                                              std::regex{pair.first},
                                              pair.second);
        }

        ipa_syllables.emplace_back(convertIPASyllable(ipa_syllable));
    }

    std::ostringstream ipa;
    std::string double_space = "  ";
    for (const auto &ipa_syllable : ipa_syllables) {
        ipa << ipa_syllable << double_space;
    }

    // Remove trailing space
    std::string result = ipa.str();
    result.erase(result.end() - double_space.length());

    return result;
}

std::string createPrettyPinyin(const std::string &pinyin)
{
    if (pinyin.empty()) {
        return pinyin;
    }

    std::string result;

    // Create a vector of each space-separated value in pinyin
    std::vector<std::string> syllables;
    Utils::split(pinyin, ' ', syllables);
    if (syllables.empty()) {
        return pinyin;
    }

    for (auto &syllable : syllables) {
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
        auto search = replacementMap.find(
            syllable.substr(location, character_size));
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

std::string createNumberedPinyin(const std::string &pinyin)
{
    if (pinyin.empty()) {
        return pinyin;
    }

    std::string result;

    std::vector<std::string> syllables;
    Utils::split(pinyin, ' ', syllables);
    if (syllables.empty()) {
        return pinyin;
    }

    for (auto &syllable : syllables) {
        size_t location = syllable.find("u:");
        while (location != std::string::npos) {
            syllable.erase(location, 2);
            syllable.insert(location, "ü");
            location = syllable.find("u:");
        }

        result += syllable + " ";
    }

    // Remove trailing space
    result.erase(result.end() - 1);

    return result;
}

std::string createPinyinWithV(const std::string &pinyin)
{
    if (pinyin.empty()) {
        return pinyin;
    }

    std::string result;

    std::vector<std::string> syllables;
    Utils::split(pinyin, ' ', syllables);
    if (syllables.empty()) {
        return pinyin;
    }

    for (auto &syllable : syllables) {
        size_t location = syllable.find("u:");
        while (location != std::string::npos) {
            syllable.erase(location, 2);
            syllable.insert(location, "v");
            location = syllable.find("u:");
        }

        result += syllable + " ";
    }

    // Remove trailing space
    result.erase(result.end() - 1);

    return result;
}

std::string convertPinyinToZhuyin(const std::string &pinyin)
{
    return convertPinyinToZhuyin(pinyin, /* useSpacesToSegment */ false);
}

// Note that the majority of this code is derivative of Wiktionary's conversion
// code, contained in the module cmn-pron
// (https://en.wiktionary.org/wiki/Module:cmn-pron)
std::string convertPinyinToZhuyin(const std::string &pinyin,
                                  bool useSpacesToSegment)
{
    if (pinyin.empty()) {
        return pinyin;
    }

    std::vector<std::string> syllables;
    if (useSpacesToSegment) {
        // Insert a space before and after every special character, so that the
        // Yale conversion doesn't attempt to convert special characters.
        std::string pinyinCopy = pinyin;
        for (const auto &specialCharacter : specialCharacters) {
            size_t location = pinyinCopy.find(specialCharacter);
            if (location != std::string::npos) {
                pinyinCopy.erase(location, specialCharacter.length());
                pinyinCopy.insert(location, " " + specialCharacter + " ");
            }
        }
        Utils::split(pinyinCopy, ' ', syllables);
    } else {
        syllables = segmentPinyin(QString{pinyin.c_str()});
    }

    std::vector<std::string> zhuyin_syllables;

    for (const auto &syllable : syllables) {
        // Skip syllables that are just punctuation
        if (specialCharacters.find(syllable) != specialCharacters.end()) {
            zhuyin_syllables.push_back(syllable);
            continue;
        }

        // Skip syllables that don't have tone
        auto location = syllable.find_first_of("12345");
        if (location == std::string::npos) {
            zhuyin_syllables.push_back(syllable);
            continue;
        }
        unsigned long tone = static_cast<unsigned long>(
            std::stoi(syllable.substr(location, 1)));

        std::string zhuyin_syllable{syllable};
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"u\\:"},
                                             "v");
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"([jqx])u"},
                                             "$1v");
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"([zcs]h?)i"},
                                             "$1");
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"([r])i"},
                                             "$1");

        // Handle special cases
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"^ng([012345])$"},
                                             "ㄫ$1");
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"^hm([012345])$"},
                                             "ㄏㄇ$1");
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"^hng([012345])$"},
                                             "ㄏㄫ$1");
        zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                             std::regex{"^er([012345])$"},
                                             "ㄦ$1");

        // Handle general case
        // Convert Pinyin initial
        std::smatch initial_match;
        std::regex pinyin_initial = std::regex{"^([bpmfdtnlgkhjqxzcsr]?h?)"};
        if (std::regex_search(zhuyin_syllable, initial_match, pinyin_initial)) {
            if (initial_match[1].length()) {
                zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                                     pinyin_initial,
                                                     zhuyinInitialMap.at(
                                                         initial_match[1]));
            }
        }
        // Convert Pinyin final
        std::smatch final_match;
        std::regex pinyin_final = std::regex{
            "([aeiouêvyw]?[aeioun]?[aeioung]?[ng]?)(r?)([012345])$"};
        if (std::regex_search(zhuyin_syllable, final_match, pinyin_final)) {
            std::string final;
            std::string er;
            if (final_match[1].length()) {
                final = zhuyinFinalMap.at(final_match[1]);
            }
            if (final_match[2].length()) {
                er = "ㄦ";
            }
            zhuyin_syllable = std::regex_replace(zhuyin_syllable,
                                                 pinyin_final,
                                                 final + er);
        }

        // Add tone to zhuyin syllable
        if (tone == 5) {
            zhuyin_syllable = zhuyinTones[tone] + zhuyin_syllable;
        } else {
            auto er_pos = zhuyin_syllable.find("ㄦ");
            if (er_pos != std::string::npos && zhuyin_syllable != "ㄦ") {
                zhuyin_syllable.insert(er_pos, zhuyinTones[tone]);
            } else {
                zhuyin_syllable = zhuyin_syllable + zhuyinTones[tone];
            }
        }

        zhuyin_syllables.emplace_back(zhuyin_syllable);
    }

    std::ostringstream zhuyin;
    for (const auto &zhuyin_syllable : zhuyin_syllables) {
        zhuyin << zhuyin_syllable << " ";
    }

    // Remove trailing space
    std::string result = zhuyin.str();
    result.erase(result.end() - 1);

    return result;
}

std::string constructRomanisationQuery(const std::vector<std::string> &words,
                                       const char *delimiter,
                                       const bool surroundWithQuotes)
{
    const char *quotes = surroundWithQuotes ? "\"" : "";
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

std::vector<std::string> segmentPinyin(const QString &string)
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

std::vector<std::string> segmentJyutping(const QString &string)
{
    return segmentJyutping(string, /* ignoreSpecialCharacters */ true);
}

std::vector<std::string> segmentJyutping(const QString &string,
                                         bool ignoreSpecialCharacters)
{
    std::vector<std::string> words;

    std::unordered_set<std::string> initials = {"b",  "p", "m",  "f",  "d",
                                                "t",  "n", "l",  "g",  "k",
                                                "ng", "h", "gw", "kw", "w",
                                                "z",  "c", "s",  "j",  "m"};
    std::unordered_set<std::string> finals
        = {"a",   "aa",  "aai", "aau", "aam", "aan", "aang", "aap", "aat",
           "aak", "ai",  "au",  "am",  "an",  "ang", "ap",   "at",  "ak",
           "e",   "ei",  "eu",  "em",  "en",  "eng", "ep",   "ek",  "i",
           "iu",  "im",  "in",  "ing", "ip",  "it",  "ik",   "o",   "oi",
           "ou",  "on",  "ong", "ot",  "ok",  "u",   "ui",   "un",  "ung",
           "ut",  "uk",  "oe",  "oet", "eoi", "eon", "oeng", "eot", "oek",
           "yu",  "yun", "yut", "m",   "ng"};

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
        bool isSpecialCharacter = (specialCharacters.find(
                                       stringToExamine.toStdString())
                                   != specialCharacters.end());
        if (stringToExamine == " " || stringToExamine == "'"
            || isSpecialCharacter) {
            if (initial_found) { // Add any incomplete word to the vector
                QString previous_initial = string.mid(start_index,
                                                      end_index - start_index);
                words.push_back(previous_initial.toStdString());
                start_index = end_index;
                initial_found = false;
            }
            if (!ignoreSpecialCharacters && isSpecialCharacter) {
                words.push_back(stringToExamine.toStdString());
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
} // namespace ChineseUtils
