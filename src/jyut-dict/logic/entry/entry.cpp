#include "entry.h"

Entry::Entry()
{

}

Entry::Entry(std::string word, std::string jyutping, std::string pinyin,
             std::vector<std::string> definitions,
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
      _definitions{entry.getDefinitions()},
      _derivedWords{entry.getDerivedWords()},
      _sentences{entry.getSentences()}
{

}

Entry::Entry(const Entry&& entry)
    : _word{entry.getWord()},
      _definitions{entry.getDefinitions()},
      _derivedWords{entry.getDerivedWords()},
      _sentences{entry.getSentences()}
{

}

Entry& Entry::operator=(Entry& entry __unused)
{
    return *this;
}

Entry& Entry::operator=(Entry&& entry __unused)
{
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

std::vector<std::string> Entry::getDefinitions(void) const
{
    return _definitions;
}

void Entry::setDefinitions(std::vector<std::string> definitions)
{
    _definitions = definitions;
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
