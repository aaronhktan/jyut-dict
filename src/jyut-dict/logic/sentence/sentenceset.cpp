#include "sentenceset.h"

#include "logic/dictionary/dictionarysource.h"

SentenceSet::SentenceSet(const std::string &source)
    : _source{source}
    , _sourceShortString{DictionarySourceUtils::getSourceShortString(source)}
{
}

SentenceSet::SentenceSet(const std::string &source,
                         const std::vector<Sentence::TargetSentence> &sentences)
    : _source{source}
    , _sourceShortString{DictionarySourceUtils::getSourceShortString(source)}
    , _sentences{sentences}
{
}

std::ostream &operator<<(std::ostream &out, const SentenceSet &sentenceSet)
{
    out << "======== source\n" << sentenceSet.getSource() << "\n";
    for (const auto &sentence : sentenceSet.getSentences()) {
        out << "=========== translation\n" << sentence.sentence << "\n";
        out << "=========== language\n" << sentence.language << "\n";
        out << "=========== directTarget\n" << sentence.directTarget << "\n";
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

const std::string &SentenceSet::getSource(void) const
{
    return _source;
}

const std::string &SentenceSet::getSourceLongString(void) const
{
    return _source;
}

const std::string &SentenceSet::getSourceShortString(void) const
{
    return _sourceShortString;
}

std::span<const Sentence::TargetSentence> SentenceSet::getSentenceSnippet(
    void) const
{
    if (!_snippet.empty()) {
        return _snippet;
    }

    for (int i = 0; i < std::min(static_cast<size_t>(5), _sentences.size());
         ++i) {
        _snippet.emplace_back(_sentences.at(i));
    }

    return _snippet;
}

std::span<const Sentence::TargetSentence> SentenceSet::getSentences() const
{
    return _sentences;
}
