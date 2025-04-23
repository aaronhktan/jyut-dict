#include "mandarinutils.h"

#include "logic/utils/utils.h"

#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace MandarinUtils {

const static std::unordered_set<std::string> specialCharacters = {
    ".",  "。", ",",  "，", "！", "？", "%",  "－", "…",  "⋯",
    ".",  "·",  "\"", "“",  "”",  "$",  "｜", "：", "１", "２",
    "３", "４", "５", "６", "７", "８", "９", "０",
};

const static std::unordered_set<std::string> regexCharacters = {
    "!",
    "(",
    ")",
    "|",
};

const static std::unordered_map<std::string, std::vector<std::string>>
    pinyinToneReplacements = {
        {"a", {"ā", "á", "ǎ", "à", "a"}},
        {"e", {"ē", "é", "ě", "è", "e"}},
        {"i", {"ī", "í", "ǐ", "ì", "i"}},
        {"o", {"ō", "ó", "ǒ", "ò", "o"}},
        {"u", {"ū", "ú", "ǔ", "ù", "u"}},
        {"ü", {"ǖ", "ǘ", "ǚ", "ǜ", "ü"}},
};

const static std::unordered_map<std::string, std::string> zhuyinInitials = {
    {"b", "ㄅ"},  {"p", "ㄆ"}, {"m", "ㄇ"}, {"f", "ㄈ"},  {"d", "ㄉ"},
    {"t", "ㄊ"},  {"n", "ㄋ"}, {"l", "ㄌ"}, {"g", "ㄍ"},  {"k", "ㄎ"},
    {"h", "ㄏ"},  {"j", "ㄐ"}, {"q", "ㄑ"}, {"x", "ㄒ"},  {"z", "ㄗ"},
    {"c", "ㄘ"},  {"s", "ㄙ"}, {"r", "ㄖ"}, {"zh", "ㄓ"}, {"ch", "ㄔ"},
    {"sh", "ㄕ"},
};

const static std::unordered_map<std::string, std::string> zhuyinFinals
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

const static std::unordered_set<std::string> mandarinIPAGlottal = {
    "a", "o", "e", "ai", "ei", "ao", "ou", "an", "en", "er", "ang", "ong", "eng"};

const static std::unordered_map<std::string, std::string> mandarinIPAInitials = {
    {"b", "p"},  {"c", "t͡sʰ"}, {"ch", "ʈ͡ʂʰ"}, {"d", "t"},  {"f", "f"},
    {"g", "k"},  {"h", "x"},   {"j", "t͡ɕ"},   {"k", "kʰ"}, {"l", "l"},
    {"m", "m"},  {"n", "n"},   {"ng", "ŋ"},   {"p", "pʰ"}, {"q", "t͡ɕʰ"},
    {"r", "ʐ"},  {"s", "s"},   {"sh", "ʂ"},   {"t", "tʰ"}, {"x", "ɕ"},
    {"z", "t͡s"}, {"zh", "ʈ͡ʂ"},
};

const static std::unordered_map<std::string, std::string> mandarinIPAFinals = {
    {"a", "ä"},       {"ai", "aɪ̯"},      {"air", "ɑɻ"},     {"an", "än"},
    {"ang", "ɑŋ"},    {"angr", "ɑ̃ɻ"},    {"anr", "ɑɻ"},     {"ao", "ɑʊ̯"},
    {"aor", "aʊ̯ɻʷ"},  {"ar", "ɑɻ"},      {"e", "ɤ"},        {"ei", "eɪ̯"},
    {"eir", "əɻ"},    {"en", "ən"},      {"eng", "ɤŋ"},     {"engr", "ɤ̃ɻ"},
    {"enr", "əɻ"},    {"er", "ɤɻ"},      {"i", "i"},        {"ia", "jä"},
    {"ian", "jɛn"},   {"iang", "jɑŋ"},   {"iangr", "jɑ̃ɻ"},  {"ianr", "jɑɻ"},
    {"iao", "jɑʊ̯"},   {"iaor", "jaʊ̯ɻʷ"}, {"iar", "jɑɻ"},    {"ie", "jɛ"},
    {"ier", "jɛɻ"},   {"in", "in"},      {"ing", "iŋ"},     {"ingr", "iɤ̯̃ɻ"},
    {"inr", "iə̯ɻ"},   {"io", "jɔ"},      {"iong", "jʊŋ"},   {"iongr", "jʊ̃ɻ"},
    {"ir", "iə̯ɻ"},    {"iu", "joʊ̯"},     {"iur", "jɤʊ̯ɻʷ"},  {"m", "m̩"},
    {"n", "n̩"},       {"ng", "ŋ̍"},       {"o", "wɔ"},       {"ong", "ʊŋ"},
    {"ongr", "ʊ̃ɻ"},   {"or", "wɔɻ"},     {"ou", "oʊ̯"},      {"our", "ɤʊ̯ɻʷ"},
    {"u", "u"},       {"ua", "wä"},      {"uai", "waɪ̯"},    {"uair", "wɑɻ"},
    {"uan", "wän"},   {"uang", "wɑŋ"},   {"uangr", "wɑ̃ɻ"},  {"uanr", "wɑɻ"},
    {"uar", "u̯ɑɻ"},   {"ue", "ɥɛ"},      {"ui", "weɪ̯"},     {"uir", "wəɻ"},
    {"un", "wən"},    {"unr", "wəɻ"},    {"uo", "wɔ"},      {"uor", "wɔɻ"},
    {"ur", "uɻʷ"},    {"v", "y"},        {"van", "ɥɛn"},    {"vanr", "ɥɑɻ"},
    {"ve", "ɥɛ"},     {"ver", "ɥɛɻ"},    {"vn", "yn"},      {"vnr", "yə̯ɻ"},
    {"vr", "yə̯ɻ"},    {"wa", "wä"},      {"wai", "waɪ̯"},    {"wair", "wɑɻ"},
    {"wan", "wän"},   {"wang", "wɑŋ"},   {"wangr", "wɑ̃ɻ"},  {"wanr", "wɑɻ"},
    {"war", "wɑɻ"},   {"wei", "weɪ̯"},    {"weir", "wəɻ"},   {"wen", "wən"},
    {"weng", "wəŋ"},  {"wengr", "ʊ̃ɻ"},   {"wenr", "wəɻ"},   {"wo", "wɔ"},
    {"wor", "wɔɻ"},   {"wu", "u"},       {"wur", "uɻʷ"},    {"ya", "jä"},
    {"yai", "jaɪ̯"},   {"yan", "jɛn"},    {"yang", "jɑŋ"},   {"yangr", "jɑ̃ɻ"},
    {"yanr", "jɑɻ"},  {"yao", "jɑʊ̯"},    {"yaor", "jaʊ̯ɻʷ"}, {"yar", "jɑɻ"},
    {"ye", "jɛ"},     {"yer", "jɛɻ"},    {"yi", "i"},       {"yin", "in"},
    {"ying", "iŋ"},   {"yingr", "iɤ̯̃ɻ"},  {"yinr", "iə̯ɻ"},   {"yir", "iə̯ɻ"},
    {"yo", "jɔ"},     {"yong", "jʊŋ"},   {"yongr", "jʊ̃ɻ"},  {"yor", "jɔɻ"},
    {"you", "joʊ̯"},   {"your", "jɤʊ̯ɻʷ"}, {"yu", "y"},       {"yuan", "ɥɛn"},
    {"yuanr", "ɥɑɻ"}, {"yue", "ɥɛ"},     {"yuer", "ɥɛɻ"},   {"yun", "yn"},
    {"yunr", "yə̯ɻ"},  {"yur", "yə̯ɻ"},
};

const static std::unordered_map<std::string, std::string>
    mandarinIPAVoicelessInitials = {
        {"k", "g̊"},
        {"p", "b̥"},
        {"t", "d̥"},
        {"t͡s", "d͡z̥"},
        {"t͡ɕ", "d͡ʑ̥"},
        {"ʈ͡ʂ", "ɖ͡ʐ̥"},
};

#if defined(Q_OS_WIN)
// For consistency with other reasonings below, use superscript numbers instead
// of tone letters on Windows.
const static std::vector<std::string> mandarinIPANeutralTone
    = {"²", "³", "⁴", "¹", "¹"};
#else
const static std::vector<std::string> mandarinIPANeutralTone
    = {"˨", "˧", "˦", "˩", "˩"};
#endif

#if defined(Q_OS_MAC)
// Added a six-per-em space (U+2006) between adjacent tone markers, because Qt's
// kerning squishes them too close together
const static std::vector<std::string> mandarinIPAThirdTone
    = {"˨ ˩ ˦ ꜕ ꜖ ꜖", "˨ ˩ ˦ ꜕ ꜖ ꜖", "˨ ˩ ˦ ꜔ ꜒", "˨ ˩ ˦ ꜕ ꜖ ꜖", "˨ ˩ ˦"};
#elif defined(Q_OS_WIN)
// On Windows, the reverse tone letters are not the same height as the "normal"
// tone letters. In addition, the Segoe UI font cannot handle three tone-letter
// ligatures. As such, use superscript numbers instead.
const static std::vector<std::string> mandarinIPAThirdTone
    = {"²¹⁴⁻²¹¹", "²¹⁴⁻²¹¹", "²¹⁴⁻³⁵", "²¹⁴⁻²¹¹", "²¹⁴"};
#else
const static std::vector<std::string> mandarinIPAThirdTone
    = {"˨˩˦꜕꜖꜖", "˨˩˦꜕꜖꜖", "˨˩˦꜔꜒", "˨˩˦꜕꜖꜖", "˨˩˦"};
#endif

#if defined(Q_OS_MAC)
// Added a six-per-em space (U+2006) between adjacent tone markers, because Qt's
// kerning squishes them too close together
const static std::vector<std::string> mandarinIPATones
    = {"˥ ˥", "˧ ˥", "˨ ˩ ˦", "˥ ˩", ""};
#elif defined(Q_OS_WIN)
// The Segoe UI font cannot handle three tone-letter ligatures. As such, use
// superscript numbers instead.
const static std::vector<std::string> mandarinIPATones
    = {"⁵⁵", "³⁵", "²¹⁴", "⁵¹", ""};
#else
const static std::vector<std::string> mandarinIPATones
    = {"˥˥", "˧˥", "˨˩˦", "˥˩", ""};
#endif

std::string createPrettyPinyin(const std::string &pinyin)
{
    if (pinyin.empty()) {
        return pinyin;
    }

    std::string result;

    // Create a vector of each space-separated value in pinyin
    std::vector<std::string> syllables;
    MandarinUtils::segmentPinyin(QString::fromStdString(pinyin), syllables);
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
        auto search = pinyinToneReplacements.find(
            syllable.substr(location, character_size));
        if (search != pinyinToneReplacements.end()) {
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
        std::string pinyinCopy;
        // Insert a space before and after every special character, so that the
        // Zhuyin conversion doesn't attempt to convert special characters.
        std::u32string pinyin_utf32 = QString::fromStdString(pinyin)
                                          .normalized(
                                              QString::NormalizationForm_C)
                                          .toStdU32String();
        for (const auto &character : pinyin_utf32) {
            std::string character_utf8 = QString::fromStdU32String(
                                             std::u32string{character})
                                             .toStdString();
            if (specialCharacters.find(character_utf8)
                != specialCharacters.end()) {
                pinyinCopy += " " + character_utf8 + " ";
            } else {
                pinyinCopy += character_utf8;
            }
        }
        Utils::split(pinyinCopy, ' ', syllables);
    } else {
        segmentPinyin(QString::fromStdString(pinyin), syllables);
    }

    std::vector<std::string> zhuyin_syllables;

    for (const auto &syllable : syllables) {
        // Most numbers, single characters, etc. are not Pinyin.
        // Filter those out.
        if (syllable.size() == 1) {
            zhuyin_syllables.emplace_back(syllable);
            continue;
        }

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
                                                     zhuyinInitials.at(
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
                auto final_match_pos = zhuyinFinals.find(final_match[1]);
                if (final_match_pos == zhuyinFinals.end()) {
                    // No final could be converted, give up
                    zhuyin_syllables.emplace_back(syllable);
                    continue;
                }
                final = zhuyinFinals.at(final_match[1]);
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

std::tuple<std::string, std::string> convertIPAMandarinSyllable(
    std::string &syllable)
{
    std::string ipa_initial;
    std::string ipa_final;

    // Check for special-case "ng", otherwise get replacement initial and final as normal
    if (syllable == "ng") {
        ipa_final = mandarinIPAFinals.at("ng");
    } else {
        std::regex initial_final_regex{"^([bcdfghjklmnpqrstxz]?h?)(.+)$"};
        std::smatch ipa_match;

        auto regex_res = std::regex_match(syllable,
                                          ipa_match,
                                          initial_final_regex);

        if (!regex_res) {
            std::cerr << "Invalid pinyin for IPA conversion!" << std::endl;
            return std::make_tuple("", syllable);
        }

        bool found_initial = !(mandarinIPAInitials.find(ipa_match[1])
                               == mandarinIPAInitials.end());
        bool found_final = !(mandarinIPAFinals.find(ipa_match[2])
                             == mandarinIPAFinals.end());
        if (!found_initial && !found_final) {
            return std::make_tuple("", syllable);
        }
        if (found_initial) {
            ipa_initial = mandarinIPAInitials.at(ipa_match[1]);
        }
        if (found_final) {
            ipa_final = mandarinIPAFinals.at(ipa_match[2]);
        }
    }

    // Replace close front unrounded vowel with syllabic retroflex sibilant
    // fricative (+ voiced retroflex approximant if erhua)
    // in Pinyin starting with ch, sh, zh, or r
    if (ipa_initial == "ʈ͡ʂʰ" || ipa_initial == "ʂ" || ipa_initial == "ʈ͡ʂ"
        || ipa_initial == "ʐ") {
        if (ipa_final == "ir") {
            ipa_final = "ʐ̩ɻ";
        } else if (ipa_final == "i") {
            ipa_final = "ʐ̩";
        }
    }

    // Replace close front unrounded vowel with syllabic alveolar sibilant
    // fricative (+ voiced retroflex approximant if erhua)
    // in Pinyin starting with c, s, or z
    if (ipa_initial == "t͡sʰ" || ipa_initial == "s" || ipa_initial == "t͡s") {
        if (ipa_final == "ir") {
            ipa_final = "z̩ɻ";
        } else if (ipa_final == "i") {
            ipa_final = "z̩";
        }
    }

    // Do some cleanup for Pinyin like "ri"
    if (ipa_initial == "ʐ" && ipa_final == "ʐ̩") {
        ipa_initial = "";
    }

    return std::make_tuple(ipa_initial, ipa_final);
}

std::string convertPinyinToIPA(const std::string &pinyin,
                               bool useSpacesToSegment)
{
    if (pinyin.empty()) {
        return pinyin;
    }

    std::vector<std::string> syllables;
    if (useSpacesToSegment) {
        std::string pinyinCopy;
        // Insert a space before and after every special character, so that the
        // IPA conversion doesn't attempt to convert special characters.
        std::u32string pinyin_utf32 = QString::fromStdString(pinyin)
                                          .normalized(
                                              QString::NormalizationForm_C)
                                          .toStdU32String();
        for (const auto &character : pinyin_utf32) {
            std::string character_utf8 = QString::fromStdU32String(
                                             std::u32string{character})
                                             .toStdString();
            if (specialCharacters.find(character_utf8)
                != specialCharacters.end()) {
                pinyinCopy += " " + character_utf8 + " ";
            } else {
                pinyinCopy += character_utf8;
            }
        }
        Utils::split(pinyinCopy, ' ', syllables);
    } else {
        segmentPinyin(QString::fromStdString(pinyin), syllables);
    }

    std::vector<std::string> ipa_syllables;

    // Pre-compute list of tones corresponding to each syllable
    // This is used for tone sandhi reasons (3->3 sandhi, x->5 sandhi, etc.)
    std::vector<std::pair<int, signed long>> syllable_tones;
    for (const auto &syllable : syllables) {
        auto tone_location = syllable.find_first_of("12345");
        if (tone_location == std::string::npos || syllable.size() == 1) {
            syllable_tones.push_back({-1, -1});
        } else {
            int tone = std::stoi(syllable.substr(tone_location, 1));
            syllable_tones.push_back({tone, tone_location});
        }
    }

    for (size_t i = 0; i < syllables.size(); i++) {
        const auto &syllable = syllables[i];
        std::string ipa_glottal;
        std::string ipa_initial;
        std::string ipa_final;
        std::string ipa_tone;

        // Most numbers, single characters, etc. are not Pinyin.
        // Filter those out.
        if (syllable.size() == 1) {
            ipa_syllables.emplace_back(syllable);
            continue;
        }

        // Filter out special characters separately, as those may
        // sometimes be more than one byte long.
        if (specialCharacters.find(syllable) != specialCharacters.end()) {
            ipa_syllables.emplace_back(syllable);
            continue;
        }

        // Get syllable without tone
        auto tone_location = syllable_tones[i].second;
        if (tone_location < 0) {
            ipa_syllables.emplace_back(syllable);
            continue;
        }
        std::string syllable_without_tone
            = syllable.substr(0, static_cast<unsigned long>(tone_location));

        // Figure out whether this syllable needs a glottal stop
        if (mandarinIPAGlottal.find(syllable_without_tone)
            != mandarinIPAGlottal.end()) {
            ipa_glottal = "ˀ";
        }

        // Mark close front rounded vowel with v instead of "u" or "u:"
        syllable_without_tone = std::regex_replace(syllable_without_tone,
                                                   std::regex{"u\\:"},
                                                   "v");
        syllable_without_tone = std::regex_replace(syllable_without_tone,
                                                   std::regex{"([jqx])u"},
                                                   "$1v");

        // Convert initial and final
        std::tie(ipa_initial, ipa_final) = convertIPAMandarinSyllable(
            syllable_without_tone);

        // Convert tones
        int tone = syllable_tones[i].first;
        int next_tone = (i == syllables.size() - 1)
                            ? -1
                            : syllable_tones[i + 1].first;
        int prev_tone = (i == 0) ? -1 : syllable_tones[i - 1].first;

        switch (tone) {
        case 5: {
            // When neutral tone, replace some initials with voiceless versions
            if (mandarinIPAVoicelessInitials.find(ipa_initial)
                != mandarinIPAVoicelessInitials.end()) {
                ipa_initial = mandarinIPAVoicelessInitials.at(ipa_initial);
            }
            if (ipa_final == "ɤ") {
                ipa_final = "ə";
            }
            ipa_tone = prev_tone == -1
                           ? ""
                           : mandarinIPANeutralTone
                               [static_cast<unsigned long>(prev_tone) - 1];
            break;
        }
        case 3: {
            if (i == syllables.size() - 1) {
#if defined(Q_OS_MAC)
                ipa_tone = (i == 0) ? "˨ ˩ ˦" : "˨ ˩ ˦ ꜕ ꜖ ( ꜓ )";
#elif defined(Q_OS_WIN)
                ipa_tone = (i == 0) ? "²¹⁴" : "²¹⁴⁻²¹⁽⁴⁾";
#else
                ipa_tone = (i == 0) ? "˨˩˦" : "˨˩˦꜕꜖(꜓)";
#endif
            } else {
                // If next syllable doesn't have tone, default to no tone sandhi
                // (which is also what happens when the following tone is tone #5
                ipa_tone = next_tone == -1
                               ? mandarinIPAThirdTone[4]
                               : mandarinIPAThirdTone
                                   [static_cast<unsigned long>(next_tone) - 1];
            }
            break;
        }
        case 4: {
            if (next_tone == 4) {
#if defined(Q_OS_MAC)
                ipa_tone = "˥ ˩ ꜒ ꜔";
#elif defined(Q_OS_WIN)
                ipa_tone = "⁵¹⁻⁵³";
#else
                ipa_tone = "˥˩꜒꜔";
#endif
            } else {
                ipa_tone
                    = mandarinIPATones[static_cast<unsigned long>(tone) - 1];
            }
            break;
        }
        default: {
            ipa_tone = mandarinIPATones[static_cast<unsigned long>(tone) - 1];
            break;
        }
        }

        ipa_syllables.push_back(
            ipa_glottal + ipa_initial
            + ipa_final
#if defined(Q_OS_MAC)
            // Only macOS needs this space to fix weird kerning
            + " "
#endif
            + ipa_tone);
    }

    std::ostringstream ipa;
    std::string double_space = "  ";
    for (const auto &ipa_syllable : ipa_syllables) {
        ipa << ipa_syllable << double_space;
    }

    // Remove trailing space
    std::string result = ipa.str();
    result.resize(result.size() - double_space.length());

    return result;
}

bool segmentPinyin(const QString &string,
                   std::vector<std::string> &out,
                   bool removeSpecialCharacters,
                   bool removeGlobCharacters)
{
    std::vector<std::string> syllables;

    std::unordered_set<std::string> initials = {"b", "p", "m",  "f",  "d",  "t",
                                                "n", "l", "g",  "k",  "h",  "j",
                                                "q", "x", "zh", "ch", "sh", "r",
                                                "z", "c", "s",  "y"};
    std::unordered_set<std::string> finals
        = {"a",   "e",   "ai",   "ei",   "ao",   "ou", "an", "ang", "en",
           "ang", "eng", "ong",  "er",   "i",    "ia", "ie", "iao", "iu",
           "ian", "in",  "iang", "ing",  "iong", "u",  "ua", "uo",  "uai",
           "ui",  "uan", "un",   "uang", "u",    "u:", "ue", "u:e", "o"};

    bool valid_pinyin = true;
    // Keep track of indices for current segmented word; [start_index, end_index)
    // Greedily try to expand end_index by checking for valid sequences
    // of characters
    int start_idx = 0;
    int end_idx = 0;
    bool initial_found = false;

    while (end_idx < string.length()) {
        bool next_iteration = false;
        // Ignore separation characters; these are special.
        QString currentString = string.mid(end_idx, 1).toLower();
        bool isSpecialCharacter = (specialCharacters.find(
                                       currentString.toStdString())
                                   != specialCharacters.end());
        bool isGlobCharacter = currentString.trimmed() == "*"
                               || currentString.trimmed() == "?";
        if (currentString == " " || currentString == "'" || isSpecialCharacter
            || isGlobCharacter) {
            if (initial_found) { // Add any incomplete word to the vector
                QString previous_initial
                    = string.mid(start_idx, end_idx - start_idx).toLower();
                syllables.push_back(previous_initial.toStdString());
                if (finals.find(previous_initial.toStdString())
                    == finals.end()) {
                    valid_pinyin = false;
                }
                start_idx = end_idx;
                initial_found = false;
            }
            if (!removeGlobCharacters && isGlobCharacter) {
                // Add anything before the current glob character
                if (end_idx >= 1 && (end_idx - start_idx >= 1)) {
                    QString previous_initial
                        = string.mid(start_idx, end_idx - start_idx).toLower();
                    syllables.push_back(previous_initial.toStdString());
                    if (finals.find(previous_initial.toStdString())
                        == finals.end()) {
                        valid_pinyin = false;
                    }
                    initial_found = false;
                }

                // Since whitespace matters for glob characters, consume the
                // next or previous whitespace if it exists (and was not
                // already consumed by another glob character).
                int new_end_index = end_idx;
                int length = 1;
                if ((end_idx >= 1) && (string.at(end_idx - 1) == ' ')
                    && syllables.back().back() != ' ') {
                    // Add preceding whitespace to this word
                    new_end_index--;
                    length++;
                }
                if ((string.length() > end_idx + 1)
                    && (string.at(end_idx + 1) == ' ')) {
                    // Add succeeding whitespace to this word
                    length++;
                    end_idx++;
                }
                QString glob = string.mid(new_end_index, length).toLower();
                syllables.push_back(glob.toStdString());

                start_idx = end_idx;
            } else if (!removeSpecialCharacters && isSpecialCharacter) {
                syllables.push_back(currentString.toStdString());
            }

            start_idx++;
            end_idx++;
            continue;
        }

        // First, check for initials
        // If initial is valid, then extend the end_index for length of initial
        // cluster of consonants.
        for (int initial_len = 2; initial_len > 0; initial_len--) {
            currentString = string.mid(end_idx, initial_len).toLower();
            auto searchResult = initials.find(currentString.toStdString());
            if (searchResult != initials.end()
                && currentString.length() == initial_len) {
                if (initial_found) {
                    // Two initials in a row are invalid Pinyin.
                    valid_pinyin = false;
                }
                end_idx += initial_len;
                next_iteration = true;
                initial_found = true;
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
        for (int final_len = 4; final_len > 0; final_len--) {
            currentString = string.mid(end_idx, final_len).toLower();
            auto searchResult = finals.find(currentString.toStdString());
            if (searchResult != finals.end()
                && currentString.length() == final_len) {
                end_idx += final_len;

                // Append erhua "r" and tone digit to the syllable
                if (end_idx < string.length() && string.at(end_idx) == 'r') {
                    end_idx++;
                }
                if (end_idx < string.length() && string.at(end_idx).isDigit()) {
                    if (string.at(end_idx).digitValue() < 1
                        || string.at(end_idx).digitValue() > 5) {
                        valid_pinyin = false;
                    }
                    end_idx++;
                }

                QString syllable
                    = string.mid(start_idx, end_idx - start_idx).toLower();
                syllables.push_back(syllable.toStdString());
                start_idx = end_idx;
                next_iteration = true;
                initial_found = false;
                break;
            }
        }

        if (next_iteration) {
            continue;
        }

        end_idx++;
    }

    // Then add whatever's left in the search term, minus whitespace.
    QString lastSyllable
        = string.mid(start_idx, end_idx - start_idx).simplified().toLower();
    if (!lastSyllable.isEmpty() && lastSyllable != "'") {
        syllables.push_back(lastSyllable.toStdString());
        if (finals.find(lastSyllable.toStdString()) == finals.end()) {
            valid_pinyin = false;
        }
    }

    out = syllables;
    return valid_pinyin;
}

} // namespace MandarinUtils
