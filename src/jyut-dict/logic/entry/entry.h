#ifndef ENTRY_H
#define ENTRY_H

#include "logic/entry/definitionsset.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/sentence/sourcesentence.h"
#include "logic/settings/settings.h"

#include <QObject>
#include <QVariant>

#include <ostream>
#include <string>
#include <vector>

// The Entry class is very important, as it is the representation
// of an entry in the dictionary.

// It contains multiple functions that modify, change, and return aspects of an entry

class Entry : public QObject
{
public:
    Entry();
    Entry(std::string simplified, std::string traditional,
          std::string jyutping, std::string pinyin,
          std::vector<DefinitionsSet> definitions,
          std::vector<std::string> derivedWords,
          std::vector<SourceSentence> sentences);
    Entry(const Entry &entry);
    Entry(const Entry &&entry);

    ~Entry() override;

    Entry &operator=(const Entry &entry);
    Entry &operator=(const Entry &&entry);
    bool operator==(const Entry &other) const;
    bool operator!=(const Entry &other) const;
    friend std::ostream &operator<<(std::ostream &out, const Entry &entry);

    std::string getCharacters(EntryCharactersOptions options,
                              bool use_colours) const;
    std::string getCharactersNoSecondary(EntryCharactersOptions options,
                                         bool use_colours) const;

    std::string getSimplified(void) const;
    void setSimplified(std::string simplified);

    std::string getTraditional(void) const;
    void setTraditional(std::string traditional);

    std::string getPhonetic(EntryPhoneticOptions options) const;
    std::string getPhonetic(EntryPhoneticOptions options,
                            MandarinOptions mandarinOptions) const;
    std::string getPhonetic(EntryPhoneticOptions options,
                            CantoneseOptions cantoneseOptions) const;
    std::string getPhonetic(EntryPhoneticOptions options,
                            CantoneseOptions cantoneseOptions,
                            MandarinOptions mandarinOptions) const;

    std::string getCantonesePhonetic(CantoneseOptions cantoneseOptions) const;
    std::string getMandarinPhonetic(MandarinOptions mandarinOptions) const;

    std::string getJyutping(void) const;
    void setJyutping(std::string jyutping);
    std::vector<int> getJyutpingNumbers() const;

    std::string getPinyin(void) const;
    std::string getPrettyPinyin(void) const;
    void setPinyin(std::string pinyin);
    std::vector<int> getPinyinNumbers() const;

    std::vector<DefinitionsSet> getDefinitionsSets(void) const;
    std::string getDefinitionSnippet(void) const;
    void addDefinitions(std::string source,
                        std::vector<Definition::Definition> definitions);

    std::vector<std::string> getDerivedWords(void) const;
    void setDerivedWords(std::vector<std::string> derivedWords);

    std::vector<SourceSentence> getSentences(void) const;
    void setSentences(std::vector<SourceSentence> sentences);

    void refreshColours(
        const EntryColourPhoneticType type = EntryColourPhoneticType::JYUTPING);

    void setIsWelcome(const bool isWelcome);
    bool isWelcome(void) const;
    void setIsEmpty(const bool isEmpty);
    bool isEmpty(void) const;

private:
    std::string _simplified;
    std::string _simplifiedDifference;
    std::string _traditional;
    std::string _traditionalDifference;

    std::string _colouredSimplified;
    std::string _colouredSimplifiedDifference;
    std::string _colouredTraditional;
    std::string _colouredTraditionalDifference;

    std::string _jyutping;
    std::string _pinyin;
    std::string _prettyPinyin;

    std::vector<DefinitionsSet> _definitions;
    std::vector<std::string> _derivedWords;
    std::vector<SourceSentence> _sentences;

    bool _isWelcome = false;
    bool _isEmpty = false;
};

Q_DECLARE_METATYPE(Entry);

#endif // ENTRY_H
