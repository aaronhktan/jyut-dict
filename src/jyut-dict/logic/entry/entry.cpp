#include "entry.h"

#include <codecvt>
#include <locale>

#include <iostream>
#include <string>
#include <iomanip>

Entry::Entry()
    : QObject()
{
    _simplified = "";
    _traditional = "";
    _jyutping = "";
    _pinyin = "";
    _definitions = {};
    _derivedWords = {};
    _sentences = {};
}

Entry::Entry(std::string simplified, std::string traditional,
             std::string jyutping, std::string pinyin,
             std::vector<DefinitionsSet> definitions,
             std::vector<std::string> derivedWords,
             std::vector<Sentence> sentences)
    : _simplified{simplified},
      _traditional{traditional},
      _jyutping{jyutping},
      _pinyin{pinyin},
      _definitions{definitions},
      _derivedWords{derivedWords},
      _sentences{sentences}
{
    // Normalize pinyin and jyutping to lowercase >:(
    std::transform(_jyutping.begin(), _jyutping.end(), _jyutping.begin(), ::tolower);
    std::transform(_pinyin.begin(), _pinyin.end(), _pinyin.begin(), ::tolower);

    // Create comparison versions of traditional and simplified entries
    compareStrings(_simplified, _traditional, _traditionalDifference);
    compareStrings(_traditional, _simplified, _simplifiedDifference);

    // Create coloured versions of Simplified and Traditional characters
    _colouredSimplified = applyColours(_simplified, getJyutpingNumbers());
    _colouredTraditional = applyColours(_traditional, getJyutpingNumbers());
    _colouredSimplifiedDifference = applyColours(_simplifiedDifference, getJyutpingNumbers());
    _colouredTraditionalDifference = applyColours(_traditionalDifference, getJyutpingNumbers());
}

Entry::Entry(const Entry& entry)
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
      _definitions{entry.getDefinitionsSets()},
      _derivedWords{entry.getDerivedWords()},
      _sentences{entry.getSentences()}
{

}

Entry::Entry(const Entry&& entry)
    : _simplified{std::move(entry._simplified)},
      _simplifiedDifference{std::move(entry._simplifiedDifference)},
      _traditional{std::move(entry._traditional)},
      _traditionalDifference{std::move(entry._traditionalDifference)},
      _colouredSimplified{entry._colouredSimplified},
      _colouredSimplifiedDifference{entry._colouredSimplifiedDifference},
      _colouredTraditional{entry._colouredTraditional},
      _colouredTraditionalDifference{entry._colouredTraditionalDifference},
      _jyutping{std::move(entry._jyutping)},
      _pinyin{std::move(entry._pinyin)},
      _definitions{std::move(entry._definitions)},
      _derivedWords{std::move(entry._derivedWords)},
      _sentences{std::move(entry._sentences)}
{

}

Entry::~Entry()
{

}

Entry& Entry::operator=(const Entry& entry)
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
    _definitions = entry.getDefinitionsSets();
    _derivedWords = entry.getDerivedWords();
    _sentences = entry.getSentences();

    return *this;
}

Entry& Entry::operator=(Entry&& entry)
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
    _definitions = entry.getDefinitionsSets();
    _derivedWords = entry.getDerivedWords();
    _sentences = entry.getSentences();

    return *this;
}

std::ostream& operator<<(std::ostream& out, const Entry& entry)
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
                return _colouredSimplified + " {" + _colouredTraditionalDifference + "}";
            }
            return _simplified + " {" + _traditionalDifference + "}";
        }
        case (EntryCharactersOptions::PREFER_TRADITIONAL): {
            if (use_colours) {
                return _colouredTraditional + " {" + _colouredSimplifiedDifference + "}";
            }
            return _traditional + " {" + _simplifiedDifference + "}";
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
    switch (options) {
        case EntryPhoneticOptions::ONLY_JYUTPING: {
            return _jyutping;
        }
        case EntryPhoneticOptions::ONLY_PINYIN: {
            return _pinyin;
        }
        case EntryPhoneticOptions::PREFER_JYUTPING: {
            return _jyutping + " {" + _pinyin + "}";
        }
        case EntryPhoneticOptions::PREFER_PINYIN: {
            return _pinyin + " {" + _jyutping + "}";
        }
    }
    return _jyutping;
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

void Entry::addDefinitions(DictionarySource source, std::vector<std::string> definitions)
{
    _definitions.push_back(DefinitionsSet(source, definitions));
}

std::vector<std::string> Entry::getDerivedWords(void) const
{
    return _derivedWords;
}

void Entry::setDerivedWords(std::vector<std::string> derivedWords)
{
    _derivedWords = derivedWords;
}

std::vector<Sentence> Entry::getSentences(void) const
{
    return _sentences;
}

void Entry::setSentences(std::vector<Sentence> sentences)
{
    _sentences = sentences;
}

std::string Entry::applyColours(std::string original,
                                std::vector<int> tones) const
{
    std::string coloured_string;
#ifdef _MSC_VER
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring converted_original = converter.from_bytes(original);
#else
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        std::u32string converted_original = converter.from_bytes(original);
#endif
        size_t pos = 0;
        for (auto character : converted_original) {
            // Skip dashes and full-width commas; they have no colour
            if (character == converter.from_bytes("－")[0]) {
                coloured_string += converter.to_bytes(character);
                pos++;
                continue;
            }

            if (character == converter.from_bytes("， ")[0]) {
                coloured_string += converter.to_bytes(character);
                continue;
            }

            int tone = -1;
            try {
                tone = tones.at(pos);
            } catch (const std::out_of_range& e) {
//                std::cout << "Couldn't get tone at position:, " << e.what();
                continue;
            }

            switch(tone) {
            case 0: {
                coloured_string += "<font color=\"grey\">";
                coloured_string += converter.to_bytes(character);
                coloured_string += "</font>";
                break;
            }
            case 1: {
                coloured_string += "<font color=\"#00bcd4\">";
                coloured_string += converter.to_bytes(character);
                coloured_string += "</font>";
                break;
            }
            case 2: {
                coloured_string += "<font color=\"#7cb342\">";
                coloured_string += converter.to_bytes(character);
                coloured_string += "</font>";
                break;
            }
            case 3: {
                coloured_string += "<font color=\"#657ff1\">";
                coloured_string += converter.to_bytes(character);
                coloured_string += "</font>";
                break;
            }
            case 4: {
                coloured_string += "<font color=\"#c2185b\">";
                coloured_string += converter.to_bytes(character);
                coloured_string += "</font>";
                break;
            }
            case 5: {
                coloured_string += "<font color=\"#068900\">";
                coloured_string += converter.to_bytes(character);
                coloured_string += "</font>";
                break;
            }
            case 6: {
                coloured_string += "<font color=\"#7651d0\">";
                coloured_string += converter.to_bytes(character);
                coloured_string += "</font>";
                break;
            }
            default: {
                coloured_string += converter.to_bytes(character);
                break;
            }
            }
            pos++;
        }

    return coloured_string;
}


// The entry first converts both the simplified and traditional strings into
// u32strings on macOS and Linux, or wstrings on Windows.
//
// Since there is no guarantee of byte length per "character" in each string
// (as most latin characters are one byte and chinese characters are usually
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
                           std::string& returnString)
{
    std::string modifiedComparison;

#ifdef _MSC_VER
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring convertedOriginal = converter.from_bytes(original);
        std::wstring convertedComparison = converter.from_bytes(comparison);
#else
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        std::u32string convertedOriginal = converter.from_bytes(original);
        std::u32string convertedComparison = converter.from_bytes(comparison);
#endif
        if (convertedOriginal.size() != convertedComparison.size()) {
            returnString.clear();
            return;
        }

        for (size_t i = 0; i < convertedOriginal.size(); i++) {
            if (convertedOriginal[i] != convertedComparison[i]) {
                modifiedComparison += converter.to_bytes(convertedComparison[i]);
            } else {
                modifiedComparison += "－";
            }
        }

    returnString = modifiedComparison;
}
