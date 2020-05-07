#include "sentenceset.h"

#include "logic/dictionary/dictionarysource.h"

SentenceSet::SentenceSet(std::string source)
    : _source{source}
{
}

SentenceSet::SentenceSet(std::string source,
                         std::vector<Sentence::TargetSentence> sentences)
    : _source{source}
    , _sentences{sentences}
{
}

SentenceSet::SentenceSet(const SentenceSet &sentenceSet)
    : _source{sentenceSet.getSource()},
      _sentences{sentenceSet.getSentences()}
{
}

SentenceSet::SentenceSet(const SentenceSet &&sentenceSet)
    : _source{std::move(sentenceSet._source)},
      _sentences{std::move(sentenceSet._sentences)}
{
}

SentenceSet::~SentenceSet()
{
}

SentenceSet &SentenceSet::operator=(const SentenceSet &sentence)
{
    if (&sentence == this) {
        return *this;
    }

    _source = sentence.getSource();
    _sentences = sentence.getSentences();

    return *this;
}

SentenceSet &SentenceSet::operator=(const SentenceSet &&sentence)
{
    if (&sentence == this) {
        return *this;
    }

    _source = std::move(sentence._source);
    _sentences = std::move(sentence._sentences);

    return *this;
}

std::ostream &operator<<(std::ostream &out, const SentenceSet &sentenceSet)
{
    for (Sentence::TargetSentence sentence : sentenceSet.getSentences()) {
        out << sentence.sentence << "\n";
    }
    return out;
}

bool SentenceSet::isEmpty(void) const
{
    return _sentences.empty();
}

bool SentenceSet::pushSentence(Sentence::TargetSentence sentence)
{
    _sentences.push_back(sentence);
    return true;
}

std::string SentenceSet::getSource(void) const
{
    return _source;
}

std::string SentenceSet::getSourceLongString(void) const
{
    return _source;
}

std::string SentenceSet::getSourceShortString() const
{
    return DictionarySourceUtils::getSourceShortString(_source);
}

std::vector<Sentence::TargetSentence> SentenceSet::getSentenceSnippet(void) const
{
    std::vector<Sentence::TargetSentence> snippet;
    std::size_t size = _sentences.size() >= 5 ? 5 : _sentences.size();
    std::copy(_sentences.begin(),
              _sentences.begin() + size,
              std::back_inserter(snippet));
    return snippet;
}

std::vector<Sentence::TargetSentence> SentenceSet::getSentences() const
{
    return _sentences;
}
