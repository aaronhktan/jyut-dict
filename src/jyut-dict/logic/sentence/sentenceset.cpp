#include "sentenceset.h"

#include "logic/dictionary/dictionarysource.h"

SentenceSet::SentenceSet(const std::string &source)
    : _source{source}
{
}

SentenceSet::SentenceSet(const std::string &source,
                         const std::vector<Sentence::TargetSentence> &sentences)
    : _source{source}
    , _sentences{sentences}
{
}

std::ostream &operator<<(std::ostream &out, const SentenceSet &sentenceSet)
{
    for (const auto &sentence : sentenceSet.getSentences()) {
        out << sentence.sentence << "\n";
    }
    return out;
}

bool SentenceSet::isEmpty(void) const
{
    return _sentences.empty()
           || std::all_of(_sentences.begin(),
                          _sentences.end(),
                          [](const Sentence::TargetSentence &sentence) {
                              return sentence.sentence == ""
                                     && sentence.language == ""
                                     && sentence.directTarget == false;
                          });
}

bool SentenceSet::pushSentence(const Sentence::TargetSentence &sentence)
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
    if (_sentences.empty()) {
        return {};
    }
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
