#include "entry.h"

Entry::Entry()
{

}

Entry::Entry(std::string word, std::string jyutping, std::string pinyin,
             std::vector<DefinitionsSet> definitions,
             std::vector<std::string> derivedWords,
             std::vector<Sentence> sentences)
    : _word{word},
      _jyutping{jyutping},
      _pinyin{pinyin},
      _definitions{definitions},
      _derivedWords{derivedWords},
      _sentences{sentences}
{

}

Entry::Entry(const Entry& entry)
    : _word{entry.getWord()},
      _definitions{entry.getDefinitionsSets()},
      _derivedWords{entry.getDerivedWords()},
      _sentences{entry.getSentences()}
{

}

Entry::Entry(const Entry&& entry)
    : _word{entry.getWord()},
      _definitions{entry.getDefinitionsSets()},
      _derivedWords{entry.getDerivedWords()},
      _sentences{entry.getSentences()}
{

}

Entry& Entry::operator=(Entry& entry)
{
    if (&entry == this) {
        return *this;
    }

    _word = entry.getWord();
    _definitions = entry.getDefinitionsSets();
    _derivedWords = entry.getDerivedWords();
    _sentences = entry.getSentences();

    return *this;
}

Entry& Entry::operator=(Entry&& entry __unused)
{
    if (&entry == this) {
        return *this;
    }

    _word = entry.getWord();
    _definitions = entry.getDefinitionsSets();
    _derivedWords = entry.getDerivedWords();
    _sentences = entry.getSentences();

    return *this;
}

std::ostream& operator<<(std::ostream& out, const Entry& entry)
{
    out << entry.getWord();
    return out;
}

std::string Entry::getWord(void) const
{
    return _word;
}

void Entry::setWord(std::string word)
{
    _word = word;
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
