#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "translationset.h"

#include <optional>
#include <span>
#include <string>

// Example contains an example, and a set of translations for that example.

class Example
{
public:
    explicit Example() = default;
    Example(const std::string &sourceLanguage,
                   const std::string &simplified,
                   const std::string &traditional,
                   const std::string &jyutping,
                   const std::string &pinyin,
                   const std::vector<TranslationSet> &translationSets);

    friend std::ostream &operator<<(std::ostream &out,
                                    const Example &example);
    bool operator==(const Example &other) const
    {
        return _sourceLanguage == other._sourceLanguage
               && _simplified == other._simplified
               && _traditional == other._traditional
               && _jyutping == other._jyutping && _pinyin == other._pinyin
               && _translationSets == other._translationSets;
    }

    const std::string &getSourceLanguage(void) const;
    void setSourceLanguage(std::string sourceLanguage);

    const std::string &getCharacters(EntryCharactersOptions options) const;

    const std::string &getSimplified(void) const;
    void setSimplified(std::string simplified);

    const std::string &getTraditional(void) const;
    void setTraditional(std::string traditional);

    bool generatePhonetic(CantoneseOptions cantoneseOptions,
                          MandarinOptions mandarinOptions);

    const std::string &getPhonetic(EntryPhoneticOptions options,
                                   CantoneseOptions cantoneseOptions,
                                   MandarinOptions mandarinOptions) const;
    const std::string &getCantonesePhonetic(
        CantoneseOptions cantoneseOptions) const;
    const std::string &getMandarinPhonetic(MandarinOptions mandarinOptions) const;

    const std::string &getJyutping(void) const;
    void setJyutping(const std::string &jyutping);

    const std::string &getPinyin(void) const;
    const std::string &getPrettyPinyin(void) const;
    void setPinyin(const std::string &pinyin);

    std::span<const TranslationSet> getTranslationSets(void) const;
    std::string getTranslationSnippet(void) const;
    std::string getTranslationSnippetLanguage(void) const;

    void setIsWelcome(const bool isWelcome);
    bool isWelcome(void) const;
    void setIsEmpty(const bool isEmpty);
    bool isEmpty(void) const;

private:
    std::string _sourceLanguage;
    std::string _simplified;
    std::string _traditional;

    std::string _jyutping;
    std::optional<std::string> _yale = std::nullopt;
    std::optional<std::string> _cantoneseIPA = std::nullopt;

    std::string _pinyin;
    std::optional<std::string> _prettyPinyin = std::nullopt;
    std::optional<std::string> _numberedPinyin = std::nullopt;
    std::optional<std::string> _zhuyin = std::nullopt;
    std::optional<std::string> _mandarinIPA = std::nullopt;

    std::vector<TranslationSet> _translationSets;

    bool _isWelcome = false;
    bool _isEmpty = false;

};

// Required for QueuedConnection
Q_DECLARE_METATYPE(Example);

#endif // EXAMPLE_H
