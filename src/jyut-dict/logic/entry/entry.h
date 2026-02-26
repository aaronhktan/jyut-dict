#ifndef ENTRY_H
#define ENTRY_H

#include "logic/entry/definitionsset.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"

#include <QObject>
#include <QVariant>

#include <ostream>
#include <span>
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

    const std::string &getCharacters(EntryCharactersOptions options,
                                     bool useColours) const;
    const std::string &getCharactersNoSecondary(EntryCharactersOptions options,
                                                bool useColours) const;

    const std::string &getSimplified(void) const;
    void setSimplified(std::string simplified);

    const std::string &getTraditional(void) const;
    void setTraditional(std::string traditional);

    bool generatePhonetic(CantoneseOptions cantoneseOptions,
                          MandarinOptions mandarinOptions);
    bool generateDefinitionsPhonetic(CantoneseOptions cantoneseOptions,
                                     MandarinOptions mandarinOptions);

    std::string getPhonetic(EntryPhoneticOptions options) const;
    std::string getPhonetic(EntryPhoneticOptions options,
                            MandarinOptions mandarinOptions) const;
    std::string getPhonetic(EntryPhoneticOptions options,
                            CantoneseOptions cantoneseOptions) const;
    std::string getPhonetic(EntryPhoneticOptions options,
                            CantoneseOptions cantoneseOptions,
                            MandarinOptions mandarinOptions) const;

    const std::string &getCantonesePhonetic(
        CantoneseOptions cantoneseOptions) const;
    const std::string &getMandarinPhonetic(MandarinOptions mandarinOptions) const;

    const std::string &getJyutping(void) const;
    void setJyutping(const std::string &jyutping);
    const std::vector<uint8_t> &getJyutpingNumbers();

    const std::string &getPinyin(void) const;
    void setPinyin(const std::string &pinyin);
    const std::vector<uint8_t> &getPinyinNumbers();

    std::span<const DefinitionsSet> getDefinitionsSets(void) const;
    const std::string &getDefinitionSnippet(void);
    void addDefinitions(const std::string &source,
                        const std::vector<Definition::Definition> &definitions);

    void refreshColours(
        const EntryColourPhoneticType type = EntryColourPhoneticType::CANTONESE);

    void setIsWelcome(const bool isWelcome);
    bool isWelcome(void) const;
    void setIsEmpty(const bool isEmpty);
    bool isEmpty(void) const;

private:
    std::string _simplified;
    std::string _simplifiedDifference;
    std::string _preferSimplified;
    std::string _traditional;
    std::string _traditionalDifference;
    std::string _preferTraditional;

    std::string _colouredSimplified;
    std::string _colouredSimplifiedDifference;
    std::string _colouredPreferSimplified;
    std::string _colouredTraditional;
    std::string _colouredTraditionalDifference;
    std::string _colouredPreferTraditional;

    std::string _jyutping;
    std::string _yale;
    bool _isYaleValid = false;
    std::string _cantoneseIPA;
    bool _isCantoneseIPAValid = false;

    bool _isJyutpingNumbersValid = false;
    std::vector<uint8_t> _jyutpingNumbers;

    std::string _pinyin;
    std::string _prettyPinyin;
    bool _isPrettyPinyinValid = false;
    std::string _numberedPinyin;
    bool _isNumberedPinyinValid = false;
    std::string _zhuyin;
    bool _isZhuyinValid = false;
    std::string _mandarinIPA;
    bool _isMandarinIPAValid = false;

    bool _isPinyinNumbersValid = false;
    std::vector<uint8_t> _pinyinNumbers;

    std::vector<DefinitionsSet> _definitions;
    std::string _definitionSnippet;

    bool _isWelcome = false;
    bool _isEmpty = false;
};

Q_DECLARE_METATYPE(Entry);

#endif // ENTRY_H
