#include "entry.h"

#include <codecvt>
#include <locale>

#include <iostream>
#include <string>
#include <iomanip>

Entry::Entry()
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
}

Entry::Entry(const Entry& entry)
    : _simplified{entry.getSimplified()},
      _traditional{entry.getTraditional()},
      _jyutping{entry.getJyutping()},
      _pinyin{entry.getPinyin()},
      _definitions{entry.getDefinitionsSets()},
      _derivedWords{entry.getDerivedWords()},
      _sentences{entry.getSentences()}
{

}

Entry::Entry(const Entry&& entry)
    : _simplified{std::move(entry._simplified)},
      _traditional{std::move(entry._traditional)},
      _jyutping{std::move(entry._jyutping)},
      _pinyin{std::move(entry._pinyin)},
      _definitions{std::move(entry._definitions)},
      _derivedWords{std::move(entry._derivedWords)},
      _sentences{std::move(entry._sentences)}
{

}

Entry& Entry::operator=(Entry& entry)
{
    if (&entry == this) {
        return *this;
    }

    _simplified = entry.getSimplified();
    _traditional = entry.getTraditional();
    _jyutping = entry.getJyutping();
    _pinyin = entry.getPinyin();
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

    _simplified = entry.getSimplified();
    _traditional = entry.getTraditional();
    _jyutping = entry.getJyutping();
    _pinyin = entry.getPinyin();
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

std::string Entry::getCharacters(EntryCharactersOptions options) const
{
    switch (options) {
        case EntryCharactersOptions::ONLY_SIMPLIFIED: {
            return _simplified;
        }
        case EntryCharactersOptions::ONLY_TRADITIONAL: {
            return _traditional;
        }
        case EntryCharactersOptions::PREFER_SIMPLIFIED: {
            std::string modifiedTraditional;
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
            std::u32string simplified = converter.from_bytes(_simplified);
            std::u32string traditional = converter.from_bytes(_traditional);
            size_t pos = 0;
            for (auto character : traditional) {
                bool isDifferent = false;
                for (size_t i = 0; i < converter.to_bytes(character).size(); i++) {
                    if (_traditional.at(pos + i) != _simplified.at(pos + i)) {
                        modifiedTraditional += converter.to_bytes(character);
                        isDifferent = true;
                        break;
                    }
                }

                if (!isDifferent) {
                    modifiedTraditional += "－";
                }
                pos += converter.to_bytes(character).size();
            }

            return _simplified + " {" + modifiedTraditional + "}";
        }
        case EntryCharactersOptions::PREFER_TRADITIONAL: {
            std::string modifiedSimplified;
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
            std::u32string simplified = converter.from_bytes(_simplified);
            std::u32string traditional = converter.from_bytes(_traditional);
            size_t pos = 0;
            for (auto character : simplified) {
                bool isDifferent = false;
                for (size_t i = 0; i < converter.to_bytes(character).size(); i++) {
                    if (_simplified.at(pos + i) != _traditional.at(pos + i)) {
                        modifiedSimplified += converter.to_bytes(character);
                        isDifferent = true;
                        break;
                    }
                }

                if (!isDifferent) {
                    modifiedSimplified += "－";
                }
                pos += converter.to_bytes(character).size();
            }

            return _traditional + " {" + modifiedSimplified + "}";
        }
        default: {
            return _jyutping;
        }
    }
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
        default: {
            return _jyutping;
        }
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

std::string Entry::getPinyin(void) const
{
    return _pinyin;
}

void Entry::setPinyin(std::string pinyin)
{
    _pinyin = pinyin;
}

std::vector<DefinitionsSet> Entry::getDefinitionsSets(void) const
{
    return _definitions;
}

std::string Entry::getDefinitionSnippet(void) const
{
    if (!_definitions.size()) {
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

Entry::~Entry()
{

}
