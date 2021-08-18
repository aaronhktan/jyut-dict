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

    TargetSentence(std::string sentence, std::string language, bool directTarget):
        sentence{sentence},
        language{language},
        directTarget(directTarget)
    {}
};

}

class SentenceSet
{
public:
    SentenceSet(std::string source);
    SentenceSet(std::string source,
                std::vector<Sentence::TargetSentence> sentences);
    SentenceSet(const SentenceSet &sentenceSet);
    SentenceSet(const SentenceSet &&sentenceSet);
    SentenceSet &operator=(const SentenceSet &sentenceSet);
    SentenceSet &operator=(const SentenceSet &&sentenceSet);
    friend std::ostream &operator<<(std::ostream &out,
                                    const SentenceSet &sentence);
    ~SentenceSet();

    bool isEmpty(void) const;

    bool pushSentence(Sentence::TargetSentence sentence);

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
