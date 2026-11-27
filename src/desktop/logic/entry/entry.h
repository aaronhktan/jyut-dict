#ifndef ENTRY_H
#define ENTRY_H

#include "logic/entry/definitionsset.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"

#include <QObject>

#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <vector>

class QVariant;

// The Entry class is very important, as it is the representation
// of an entry in the dictionary.

// It contains multiple functions that modify, change, and return aspects of an entry

class Entry : public QObject
{
public:
    explicit Entry() = default;
    Entry(size_t id,
          const std::string &traditional,
          const std::string &simplified,
          const std::string &jyutping,
          const std::string &pinyin,
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

    size_t id() const { return _id; }

    const std::string &getCharacters(
        EntryCharactersOptions options,
        bool useColours,
        EntryColourPhoneticType colourPhoneticType,
        std::span<const std::string> jyutpingToneColours,
        std::span<const std::string> pinyinToneColours) const;
    const std::string &getCharactersNoSecondary(
        EntryCharactersOptions options,
        bool useColours,
        EntryColourPhoneticType colourPhoneticType,
        std::span<const std::string> jyutpingToneColours,
        std::span<const std::string> pinyinToneColours) const;

    const std::string &getTraditional(void) const;
    void setTraditional(const std::string &traditional);

    const std::string &getSimplified(void) const;
    void setSimplified(const std::string &simplified);

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
    const std::vector<uint8_t> &getJyutpingNumbers() const;

    const std::string &getPinyin(void) const;
    void setPinyin(const std::string &pinyin);
    const std::vector<uint8_t> &getPinyinNumbers() const;

    std::span<const DefinitionsSet> getDefinitionsSets(void) const;
    const std::string &getDefinitionSnippet(void);
    void addDefinitions(const std::string &source,
                        const std::vector<Definition::Definition> &definitions);


    void setIsWelcome(const bool isWelcome);
    bool isWelcome(void) const;
    void setIsEmpty(const bool isEmpty);
    bool isEmpty(void) const;

private:
    bool generatePhonetic(
        CantoneseOptions cantoneseOptions = CantoneseOptions::NONE,
        MandarinOptions mandarinOptions = MandarinOptions::NONE) const;
    void refreshColours(const EntryColourPhoneticType type,
                        std::span<const std::string> jyutpingToneColours,
                        std::span<const std::string> pinyinToneColours) const;

    size_t _id;
    std::string _traditional;
    std::string _traditionalDifference;
    std::string _preferTraditional;
    std::string _simplified;
    std::string _simplifiedDifference;
    std::string _preferSimplified;

    mutable std::optional<std::string> _colouredTraditional = std::nullopt;
    mutable std::optional<std::string> _colouredTraditionalDifference
        = std::nullopt;
    mutable std::optional<std::string> _colouredPreferTraditional = std::nullopt;
    mutable std::optional<std::string> _colouredSimplified = std::nullopt;
    mutable std::optional<std::string> _colouredSimplifiedDifference
        = std::nullopt;
    mutable std::optional<std::string> _colouredPreferSimplified = std::nullopt;
    mutable std::optional<uint64_t> _cachedCharacterHash = std::nullopt;

    std::string _jyutping;
    mutable std::optional<std::string> _yale = std::nullopt;
    mutable std::optional<std::string> _cantoneseIPA = std::nullopt;
    mutable std::optional<std::vector<uint8_t>> _jyutpingNumbers = std::nullopt;

    std::string _pinyin;
    mutable std::optional<std::string> _prettyPinyin = std::nullopt;
    mutable std::optional<std::string> _numberedPinyin = std::nullopt;
    mutable std::optional<std::string> _zhuyin = std::nullopt;
    mutable std::optional<std::string> _mandarinIPA = std::nullopt;
    mutable std::optional<std::vector<uint8_t>> _pinyinNumbers = std::nullopt;

    std::vector<DefinitionsSet> _definitions;
    std::string _definitionSnippet;

    bool _isWelcome = false;
    bool _isEmpty = false;
};

Q_DECLARE_METATYPE(Entry);

#endif // ENTRY_H
