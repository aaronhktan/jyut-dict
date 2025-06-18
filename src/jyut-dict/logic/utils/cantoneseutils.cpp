#include "cantoneseutils.h"

#include "logic/utils/utils.h"

#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace {
std::unordered_set<std::string> specialCharacters
    = {".",  "。", ",",  "，", "！", "？", "%",  "－", "…",  "⋯",
       ".",  "·",  "\"", "“",  "”",  "$",  "｜", "：", "１", "２",
       "３", "４", "５", "６", "７", "８", "９", "０"};

std::unordered_set<std::string> regexCharacters = {"!", "(", ")", "|"};

std::unordered_set<std::string> initials = {"b",  "p", "m",  "f",  "d",
                                            "t",  "n", "l",  "g",  "k",
                                            "ng", "h", "gw", "kw", "w",
                                            "z",  "c", "s",  "j",  "m"};
std::unordered_set<std::string> finals
    = {"a",   "aa",   "aai", "aau", "aam", "aan", "aang", "aap", "aat", "aak",
       "ai",  "au",   "am",  "an",  "ang", "ap",  "at",   "ak",  "e",   "ei",
       "eu",  "em",   "en",  "eng", "ep",  "ek",  "i",    "iu",  "im",  "in",
       "ing", "ip",   "it",  "ik",  "o",   "oi",  "ou",   "on",  "ong", "ot",
       "ok",  "u",    "ui",  "un",  "ung", "ut",  "uk",   "oe",  "oet", "eoi",
       "eon", "oeng", "eot", "oek", "yu",  "yun", "yut",  "m",   "ng"};

std::unordered_map<std::string, std::vector<std::string>>
    jyutpingToYaleSpecialSyllables = {
        {"m", {"m̄", "ḿ", "m", "m̀h", "ḿh", "mh"}},
        {"ng", {"n̄g", "ńg", "ng", "ǹgh", "ńgh", "ngh"}},
};

std::unordered_map<std::string, std::string> jyutpingToYaleSpecialFinals = {
    {"aa", "a"},
    {"oe", "eu"},
    {"oeng", "eung"},
    {"oek", "euk"},
    {"eoi", "eui"},
    {"eon", "eun"},
    {"eot", "eut"},
};

std::unordered_map<std::string, std::vector<std::string>> yaleToneReplacements = {
    {"a", {"ā", "á", "a", "à", "á", "a"}},
    {"e", {"ē", "é", "e", "è", "é", "e"}},
    {"i", {"ī", "í", "i", "ì", "í", "i"}},
    {"o", {"ō", "ó", "o", "ò", "ó", "o"}},
    {"u", {"ū", "ú", "u", "ù", "ú", "u"}},
};

std::vector<std::pair<std::string, std::string>> cantoneseIPASpecialSyllables
    = {{"a", "@"},
       {"yu", "y"},
       {"@@", "a"},
       {"uk", "^k"},
       {"ik", "|k"},
       {"ou", "~u"},
       {"eoi", "eoy"},
       {"ung", "^ng"},
       {"ing", "|ng"},
       {"ei", ">i"}};

std::unordered_map<std::string, std::string> cantoneseIPAInitials
    = {{"b", "p"},
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
       {"c", "t͡sʰ"}};

std::unordered_map<std::string, std::string> cantoneseIPANuclei = {{"a", "äː"},
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
                                                                   {"y", "yː"}};

std::unordered_map<std::string, std::string> cantoneseIPACodas = {{"i", "i̯"},
                                                                  {"u", "u̯"},
                                                                  {"y", "y̯"},
                                                                  {"ng", "ŋ"},
                                                                  {"p", "p̚"},
                                                                  {"t", "t̚"},
                                                                  {"k", "k̚"}};

// The original Wiktionary module uses breves to indicate a special letter (e.g.
// ă), but the base C++ regex engine can't match against chars outside of the
// basic set. As a workaround, I'm just replacing them with other symbols.
#if defined(Q_OS_MAC)
// Added a six-per-em space (U+2006) between adjacent tone markers, because Qt's
// kerning squishes them too close together
std::vector<std::string> jyutpingToIPATones
    = {"˥", "˧ ˥", "˧", "˨ ˩", "˩ ˧", "˨", "˥", "˧", "˨"};
#else
std::vector<std::string> jyutpingToIPATones
    = {"˥", "˧˥", "˧", "˨˩", "˩˧", "˨", "˥", "˧", "˨"};
#endif

bool unfoldJyutpingRegex(const QString &string, std::vector<QString> &out)
{
    std::vector<QString> stringPossibilities;
    // Invariant: there must be only one set of parentheses and one exclamation
    if (string.count("(") > 1 || string.count(")") > 1
        || string.count("!") > 1) {
        return false;
    }

    // If there is a parenthesis, then all of the options must be checked
    if (string.indexOf("(") != -1 && string.indexOf(")") != -1) {
        int startIdx = string.indexOf("(") + 1;
        int endIdx = string.indexOf(")");

        int orIdx = string.indexOf("|");
        while (orIdx != -1) {
            stringPossibilities.push_back(
                string.mid(0, string.indexOf("("))
                + string.mid(startIdx, orIdx - startIdx)
                + string.mid(string.indexOf(")") + 1));
            startIdx = orIdx + 1;
            orIdx = string.indexOf("|", startIdx);
        }
        stringPossibilities.push_back(string.mid(0, string.indexOf("("))
                                      + string.mid(startIdx, endIdx - startIdx)
                                      + string.mid(string.indexOf(")") + 1));
    } else {
        stringPossibilities.push_back(string);
    }

    // If there is a "!", then the initial with and the initial without
    // that optional character should be considered
    for (const auto &s : stringPossibilities) {
        int regexIdx = s.indexOf("!");
        if (regexIdx == -1) {
            out.push_back(s);
        } else if (regexIdx == 0) {
            QString tmp{s};
            out.push_back(tmp.replace(0, 1, ""));
        } else {
            QString tmp{s};
            out.push_back(tmp.replace(regexIdx - 1, 2, ""));
            tmp = s;
            out.push_back(tmp.replace(regexIdx, 1, ""));
        }
    }

    return true;
}

std::string convertYaleInitial(const std::string &syllable)
{
    std::string yale_syllable{syllable};
    yale_syllable = std::regex_replace(yale_syllable, std::regex{"jy?"}, "y");
    yale_syllable = std::regex_replace(yale_syllable, std::regex{"z"}, "j");
    yale_syllable = std::regex_replace(yale_syllable, std::regex{"c"}, "ch");
    return yale_syllable;
}

std::string convertYaleFinal(const std::string &syllable)
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
    auto replacement_vowel_search = yaleToneReplacements.find(first_vowel);
    std::string replacement_vowel = (replacement_vowel_search
                                     == yaleToneReplacements.end())
                                        ? first_vowel
                                        : replacement_vowel_search->second.at(
                                            static_cast<size_t>(tone - 1));
    yale_syllable.erase(replacement_location, first_vowel.length());
    yale_syllable.insert(replacement_location, replacement_vowel);

    // Remove the tone number, as Yale doesn't use those
    yale_syllable.erase(yale_syllable.cend() - 1);

    return yale_syllable;
}

} // namespace

namespace CantoneseUtils {
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
        std::u32string jyutping_utf32 = QString::fromStdString(jyutping)
                                            .normalized(
                                                QString::NormalizationForm_C)
                                            .toStdU32String();
        for (const auto &character : jyutping_utf32) {
            std::string character_utf8 = QString::fromStdU32String(
                                             std::u32string{character})
                                             .toStdString();
            if (specialCharacters.find(character_utf8)
                != specialCharacters.end()) {
                jyutpingCopy += " " + character_utf8 + " ";
            } else {
                jyutpingCopy += character_utf8;
            }
        }
        Utils::split(jyutpingCopy, ' ', syllables);
    } else {
        bool valid_jyutping
            = segmentJyutping(QString::fromStdString(jyutping),
                              syllables,
                              /* removeSpecialCharacters */ false,
                              /* removeGlobCharacters */ false);
        if (!valid_jyutping) {
            return "x";
        }
    }

    std::vector<std::string> yale_syllables;

    for (const auto &syllable : syllables) {
        // Most numbers, single characters, etc. are not Jyutping.
        // Filter those out.
        if (syllable.size() == 1) {
            yale_syllables.emplace_back(syllable);
            continue;
        }

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
        std::string syllable_without_tone
            = syllable.substr(0, syllable.length() - 1);
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

std::string convertIPACantoneseSyllable(const std::string &syllable)
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
        if (cantoneseIPAInitials.find(match[1]) != cantoneseIPAInitials.end()) {
            initial = cantoneseIPAInitials.at(match[1]);
        } else {
            initial = match[1];
        }
    }

    // Get equivalent to matched nucleus
    if (match[2].length()) {
        if (cantoneseIPANuclei.find(match[2]) != cantoneseIPANuclei.end()) {
            nucleus = cantoneseIPANuclei.at(match[2]);
        } else {
            nucleus = match[2];
        }
    }

    // Get equivalent to matched final
    if (match[3].length()) {
        if (cantoneseIPACodas.find(match[3]) != cantoneseIPACodas.end()) {
            coda = cantoneseIPACodas.at(match[3]);
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
        std::u32string jyutping_utf32 = QString::fromStdString(jyutping)
                                            .normalized(
                                                QString::NormalizationForm_C)
                                            .toStdU32String();
        for (const auto &character : jyutping_utf32) {
            std::string character_utf8 = QString::fromStdU32String(
                                             std::u32string{character})
                                             .toStdString();
            if (specialCharacters.find(character_utf8)
                != specialCharacters.end()) {
                jyutpingCopy += " " + character_utf8 + " ";
            } else {
                jyutpingCopy += character_utf8;
            }
        }
        Utils::split(jyutpingCopy, ' ', syllables);
    } else {
        bool validJyutping = segmentJyutping(QString::fromStdString(jyutping),
                                             syllables,
                                             /* removeSpecialCharacters */ false,
                                             /* removeGlobCharacters */ false);
        if (!validJyutping) {
            return "x";
        }
    }

    std::vector<std::string> ipa_syllables;

    for (const auto &syllable : syllables) {
        // Most numbers, single characters, etc. are not Jyutping.
        // Filter those out.
        if (syllable.size() == 1) {
            ipa_syllables.emplace_back(syllable);
            continue;
        }

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
        for (const auto &pair : cantoneseIPASpecialSyllables) {
            ipa_syllable = std::regex_replace(ipa_syllable,
                                              std::regex{pair.first},
                                              pair.second);
        }

        ipa_syllables.emplace_back(convertIPACantoneseSyllable(ipa_syllable));
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

bool jyutpingAutocorrect(const QString &in,
                         QString &out,
                         bool unsafeSubstitutions)
{
    out = in;

    // This is for some romanizations like "shui" for 水
    // And needs to happen before the "sh" -> "s" conversion
    out.replace("hui", "heoi");

    // The initial + nucleus "cu-" never appears in Jyutping, so the user
    // probably intended to make the IPA [kʰɐ] sound
    // Surround the k with capturing group to prevent replacement with (g|k)
    // if sound changes are enabled
    out.replace("cu", "(k)u");

    // "x" never appears in Jyutping, the user might be more familiar
    // with Pinyin and assume that it's an "s" sound
    out.replace("x", "s");

    out.replace("ch", "c").replace("sh", "s").replace("zh", "z");

    // Change "eung", "erng", "eong" -> "oeng"
    out.replace("eung", "oeng").replace("erng", "oeng");

    out.replace("eui", "eoi");
    out.replace("euk", "oek");
    out.replace("eun", "(eo|yu)n");
    out.replace("eut", "(eo|yu)t");
    out.replace("eu", "(e|y)u");
    out.replace("ern", "eon");

    // Change "-oen" -> "-eon"
    int idx = out.indexOf("oen");
    while (idx != -1) {
        if (QStringView{out.constBegin() + idx, 4} == QString{"oeng"}) {
            idx = out.indexOf("oen", idx + 1);
            continue;
        }
        out.replace(idx, 3, "eon");
        idx = out.indexOf("oen", idx + 1);
    }
    out.replace("oei", "eoi");
    out.replace("oet", "eot");

    out.replace("eong ", "oeng ");
    out.replace("eong'", "oeng'");
    if (out.endsWith("eong")) {
        idx = out.lastIndexOf(("eong"));
        out.replace(idx, 4, "oeng");
    }
    if (unsafeSubstitutions) {
        out.replace("eong", "oeng"); // unsafe because of zeon6 gun2
    }
    out.replace("eok", "oek");

    out.replace("ar", "aa");      // like in "char siu"
    out.replace("ee", "i");       // like in "lai see"
    out.replace("ay", "ei");      // like in "gong hay fat choy"
    out.replace("oy", "oi");      // like in "choy sum"
    out.replace("oo", "(y!u)");   // like in "soot goh"
    out.replace("ong", "(o|u)ng");
    out.replace("young", "jung"); // like in "foo young"

    out.replace("yue", "jyu"); // like "yuet yue" (粵語)
    out.replace("ue", "(yu)"); // like "tsuen wan" (轉彎)
    out.replace("tsz", "zi");  // like "tsat tsz mui" (七姊妹)
    out.replace("ck", "k");    // like "back" (白)

    // The following changes may be unsafe because it is ambiguous whether
    // they are final + initial or a "misspelling" of a final
    // However, it is unambiguous if there is a separator at the end of the
    // syllable, or it is the end of the string

    // Check if the user intends to write an [-ɛː j-] or [-ei̯] cluster
    {
        out.replace("ey ", "ei ").replace("ey'", "ei'");
        if (out.endsWith("ey")) {
            out.replace(out.length() - 2, 2, "ei");
        }

        // Initials for which <initial> + "-ei" exist in Jyutping
        std::unordered_set<QChar> closeFrontVowelCluster
            = {'p', 'f', 'd', 'n', 'l', 'h', 'w'};
        // Initials for which <initial> + "-e j-" exist in Jyutping
        std::unordered_set<QChar> openMidFrontVowelCluster = {'c', 'j', 'y'};
        // Initials for which both exist in Jyutping
        std::unordered_set<QChar> ambiguousVowelCluster
            = {'b', 'm', 'g', 'k', 'z', 's'};

        int replacementIdx = out.indexOf("ey");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 2, "ei");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeFrontVowelCluster.find(out.at(initialIdx))
                    != closeFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 2, "ei");
                } else if (openMidFrontVowelCluster.find(out.at(initialIdx))
                           != openMidFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 2, "e j");
                } else if (ambiguousVowelCluster.find(out.at(initialIdx))
                           != ambiguousVowelCluster.end()) {
                    // The [j-] cluster can only occur if what follows is not an initial
                    bool initialFound = false;
                    for (int initial_len = 2; initial_len > 0; initial_len--) {
                        QString s{out.constBegin() + replacementIdx + 2,
                                  initial_len};
                        if (initials.find(s.toStdString()) != initials.end()
                            || s.toStdString() == "y") {
                            initialFound = true;
                        }
                    }
                    if (initialFound) {
                        out.replace(replacementIdx, 2, "ei");
                    } else {
                        out.replace(replacementIdx, 2, "e j");
                    }
                }
                break;
            }
            }
            replacementIdx = out.indexOf("ey", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-ɔː h-] or [-ou̯] cluster
    {
        out.replace("oh ", "ou ").replace("oh'", "ou'");
        if (out.endsWith("oh")) {
            out.replace(out.length() - 2, 2, "ou");
        }

        // Initials for which <initial> + "-ou" exist in Jyutping
        std::unordered_set<QChar> closeBackVowelCluster = {'n', 'j'};
        // Initials for which both "-ou" and "-o h-" exist in Jyutping
        std::unordered_set<QChar> ambiguousVowelCluster
            = {'b', 'p', 'm', 'f', 'd', 't', 'l', 'g', 'h', 'w', 'z', 'c', 's'};

        int replacementIdx = out.indexOf("oh");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 2, "ou");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeBackVowelCluster.find(out.at(initialIdx))
                    != closeBackVowelCluster.end()) {
                    out.replace(replacementIdx, 2, "ou");
                } else if (ambiguousVowelCluster.find(out.at(initialIdx))
                           != ambiguousVowelCluster.end()) {
                    // The [h-] cluster can only occur if what follows is not an initial
                    bool initialFound = false;
                    for (int initial_len = 2; initial_len > 0; initial_len--) {
                        QString s{out.constBegin() + replacementIdx + 2,
                                  initial_len};
                        if (initials.find(s.toStdString()) != initials.end()
                            || s.toStdString() == "y") {
                            initialFound = true;
                        }
                    }
                    if (initialFound) {
                        out.replace(replacementIdx, 2, "ou");
                    } else {
                        out.replace(replacementIdx, 2, "o h");
                    }
                }
                break;
            }
            }
            replacementIdx = out.indexOf("oh", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-ɔː w-] or [-auː] cluster
    {
        out.replace("ow ", "au ").replace("ow'", "au'");
        if (out.endsWith("ow")) {
            out.replace(out.length() - 2, 2, "au");
        }

        // Initials for which <initial> + "-(a)au" exist in Jyutping
        std::unordered_set<QChar> closeBackVowelCluster = {'b', 'm', 'k', 's'};
        // Initials for which both exist in Jyutping
        std::unordered_set<QChar> ambiguousVowelCluster
            = {'p', 'm', 'f', 'd', 't', 'n', 'l', 'g', 'h', 'z', 'c', 's'};
        int replacementIdx = out.indexOf("ow");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 2, "au");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeBackVowelCluster.find(out.at(initialIdx))
                    != closeBackVowelCluster.end()) {
                    out.replace(replacementIdx, 2, "au");
                } else if (ambiguousVowelCluster.find(out.at(initialIdx))
                           != ambiguousVowelCluster.end()) {
                    // The [w-] cluster can only occur if what follows is not an initial
                    bool initialFound = false;
                    for (int initial_len = 2; initial_len > 0; initial_len--) {
                        QString s{out.constBegin() + replacementIdx + 2,
                                  initial_len};
                        if (initials.find(s.toStdString()) != initials.end()
                            || s.toStdString() == "y") {
                            initialFound = true;
                        }
                    }
                    if (initialFound) {
                        out.replace(replacementIdx, 2, "au");
                    } else {
                        out.replace(replacementIdx, 2, "o w");
                    }
                }
                break;
            }
            }
            replacementIdx = out.indexOf("ow", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [ɐm] or [-uː  m-] cluster
    {
        out.replace("um ", "am ").replace("um'", "am'");
        if (out.endsWith("um")) {
            out.replace(out.length() - 2, 2, "am");
        }

        // Initials for which <initial> + "am" exist in Jyutping
        std::unordered_set<QChar> openMidCentralVowelCluster
            = {'b', 'p', 'm', 'd', 't', 'n', 'l', 'k', 'h', 'z', 'c', 's', 'j'};
        // Initials for which <initial> + "-u m-" exist in Jyutping
        std::unordered_set<QChar> closeBackVowelCluster
            = {'f', 'w', 'a', 'e', 'i', 'o'};

        int replacementIdx = out.indexOf("um");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 2, "am");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (openMidCentralVowelCluster.find(out.at(initialIdx))
                    != openMidCentralVowelCluster.end()) {
                    out.replace(replacementIdx, 2, "am");
                } else if (closeBackVowelCluster.find(out.at(initialIdx))
                           != closeBackVowelCluster.end()) {
                    (void) 0; // Do nothing
                } else if (out.at(initialIdx) == "g") {
                    // The [m-] cluster can only occur if what follows is not an initial
                    bool initialFound = false;
                    for (int initial_len = 2; initial_len > 0; initial_len--) {
                        QString s{out.constBegin() + replacementIdx + 2,
                                  initial_len};
                        if (initials.find(s.toStdString()) != initials.end()
                            || s.toStdString() == "y") {
                            initialFound = true;
                        }
                    }
                    if (initialFound) {
                        out.replace(replacementIdx, 2, "am");
                    } else {
                        out.replace(replacementIdx, 2, "u m");
                    }
                }
                break;
            }
            }
            replacementIdx = out.indexOf("um", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-yː j-] or [-ɐm] cluster
    {
        // Initials for which <initial> + "-yu m-" exist in Jyutping
        std::unordered_set<QChar> closeFrontVowelCluster = {'z', 'c', 's', 'j'};
        int replacementIdx = out.indexOf("yum");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 3, "jam");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeFrontVowelCluster.find(out.at(initialIdx))
                    != closeFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 3, "yu m");
                } else {
                    out.replace(replacementIdx, 3, "jam");
                }
                break;
            }
            }
            replacementIdx = out.indexOf("yum", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-yː p-] or [-ɐp] cluster
    {
        // Initials for which <initial> + "-yu p-" exist in Jyutping
        std::unordered_set<QChar> closeFrontVowelCluster = {'z', 's', 'j'};

        int replacementIdx = out.indexOf("yup");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 3, "jap");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeFrontVowelCluster.find(out.at(initialIdx))
                    != closeFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 3, "yu p");
                } else {
                    out.replace(replacementIdx, 3, "jap");
                }
                break;
            }
            }
            replacementIdx = out.indexOf("yup", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-yː k-] or [jʊk] cluster
    {
        // Initials for which <initial> + "-yu k-" exist in Jyutping
        std::unordered_set<QChar> closeFrontVowelCluster = {'z', 's', 'c', 'j'};

        int replacementIdx = out.indexOf("yuk");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 3, "juk");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeFrontVowelCluster.find(out.at(initialIdx))
                    != closeFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 3, "yu k");
                } else {
                    out.replace(replacementIdx, 3, "juk");
                }
                break;
            }
            }
            replacementIdx = out.indexOf("yuk", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-yn g-] or [jʊŋ] cluster
    {
        // Initials for which <initial> + "-yun g-" exist in Jyutping
        std::unordered_set<QChar> closeFrontVowelCluster = {'z', 's', 'c', 'j'};

        int replacementIdx = out.indexOf("yung");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 4, "jung");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeFrontVowelCluster.find(out.at(initialIdx))
                    != closeFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 4, "(yu)n g");
                } else {
                    out.replace(replacementIdx, 4, "jung");
                }
                break;
            }
            }
            replacementIdx = out.indexOf("yung", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-yn] or [jɐn], [jyn], [yn] cluster
    {
        // Initials for which <initial> + "-yu n-" exist in Jyutping
        std::unordered_set<QChar> closeFrontVowelCluster = {'z', 's', 'c', 'j'};

        int replacementIdx = out.indexOf("yun");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 3, "j(a|yu)n");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeFrontVowelCluster.find(out.at(initialIdx))
                    != closeFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 3, "yun");
                } else {
                    out.replace(replacementIdx, 3, "(ja|jyu|yu)n");
                }
                break;
            }
            }
            replacementIdx = out.indexOf("yun", replacementIdx + 1);
        }
    }

    // Check if the user intends to write an [-yt] or [jɐt], [jyt], [yt] cluster
    {
        // Initials for which <initial> + "-yu t-" exist in Jyutping
        std::unordered_set<QChar> closeFrontVowelCluster = {'z', 's', 'c', 'j'};

        int replacementIdx = out.indexOf("yut");
        while (replacementIdx != -1) {
            switch (replacementIdx) {
            case 0: {
                out.replace(replacementIdx, 3, "j(a|yu)t");
                break;
            }
            default: {
                int initialIdx = replacementIdx - 1;
                if (replacementIdx >= 2 && out.at(initialIdx) == ")") {
                    initialIdx = replacementIdx - 2;
                }

                if (closeFrontVowelCluster.find(out.at(initialIdx))
                    != closeFrontVowelCluster.end()) {
                    out.replace(replacementIdx, 3, "(yu)t");
                } else {
                    out.replace(replacementIdx, 3, "(ja|jyu|yu)t");
                }
                break;
            }
            }
            replacementIdx = out.indexOf("yut", replacementIdx + 1);
        }
    }

    // Unsafe because it is ambiguous whether these are final + initial
    // or a "misspelling" of an initial
    // But unambiguous if they are at the start of a syllable
    if (out.startsWith("ts")) {
        out.replace(0, 2, "c");
    }
    out.replace(" ts", " c");
    if (out.startsWith("kwu")) {
        out.replace(0, 3, "(g|k)w!u");
    }
    out.replace(" kwu", " (g|k)w!u");
    if (unsafeSubstitutions) {
        out.replace("ts", "c");         // unsafe because of kat1 sau3
        out.replace("kwu", "(g|k)w!u"); // unsafe because of baak6 wun2
    }

    // Change any "y" that is not followed by a "u" to "j"
    // This needs to happen before the final replacements
    idx = out.indexOf("y");
    while (idx != -1) {
        if (QStringView{out.constBegin() + idx, 2} == QString{"yu"}
            || QStringView{out.constBegin() + idx, 3} == QString{"y!u"}
            || QStringView{out.constBegin() + idx, 3} == QString{"y)u"}) {
            idx = out.indexOf("y", idx + 1);
            continue;
        }
        out.replace(idx, 1, 'j');
        idx = out.indexOf("y", idx + 1);
    }

    out.replace("ui", "(eo|u)i");
    out.replace("un", "(y!u|a|eo)n");
    out.replace("ut", "(a|u)t");

    return 0;
}

bool jyutpingSoundChanges(std::vector<std::string> &inOut)
{
    for (auto &syllable : inOut) {
        // Whole-syllable sound changes
        if (syllable == "ng"
            || (syllable.length() == 3
                && std::string_view{syllable.begin(), syllable.end() - 1}
                       .ends_with("ng")
                && (std::isdigit(syllable.back()) || syllable.back() == '?'))) {
            syllable.replace(syllable.rfind("ng"), 2, "(ng|m)");
            continue;
        } else if (syllable == "m"
                   || (syllable.length() == 2 && syllable.front() == 'm'
                       && (std::isdigit(syllable.back())
                           || syllable.back() == '?'))) {
            syllable.replace(syllable.rfind("m"), 1, "(ng|m)");
            continue;
        }

        // Initial sound changes
        // "Lazy" pronunciations
        if (syllable.length() >= 3 && syllable.starts_with("ng")
            && !std::isdigit(syllable[2]) && syllable[2] != '?') {
            // loss of [ŋ] initial, replacement with null initial
            syllable.replace(0, 2, "(ng)!");
        } else if (syllable[0] == 'a' || syllable[0] == 'o'
                   || syllable[0] == 'u') {
            // merging of null initial with initial [ŋ] before [a, ɐ, ɔ, o]
            syllable.insert(0, "(ng)!");
        } else if (syllable[0] == 'n' || syllable[0] == 'l') {
            // merge of [n] and [l] initials
            syllable.replace(0, 1, "(n|l)");
        } else if (syllable.starts_with("go") || syllable.starts_with("ko")
                   || syllable.starts_with("g(o")
                   || syllable.starts_with("k(o")) {
            // merging of [k]/[kʷ] and [kʰ]/[kʷʰ] initials before [ɔ]
            if (syllable[0] == 'g') {
                syllable.replace(0, 1, "gw!");
            } else if (syllable[0] == 'k') {
                syllable.replace(0, 1, "kw!");
            }
        }

        // Lack of distinction between aspirated and unaspirated initials
        if (syllable.starts_with("d") || syllable.starts_with("t")) {
            syllable.replace(0, 1, "(d|t)");
        } else if (syllable.starts_with("c") || syllable.starts_with("z")) {
            syllable.replace(0, 1, "(c|z)");
        } else if (syllable.starts_with("g") || syllable.starts_with("k")) {
            syllable.replace(0, 1, "(g|k)");
        }

        // Nucleus sound changes
        // Merge between [aː] and [ɐ]
        auto pos = syllable.find("a");
        while (pos != std::string::npos) {
            if (std::string_view{syllable.begin() + pos,
                                 syllable.begin() + pos + 2}
                == "aa") {
                syllable.insert(pos + 2, "!");
                pos = syllable.find("a", pos + 3);
            } else {
                syllable.insert(pos + 1, "a!");
                pos = syllable.find("a", pos + 2);
            }
        }

        // Final sound changes
        if ((syllable.ends_with("ang") || syllable.ends_with("a!ng")
             || syllable.ends_with("ong"))
            || (syllable.length() >= 4
                && std::string_view{syllable.begin(), syllable.end() - 1}
                       .ends_with("ang"))
            || (syllable.length() >= 5
                && std::string_view{syllable.begin(), syllable.end() - 1}
                       .ends_with("aa!ng"))
            || (syllable.length() >= 4
                && std::string_view{syllable.begin(), syllable.end() - 1}
                       .ends_with("ong"))) {
            // alveolarization of final [ŋ]
            syllable.replace(syllable.rfind("ng"), 2, "ng!");
        } else if ((syllable.ends_with("an") || syllable.ends_with("a!n")
                    || syllable.ends_with("on"))
                   || (syllable.length() >= 3
                       && std::string_view{syllable.begin(), syllable.end() - 1}
                              .ends_with("an"))
                   || (syllable.length() >= 4
                       && std::string_view{syllable.begin(), syllable.end() - 1}
                              .ends_with("a!n"))
                   || (syllable.length() >= 3
                       && std::string_view{syllable.begin(), syllable.end() - 1}
                              .ends_with("on"))) {
            // velarization of final [n]
            syllable.replace(syllable.rfind("n"), 1, "ng!");
        } else if ((syllable.length() > 1 && syllable.ends_with("t")
                    && !syllable.ends_with("it") && !syllable.ends_with("ut"))
                   || (syllable.length() >= 3
                       && (std::string_view{syllable.begin(), syllable.end() - 1}
                               .ends_with("t")
                           && !std::string_view{syllable.begin(),
                                                syllable.end() - 1}
                                   .ends_with("it")
                           && !std::string_view{syllable.begin(),
                                                syllable.end() - 1}
                                   .ends_with("ut")))) {
            // velarization of final [t]
            syllable.replace(syllable.rfind("t"), 1, "(k|t)");
        } else if ((syllable.length() > 1 && syllable.ends_with("k")
                    && !syllable.ends_with("ik") && !syllable.ends_with("uk"))
                   || (syllable.length() >= 3
                       && (std::string_view{syllable.begin(), syllable.end() - 1}
                               .ends_with("k")
                           && !std::string_view{syllable.begin(),
                                                syllable.end() - 1}
                                   .ends_with("ik")
                           && !std::string_view{syllable.begin(),
                                                syllable.end() - 1}
                                   .ends_with("uk")))) {
            // velarization of final [k]
            syllable.replace(syllable.rfind("k"), 1, "(k|t)");
        }
    }

    return 0;
}

bool segmentJyutping(const QString &string,
                     std::vector<std::string> &out,
                     bool removeSpecialCharacters,
                     bool removeGlobCharacters,
                     bool removeRegexCharacters)
{
    std::vector<std::string> syllables;

    bool valid_jyutping = true;
    // Keep track of indices for current segmented word; [start_index, end_index)
    // Greedily try to expand end_index by checking for valid sequences
    // of characters
    int start_idx = 0;
    int end_idx = 0;
    bool initial_found = false;

    // Invariant: There are no graphemes in the initial Jyutping string larger
    // than a single byte when encoded using UTF-16.
    QString processedString{string};
    if (removeSpecialCharacters) {
        for (const auto &i : specialCharacters) {
            int charIdx = processedString.indexOf(i.c_str());
            while (charIdx != -1) {
                processedString.replace(charIdx, 1, " ");
                charIdx = processedString.indexOf(i.c_str());
            }
        }
    }
    if (removeGlobCharacters) {
        for (const auto &i : std::unordered_set{"*", "?"}) {
            int charIdx = processedString.indexOf(i);
            while (charIdx != -1) {
                processedString.replace(charIdx, 1, " ");
                charIdx = processedString.indexOf(i);
            }
        }
    }
    if (removeRegexCharacters) {
        for (const auto &i : regexCharacters) {
            int charIdx = processedString.indexOf(i.c_str());
            while (charIdx != -1) {
                processedString.replace(charIdx, 1, " ");
                charIdx = processedString.indexOf(i.c_str());
            }
        }
    }

    // Because Qt uses UTF-16 for its internal representation of chars,
    // and we maintain the invariant that no graphemes are larger than a
    // single byte when encoded in UTF-16, it is safe to iterate QChar by
    // QChar.
    while (end_idx < processedString.length()) {
        bool component_found = false;

        QString currentString = processedString.mid(end_idx, 1).toLower();
        bool isSpecialCharacter = (specialCharacters.find(
                                       currentString.toStdString())
                                   != specialCharacters.end());
        bool isGlobCharacter = currentString.trimmed() == "*"
                               || currentString.trimmed() == "?";
        if (currentString == " " || currentString == "'" || isSpecialCharacter
            || isGlobCharacter) {
            // The presence of a space, apostrophe, special character, or
            // glob character indicates that a syllable is completed.
            if (initial_found) {
                // If a valid initial was previously found, then the Jyutping
                // sequence [initial] + [separator] is only valid Jyutping if the
                // initial is also a valid final (i.e. [final] + [separator] is
                // OK, but [initial] + [separator] is not).
                QString previous_initial = processedString
                                               .mid(start_idx,
                                                    end_idx - start_idx)
                                               .toLower();
                syllables.push_back(previous_initial.toStdString());
                if (finals.find(previous_initial.toStdString())
                    == finals.end()) {
                    valid_jyutping = false;
                }
                start_idx = end_idx;
                initial_found = false;
            }
            if (isGlobCharacter) {
                // Similar logic to the block above
                if (end_idx >= 1 && (end_idx - start_idx >= 1)) {
                    QString previous_initial = processedString
                                                   .mid(start_idx,
                                                        end_idx - start_idx)
                                                   .toLower();
                    syllables.push_back(previous_initial.toStdString());
                    if (finals.find(previous_initial.toStdString())
                        == finals.end()) {
                        valid_jyutping = false;
                    }
                    initial_found = false;
                }

                // Since whitespace matters for glob and regex, consume the
                // next or previous whitespace if it exists (and was not
                // already consumed by another glob character).
                int glob_start_idx = end_idx;
                int length = 1;
                if ((end_idx >= 1) && (processedString.at(end_idx - 1) == ' ')
                    && (!syllables.empty()) && syllables.back().back() != ' ') {
                    // Add preceding whitespace to this word
                    glob_start_idx--;
                    length++;
                }
                if ((processedString.length() > end_idx + 1)
                    && (processedString.at(end_idx + 1) == ' ')) {
                    // Add succeeding whitespace to this word
                    length++;
                    end_idx++;
                }
                QString glob
                    = processedString.mid(glob_start_idx, length).toLower();
                syllables.push_back(glob.toStdString());

                start_idx = end_idx;
            } else if (isSpecialCharacter) {
                syllables.push_back(currentString.toStdString());
            }
            start_idx++;
            end_idx++;
            continue;
        }

        // Digits are only valid after a final (which should be handled in the
        // final-checking code)
        // OR after an initial (that is also a final), like m or ng.
        // This block checks for the latter case.
        if (currentString.at(0).isDigit()) {
            if (initial_found) {
                QString previous_initial = processedString
                                               .mid(start_idx,
                                                    end_idx - start_idx)
                                               .toLower();

                bool isValidFinal = false;
                if (removeRegexCharacters) {
                    isValidFinal = finals.find(previous_initial.toStdString())
                                   != finals.end();
                } else {
                    // Regex characters need to be handled in a special way;
                    // essentially, we need to check every possibility. If at
                    // least one possibility is a valid final, then the Jyutping
                    // can be considered valid.
                    std::vector<QString> stringsToSearch;
                    QString stringToSearch = previous_initial;
                    unfoldJyutpingRegex(stringToSearch, stringsToSearch);

                    bool isValidFinal = false;
                    for (const auto &s : stringsToSearch) {
                        auto searchResult = finals.find(s.toStdString());
                        isValidFinal = isValidFinal
                                       || (searchResult != finals.end());
                    }
                }

                if (isValidFinal) {
                    end_idx++;
                    previous_initial = processedString
                                           .mid(start_idx, end_idx - start_idx)
                                           .toLower();
                    syllables.push_back(previous_initial.toStdString());
                    start_idx = end_idx;
                    initial_found = false;

                    if (currentString.at(0).digitValue() < 1
                        || currentString.at(0).digitValue() > 6) {
                        valid_jyutping = false;
                    }

                    continue;
                }
            } else {
                // If there was no initial found, then the Jyutping isn't valid
                valid_jyutping = false;
                continue;
            }
        }

        // If initial is valid, then extend the end_index for length of initial
        // cluster of consonants.
        // The longest length of an initial with unfolded regex is 16 UTF-16 bytes.
        for (int initial_len = removeRegexCharacters
                                   ? 2
                                   : std::min(16,
                                              static_cast<int>(
                                                  processedString.size())
                                                  - end_idx);
             initial_len > 0;
             initial_len--) {
            currentString = processedString.mid(end_idx, initial_len).toLower();

            bool isValidInitial = false;
            if (removeRegexCharacters) {
                isValidInitial = initials.find(currentString.toStdString())
                                 != initials.end();
            } else {
                std::vector<QString> stringsToSearch;
                QString stringToSearch = currentString;
                unfoldJyutpingRegex(stringToSearch, stringsToSearch);

                for (const auto &s : stringsToSearch) {
                    auto searchResult = initials.find(s.toStdString());
                    isValidInitial = isValidInitial
                                     || (searchResult != initials.end());
                }
            }

            if (!isValidInitial) {
                continue;
            }

            if (initial_found) {
                // Multiple initials in a row are only valid if previous "initial"
                // was actually a final (like m or ng)
                QString previousInitial = processedString
                                              .mid(start_idx,
                                                   end_idx - start_idx)
                                              .toLower();

                bool previousInitialIsValidFinal = false;
                if (removeRegexCharacters) {
                    previousInitialIsValidFinal
                        = finals.find(previousInitial.toStdString())
                          != initials.end();
                } else {
                    std::vector<QString> stringsToSearch;
                    QString stringToSearch = previousInitial;
                    unfoldJyutpingRegex(stringToSearch, stringsToSearch);

                    for (const auto &s : stringsToSearch) {
                        auto searchResult = finals.find(s.toStdString());
                        previousInitialIsValidFinal = previousInitialIsValidFinal
                                                      || (searchResult
                                                          != initials.end());
                    }
                }

                if (previousInitialIsValidFinal) {
                    syllables.push_back(previousInitial.toStdString());
                    start_idx = end_idx;
                } else {
                    valid_jyutping = false;
                }
            }

            end_idx += initial_len;
            component_found = true;
            initial_found = true;
        }

        if (component_found) {
            continue;
        }

        // If final is valid, then extend end_index for length of final.
        // Check for number at end of word as well (this represents tone number).
        //
        // Then add the substring from [start_index, end_index) to vector
        // and reset start_index, so we can start searching after the end_index.
        for (int final_len = removeRegexCharacters
                                 ? 4
                                 : std::min(16,
                                            static_cast<int>(
                                                processedString.size())
                                                - end_idx);
             final_len > 0;
             final_len--) {
            currentString = processedString.mid(end_idx, final_len).toLower();

            bool isValidFinal = false;
            if (removeRegexCharacters) {
                isValidFinal = finals.find(currentString.toStdString())
                               != finals.end();
            } else {
                std::vector<QString> stringsToSearch;
                QString stringToSearch = currentString;
                unfoldJyutpingRegex(stringToSearch, stringsToSearch);

                for (const auto &s : stringsToSearch) {
                    auto searchResult = finals.find(s.toStdString());
                    isValidFinal = isValidFinal
                                   || (searchResult != finals.end());
                }
            }

            if (isValidFinal) {
                end_idx += final_len;
                if (end_idx < processedString.length()) {
                    if (processedString.at(end_idx).isDigit()) {
                        if (processedString.at(end_idx).digitValue() < 1
                            || processedString.at(end_idx).digitValue() > 6) {
                            valid_jyutping = false;
                        }

                        end_idx++;
                    }
                }
                QString syllable = processedString
                                       .mid(start_idx, end_idx - start_idx)
                                       .toLower();
                syllables.push_back(syllable.toStdString());
                start_idx = end_idx;
                component_found = true;
                initial_found = false;
                break;
            }
        }

        if (component_found) {
            continue;
        } else {
            valid_jyutping = false;
        }

        end_idx++;
    }

    // Then add whatever's left in the search term, minus whitespace.
    QString lastSyllable
        = string.mid(start_idx, end_idx - start_idx).simplified().toLower();
    if (!lastSyllable.isEmpty() && lastSyllable != "'") {
        syllables.push_back(lastSyllable.toStdString());
        if (finals.find(lastSyllable.toStdString()) == finals.end()) {
            valid_jyutping = false;
        }
    }

    out = syllables;
    return valid_jyutping;
}

} // namespace CantoneseUtils
