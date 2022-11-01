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
    Entry() = default;
    Entry(const std::string &simplified, const std::string &traditional,
          const std::string &jyutping, const std::string &pinyin,
          const std::vector<DefinitionsSet> &definitions);

    // Must define rule-of-five functions to allow registering this class
    // as a Qt Metatype, since QObject explicitly deletes them
    ~Entry() override = default;
    Entry(const Entry &entry);
    Entry(Entry &&entry);
    Entry &operator=(const Entry &entry);
    Entry &operator=(Entry &&entry);

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

    bool generatePhonetic(CantoneseOptions cantoneseOptions,
                          MandarinOptions mandarinOptions);

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
    void setJyutping(const std::string &jyutping);
    std::vector<int> getJyutpingNumbers() const;

    std::string getPinyin(void) const;
    std::string getPrettyPinyin(void) const;
    void setPinyin(const std::string &pinyin);
    std::vector<int> getPinyinNumbers() const;

    std::vector<DefinitionsSet> getDefinitionsSets(void) const;
    std::string getDefinitionSnippet(void) const;
    void addDefinitions(const std::string &source,
                        const std::vector<Definition::Definition> &definitions);

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
    std::string _yale;
    bool _isYaleValid = false;

    std::string _pinyin;
    std::string _prettyPinyin;
    bool _isPrettyPinyinValid = false;

    std::vector<DefinitionsSet> _definitions;

    bool _isWelcome = false;
    bool _isEmpty = false;
};

Q_DECLARE_METATYPE(Entry);

#endif // ENTRY_H
