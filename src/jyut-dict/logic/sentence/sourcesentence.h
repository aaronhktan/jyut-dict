#ifndef SOURCESENTENCE_H
#define SOURCESENTENCE_H

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "sentenceset.h"

#include <string>

// The SourceSentence represents a Chinese sentence.
// It contains SentenceSets, which each represent a source and its translations.

class SourceSentence
{
public:
    SourceSentence();
    SourceSentence(std::string sourceLanguage,
                   std::string simplified,
                   std::string traditional,
                   std::string jyutping,
                   std::string pinyin,
                   std::vector<SentenceSet> sentences);
    SourceSentence(const SourceSentence &sourceSentence);
    SourceSentence(const SourceSentence &&sourceSentence);

    ~SourceSentence();

    SourceSentence &operator=(const SourceSentence &sourceSentence);
    SourceSentence &operator=(const SourceSentence &&sourceSentence);
    friend std::ostream &operator<<(std::ostream &out,
                                    const SourceSentence &sourceSentence);

    std::string getSourceLanguage(void) const;
    void setSourceLanguage(std::string sourceLanguage);

    std::string getCharacters(EntryCharactersOptions options) const;

    std::string getSimplified(void) const;
    void setSimplified(std::string simplified);

    std::string getTraditional(void) const;
    void setTraditional(std::string traditional);

    std::string getPhonetic(EntryPhoneticOptions options,
                            CantoneseOptions cantoneseOptions,
                            MandarinOptions mandarinOptions) const;
    std::string getCantonesePhonetic(CantoneseOptions cantoneseOptions) const;
    std::string getMandarinPhonetic(MandarinOptions mandarinOptions) const;

    std::string getJyutping(void) const;
    void setJyutping(std::string jyutping);

    std::string getPinyin(void) const;
    std::string getPrettyPinyin(void) const;
    void setPinyin(std::string pinyin);

    std::vector<SentenceSet> getSentenceSets(void) const;
    std::string getSentenceSnippet(void) const;
    std::string getSentenceSnippetLanguage(void) const;

    void setIsWelcome(const bool isWelcome);
    bool isWelcome(void) const;
    void setIsEmpty(const bool isEmpty);
    bool isEmpty(void) const;

private:
    std::string _sourceLanguage;
    std::string _simplified;
    std::string _traditional;
    std::string _jyutping;
    std::string _pinyin;
    std::string _prettyPinyin;

    std::vector<SentenceSet> _sentences;

    bool _isWelcome = false;
    bool _isEmpty = false;

};

// Required for QueuedConnection
Q_DECLARE_METATYPE(SourceSentence);

#endif // SOURCESENTENCE_H
