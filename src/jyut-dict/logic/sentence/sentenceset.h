#ifndef SENTENCESET_H
#define SENTENCESET_H

#include <ostream>
#include <string>
#include <vector>

// The SentenceSet class contains a grouping of TargetSentences (aka translations
// of sentences); where all of the translations are related by source.

namespace Sentence {

struct TargetSentence
{
    std::string language;
    std::string sentence;
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