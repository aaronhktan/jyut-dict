#include "entry.h"

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
    if (_definitions.size() > 0) {
        for (auto definition: _definitions) {
            if (!definition.isEmpty()) {
                return definition.getDefinitionsSnippet();
            }
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
