#include "entry.h"

#include "logic/settings/settings.h"

#include <codecvt>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

static std::unordered_map<std::string, std::vector<std::string>> replacementMap = {
    {"a", {"ā", "á", "ǎ", "à", "a"}},
    {"e", {"ē", "é", "ě", "è", "e"}},
    {"i", {"ī", "í", "ǐ", "ì", "i"}},
    {"o", {"ō", "ó", "ǒ", "ò", "o"}},
    {"u", {"ū", "ú", "ǔ", "ù", "u"}},
    {"ü", {"ǖ", "ǘ", "ǚ", "ǜ", "ü"}},
};

static std::unordered_set<std::string> specialCharacters = {
    "，", "%", "－", "…", "·",
};

#ifdef _MSC_VER
    static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
#else
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
#endif

#ifdef Q_OS_WIN
#define SAME_CHARACTER_STRING "−"
#else
#define SAME_CHARACTER_STRING "―"
#endif

Entry::Entry()
    : QObject()
{
    _simplified = "";
    _traditional = "";
    _jyutping = "";
    _pinyin = "";
    _prettyPinyin = "";
    _definitions = {};
    _derivedWords = {};
    _sentences = {};
    _isWelcome = false;
    _isEmpty = false;
}

Entry::Entry(std::string simplified, std::string traditional,
             std::string jyutping, std::string pinyin,
             std::vector<DefinitionsSet> definitions,
             std::vector<std::string> derivedWords,
             std::vector<SourceSentence> sentences)
    : _simplified{simplified},
      _traditional{traditional},
      _jyutping{jyutping},
      _pinyin{pinyin},
      _definitions{definitions},
      _derivedWords{derivedWords},
      _sentences{sentences}
{
    _isWelcome = false;
    _isEmpty = false;

    // Normalize pinyin and jyutping to lowercase >:(
    std::transform(_jyutping.begin(), _jyutping.end(), _jyutping.begin(), ::tolower);
    std::transform(_pinyin.begin(), _pinyin.end(), _pinyin.begin(), ::tolower);

    // Create comparison versions of traditional and simplified entries
    compareStrings(_simplified, _traditional, _traditionalDifference);
    compareStrings(_traditional, _simplified, _simplifiedDifference);

    // Create pretty pinyin
    _prettyPinyin = createPrettyPinyin();
}

Entry::Entry(const Entry &entry)
    : QObject(),
      _simplified{entry._simplified},
      _simplifiedDifference{entry._simplifiedDifference},
      _traditional{entry._traditional},
      _traditionalDifference{entry._traditionalDifference},
      _colouredSimplified{entry._colouredSimplified},
      _colouredSimplifiedDifference{entry._colouredSimplifiedDifference},
      _colouredTraditional{entry._colouredTraditional},
      _colouredTraditionalDifference{entry._colouredTraditionalDifference},
      _jyutping{entry._jyutping},
      _pinyin{entry._pinyin},
      _prettyPinyin{entry._prettyPinyin},
      _definitions{entry.getDefinitionsSets()},
      _derivedWords{entry.getDerivedWords()},
      _sentences{entry.getSentences()},
      _isWelcome{entry.isWelcome()},
      _isEmpty{entry.isEmpty()}
{

}

Entry::Entry(const Entry &&entry)
    : _simplified{std::move(entry._simplified)},
      _simplifiedDifference{std::move(entry._simplifiedDifference)},
      _traditional{std::move(entry._traditional)},
      _traditionalDifference{std::move(entry._traditionalDifference)},
      _colouredSimplified{std::move(entry._colouredSimplified)},
      _colouredSimplifiedDifference{std::move(entry._colouredSimplifiedDifference)},
      _colouredTraditional{std::move(entry._colouredTraditional)},
      _colouredTraditionalDifference{std::move(entry._colouredTraditionalDifference)},
      _jyutping{std::move(entry._jyutping)},
      _pinyin{std::move(entry._pinyin)},
      _prettyPinyin{entry._prettyPinyin},
      _definitions{std::move(entry._definitions)},
      _derivedWords{std::move(entry._derivedWords)},
      _sentences{std::move(entry._sentences)},
      _isWelcome{entry.isWelcome()},
      _isEmpty{entry.isEmpty()}
{

}

Entry::~Entry()
{

}

Entry &Entry::operator=(const Entry &entry)
{
    if (&entry == this) {
        return *this;
    }

    _simplified = entry._simplified;
    _simplifiedDifference = entry._simplifiedDifference;
    _traditional = entry._traditional;
    _traditionalDifference = entry._traditionalDifference;
    _colouredSimplified = entry._colouredSimplified;
    _colouredSimplifiedDifference = entry._colouredSimplifiedDifference;
    _colouredTraditional = entry._colouredTraditional;
    _colouredTraditionalDifference = entry._colouredTraditionalDifference;
    _jyutping = entry._jyutping;
    _pinyin = entry._pinyin;
    _prettyPinyin = entry._prettyPinyin;
    _definitions = entry.getDefinitionsSets();
    _derivedWords = entry.getDerivedWords();
    _sentences = entry.getSentences();
    _isWelcome = entry.isWelcome();
    _isEmpty = entry.isEmpty();

    return *this;
}

Entry &Entry::operator=(const Entry &&entry)
{
    if (&entry == this) {
        return *this;
    }

    _simplified = entry._simplified;
    _simplifiedDifference = entry._simplifiedDifference;
    _traditional = entry._traditional;
    _traditionalDifference = entry._traditionalDifference;
    _colouredSimplified = entry._colouredSimplified;
    _colouredSimplifiedDifference = entry._colouredSimplifiedDifference;
    _colouredTraditional = entry._colouredTraditional;
    _colouredTraditionalDifference = entry._colouredTraditionalDifference;
    _jyutping = entry._jyutping;
    _pinyin = entry._pinyin;
    _prettyPinyin = entry._prettyPinyin;
    _definitions = entry.getDefinitionsSets();
    _derivedWords = entry.getDerivedWords();
    _sentences = entry.getSentences();
    _isWelcome = entry.isWelcome();
    _isEmpty = entry.isEmpty();

    return *this;
}

std::ostream &operator<<(std::ostream &out, const Entry &entry)
{
    out << "Simplified: " << entry.getSimplified() << "\n";
    out << "Traditional: " << entry.getTraditional() << "\n";
    out << "Jyutping: " << entry.getJyutping() << "\n";
    out << "Pinyin: " << entry.getPinyin() << "\n";
    for (size_t i = 0; i < entry.getDefinitionsSets().size(); i++) {
        out << entry.getDefinitionsSets()[i] << "\n";
    }
//    for (size_t i = 0; i < entry.getSentences().size(); i++) {
//        out << entry.getSentences()[i] << "\n";
//    }
    return out;
}

std::string Entry::getCharacters(EntryCharactersOptions options, bool use_colours) const
{
    switch (options) {
        case (EntryCharactersOptions::ONLY_SIMPLIFIED): {
            if (use_colours) {
                return _colouredSimplified;
            }
            return _simplified;
        }
        case (EntryCharactersOptions::ONLY_TRADITIONAL): {
            if (use_colours) {
                return _colouredTraditional;
            }
            return _traditional;
        }
        case (EntryCharactersOptions::PREFER_SIMPLIFIED): {
            if (use_colours) {
                return _colouredSimplified + " [" + _colouredTraditionalDifference + "]";
            }
            return _simplified + " [" + _traditionalDifference + "]";
        }
        case (EntryCharactersOptions::PREFER_TRADITIONAL): {
            if (use_colours) {
                return _colouredTraditional + " [" + _colouredSimplifiedDifference + "]";
            }
            return _traditional + " [" + _simplifiedDifference + "]";
        }
    }

    return _simplified;
}

std::string Entry::getSimplified(void) const
{
    return _simplified;
}

void Entry::setSimplified(std::string simplified)
{
    _simplified = simplified;
}

std::string Entry::getTraditional(void) const
{
    return _traditional;
}

void Entry::setTraditional(std::string traditional)
{
    _traditional = traditional;
}

std::string Entry::getPhonetic(EntryPhoneticOptions options) const
{
    return getPhonetic(options, CantoneseOptions::RAW_JYUTPING, MandarinOptions::RAW_PINYIN);
}

std::string Entry::getPhonetic(EntryPhoneticOptions options, MandarinOptions mandarinOptions) const
{
    return getPhonetic(options, CantoneseOptions::RAW_JYUTPING, mandarinOptions);
}

std::string Entry::getPhonetic(EntryPhoneticOptions options, CantoneseOptions cantoneseOptions) const
{
    return getPhonetic(options, cantoneseOptions, MandarinOptions::RAW_PINYIN);
}

std::string Entry::getPhonetic(EntryPhoneticOptions options, CantoneseOptions cantoneseOptions, MandarinOptions mandarinOptions) const
{
    switch (options) {
        case EntryPhoneticOptions::ONLY_JYUTPING: {
            return getCantonesePhonetic(cantoneseOptions);
        }
        case EntryPhoneticOptions::ONLY_PINYIN: {
            return getMandarinPhonetic(mandarinOptions);
        }
        case EntryPhoneticOptions::PREFER_JYUTPING: {
            std::string jyutping = getCantonesePhonetic(cantoneseOptions);
            std::string pinyin = getMandarinPhonetic(mandarinOptions);
            return jyutping + " (" + pinyin + ")";
        }
        case EntryPhoneticOptions::PREFER_PINYIN: {
            std::string jyutping = getCantonesePhonetic(cantoneseOptions);
            std::string pinyin = getMandarinPhonetic(mandarinOptions);
            return pinyin + " (" + jyutping + ")";
        }
    }
    return _jyutping;
}

std::string Entry::getCantonesePhonetic(CantoneseOptions cantoneseOptions) const
{
    switch (cantoneseOptions) {
        case CantoneseOptions::RAW_JYUTPING:
        default:
        return _jyutping;
    }
}

std::string Entry::getMandarinPhonetic(MandarinOptions mandarinOptions) const
{
    switch (mandarinOptions) {
        case MandarinOptions::PRETTY_PINYIN:
            return _prettyPinyin;
        case MandarinOptions::RAW_PINYIN:
        default:
            return _pinyin;
    }
}

std::string Entry::getJyutping(void) const
{
    return _jyutping;
}

void Entry::setJyutping(std::string jyutping)
{
    _jyutping = jyutping;
}

std::vector<int> Entry::getJyutpingNumbers() const
{
    std::vector<int> jyutpingNumbers;

    if (_jyutping.empty()) {
        return getPinyinNumbers();
    }

    size_t pos = _jyutping.find_first_of("0123456");
    while(pos != std::string::npos) {
        jyutpingNumbers.push_back(_jyutping.at(pos) - '0');
        pos = _jyutping.find_first_of("0123456", pos + 1);
    }

return jyutpingNumbers;
}

std::string Entry::getPinyin(void) const
{
    return _pinyin;
}

std::string Entry::getPrettyPinyin(void) const
{
    return _prettyPinyin;
}

void Entry::setPinyin(std::string pinyin)
{
    _pinyin = pinyin;
}

std::vector<int> Entry::getPinyinNumbers() const
{
    std::vector<int> pinyinNumbers;

    size_t pos = _pinyin.find_first_of("012345");
    while(pos != std::string::npos) {
        pinyinNumbers.push_back(_pinyin.at(pos) - '0');
        pos = _pinyin.find_first_of("012345", pos + 1);
    }

    return pinyinNumbers;
}

std::vector<DefinitionsSet> Entry::getDefinitionsSets(void) const
{
    return _definitions;
}

std::string Entry::getDefinitionSnippet(void) const
{
    if (_definitions.empty()) {
        return "";
    }

    for (auto definition: _definitions) {
        if (!definition.isEmpty()) {
            return definition.getDefinitionsSnippet();
        }
    }

    return "";
}

void Entry::addDefinitions(std::string source, std::vector<std::string> definitions)
{
    _definitions.push_back(DefinitionsSet{source, definitions});
}

std::vector<std::string> Entry::getDerivedWords(void) const
{
    return _derivedWords;
}

void Entry::setDerivedWords(std::vector<std::string> derivedWords)
{
    _derivedWords = derivedWords;
}

std::vector<SourceSentence> Entry::getSentences(void) const
{
    return _sentences;
}

void Entry::setSentences(std::vector<SourceSentence> sentences)
{
    _sentences = sentences;
}

void Entry::refreshColours(const EntryColourPhoneticType type)
{
    std::vector<int> tones;
    switch (type) {
    case EntryColourPhoneticType::NONE: {
        _colouredSimplified = _simplified;
        _colouredTraditional = _traditional;
        _colouredSimplifiedDifference = _simplifiedDifference;
        _colouredTraditionalDifference = _traditionalDifference;
        return;
    }
    case EntryColourPhoneticType::JYUTPING: {
        tones = getJyutpingNumbers();
        break;
    }
    case EntryColourPhoneticType::PINYIN: {
        tones = getPinyinNumbers();
        break;
    }
    }

    // Create coloured versions of Simplified and Traditional characters
    _colouredSimplified = applyColours(_simplified, tones, type);
    _colouredTraditional = applyColours(_traditional, tones, type);
    _colouredSimplifiedDifference = applyColours(_simplifiedDifference, tones, type);
    _colouredTraditionalDifference = applyColours(_traditionalDifference, tones, type);
}

void Entry::setIsWelcome(const bool isWelcome)
{
    _isWelcome = isWelcome;
}

bool Entry::isWelcome(void) const
{
    return _isWelcome;
}

void Entry::setIsEmpty(const bool isEmpty)
{
    _isEmpty = isEmpty;
}

bool Entry::isEmpty(void) const
{
    return _isEmpty;
}

std::string Entry::applyColours(std::string original,
                                std::vector<int> tones,
                                EntryColourPhoneticType type) const
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
        if (character == converter.from_bytes(SAME_CHARACTER_STRING)[0]) {
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
                               + Settings::jyutpingToneColours.at(
                                     static_cast<size_t>(tone))
                               + "\">";
            break;
        }
        case EntryColourPhoneticType::PINYIN: {
            coloured_string += "<font color=\""
                               + Settings::pinyinToneColours.at(
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


// The entry first converts both the simplified and traditional strings into
// u32strings on macOS and Linux, or wstrings on Windows.
//
// Since there is no guarantee of byte length per "character" in each string
// (as most Latin characters are one byte and Chinese characters are usually
// 3 bytes due to multibyte encoding schemes of UTF-8, and several entries
// consist of both latin and chinese character combinations), converting
// the string to u32string/wstring allows us to advance through the string
// "character" by "character", as a human would view it,
// even though the actual byte array is not stored that way.
//
// For each character, compare each of the characters between the simplified and
// traditional versions; if characters are different
// add that character to the comparison string.
// Otherwise, mark that character as the same between simplified and traditional
// by concatenating a full-width dash character to the comparison string.
//
// Finally, concatenate the preferred option with the comparison string,
// with the comparison string surrounded in curly braces.
//
// Example return values with an entry Traditional: 身體, Simplified: 身体
// With EntryCharactersOptions::PREFER_SIMPLIFIED:  "身体 {－體}"
// With EntryCharactersOptions::PREFER_TRADITIONAL: "身體 {－体}"
void Entry::compareStrings(std::string original, std::string comparison,
                           std::string &returnString)
{
    std::string modifiedComparison;

#ifdef _MSC_VER
        std::wstring convertedOriginal = converter.from_bytes(original);
        std::wstring convertedComparison = converter.from_bytes(comparison);
#else
        std::u32string convertedOriginal = converter.from_bytes(original);
        std::u32string convertedComparison = converter.from_bytes(comparison);
#endif
    if (convertedOriginal.size() != convertedComparison.size()) {
        returnString.clear();
        return;
    }

    for (size_t i = 0; i < convertedOriginal.size(); i++) {
        std::string currentCharacter = converter.to_bytes(convertedComparison[i]);

        auto isSpecialCharacter = specialCharacters.find(currentCharacter) != specialCharacters.end();
        if (isSpecialCharacter
                || convertedOriginal[i] != convertedComparison[i]) {
            modifiedComparison += currentCharacter;
            continue;
        }

        if (std::find_if(currentCharacter.begin(), currentCharacter.end(), isalpha) != currentCharacter.end()) {
            modifiedComparison += currentCharacter;
            continue;
        }

        modifiedComparison += SAME_CHARACTER_STRING;
    }

    returnString = modifiedComparison;
}

// explodePhonetic takes a string and a delimiter, then separates that string up
// into its components as delimited by, you guessed it, the delimiter.
// Similar to the .split() function in Python and JavaScript.
std::vector<std::string> Entry::explodePhonetic(const std::string &string,
                                                const char delimiter) const
{
    std::vector<std::string> words;
    std::stringstream ss(string);
    std::string word;

    while (std::getline(ss, word, delimiter)) {
        words.push_back(word);
    }

    return words;
}

std::string Entry::createPrettyPinyin(void)
{
    std::string result;

    // Create a vector of each space-separated value in pinyin
    std::vector<std::string> syllables = explodePhonetic(_pinyin, ' ');
    if (syllables.empty()) {
        return _pinyin;
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
            location = std::string::npos;
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
