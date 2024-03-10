#ifndef SENTENCESET_H
#define SENTENCESET_H

#include <ostream>
#include <string>
#include <vector>

// The SentenceSet class contains a grouping of TargetSentences (aka translations
// of sentences); where all of the translations are related by source.

namespace Sentence {

// The TargetSentence struct provides:
// the language of the sentence translation, the sentence itself, and whether
// the sentence was a direct translation of the sentence.
struct TargetSentence
{
    std::string sentence;
    std::string language;
    bool directTarget;

    TargetSentence(const std::string &sentence,
                   const std::string &language, bool directTarget):
        sentence{sentence},
        language{language},
        directTarget(directTarget)
    {}

    bool operator==(const TargetSentence &other) const
    {
        return sentence == other.sentence && language == other.language
               && directTarget == other.directTarget;
    }
};

}

class SentenceSet
{
public:
    SentenceSet(const std::string &source);
    SentenceSet(const std::string &source,
                const std::vector<Sentence::TargetSentence> &sentences);
    friend std::ostream &operator<<(std::ostream &out,
                                    const SentenceSet &sentence);

    bool isEmpty(void) const;

    bool pushSentence(const Sentence::TargetSentence &sentence);

    std::string getSource(void) const;
    std::string getSourceLongString(void) const;
    std::string getSourceShortString(void) const;
    std::vector<Sentence::TargetSentence> getSentenceSnippet(void) const;
    std::vector<Sentence::TargetSentence> getSentences(void) const;

private:
    std::string _source;
    std::vector<Sentence::TargetSentence> _sentences;
};

#endif // SENTENCESET_H
