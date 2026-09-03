#include "example.h"

#include "logic/utils/cantoneseutils.h"
#include "logic/utils/mandarinutils.h"

Example::Example(const std::string &sourceLanguage,
                               const std::string &simplified,
                               const std::string &traditional,
                               const std::string &jyutping,
                               const std::string &pinyin,
                               const std::vector<TranslationSet> &translationSets)
    : _sourceLanguage{sourceLanguage}
    , _simplified{simplified}
    , _traditional{traditional}
    , _jyutping{jyutping}
    , _pinyin{pinyin}
    , _translationSets{translationSets}
{}

std::ostream &operator<<(std::ostream &out, const Example &example)
{
    out << "Simplified: " << example.getSimplified() << "\n";
    out << "Traditional: " << example.getTraditional() << "\n";
    out << "Jyutping: " << example.getJyutping() << "\n";
    out << "Pinyin: " << example.getPinyin() << "\n";
    for (size_t i = 0; i < example.getTranslationSets().size(); i++) {
        out << example.getTranslationSets()[i] << "\n";
    }
    return out;
}

const std::string &Example::getSourceLanguage(void) const
{
    return _sourceLanguage;
}

void Example::setSourceLanguage(std::string sourceLanguage)
{
    _sourceLanguage = sourceLanguage;
}

const std::string &Example::getCharacters(
    EntryCharactersOptions options) const
{
    switch (options) {
    case EntryCharactersOptions::ONLY_SIMPLIFIED:
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
        return _simplified;
    case EntryCharactersOptions::ONLY_TRADITIONAL:
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        return _traditional;
    }
    return _traditional;
}

const std::string &Example::getSimplified(void) const
{
    return _simplified;
}

void Example::setSimplified(std::string simplified)
{
    _simplified = simplified;
}

const std::string &Example::getTraditional(void) const
{
    return _traditional;
}

void Example::setTraditional(std::string traditional)
{
    _traditional = traditional;
}

bool Example::generatePhonetic(CantoneseOptions cantoneseOptions,
                                      MandarinOptions mandarinOptions)
{
    if ((cantoneseOptions & CantoneseOptions::PRETTY_YALE)
            == CantoneseOptions::PRETTY_YALE
        && !_yale.has_value()) {
        _yale = CantoneseUtils::convertJyutpingToYale(
            _jyutping,
            /* useSpacesToSegment */ true);
    }

    if ((cantoneseOptions & CantoneseOptions::CANTONESE_IPA)
            == CantoneseOptions::CANTONESE_IPA
        && !_cantoneseIPA.has_value()) {
        _cantoneseIPA
            = CantoneseUtils::convertJyutpingToIPA(_jyutping,
                                                   /* useSpacesToSegment */ true);
    }

    if ((mandarinOptions & MandarinOptions::PRETTY_PINYIN)
            == MandarinOptions::PRETTY_PINYIN
        && !_prettyPinyin.has_value()) {
        _prettyPinyin = MandarinUtils::createPrettyPinyin(_pinyin);
    }

    if ((mandarinOptions & MandarinOptions::NUMBERED_PINYIN)
            == MandarinOptions::NUMBERED_PINYIN
        && !_numberedPinyin.has_value()) {
        _numberedPinyin = MandarinUtils::createNumberedPinyin(_pinyin);
    }

    if ((mandarinOptions & MandarinOptions::ZHUYIN) == MandarinOptions::ZHUYIN
        && !_zhuyin.has_value()) {
        _zhuyin
            = MandarinUtils::convertPinyinToZhuyin(_pinyin,
                                                   /* useSpacesToSegment */ true);
    }

    if ((mandarinOptions & MandarinOptions::MANDARIN_IPA)
            == MandarinOptions::MANDARIN_IPA
        && !_mandarinIPA.has_value()) {
        _mandarinIPA
            = MandarinUtils::convertPinyinToIPA(_pinyin,
                                                /* useSpacesToSegment */ true);
    }

    return true;

}

const std::string &Example::getPhonetic(
    EntryPhoneticOptions options,
    CantoneseOptions cantoneseOptions,
    MandarinOptions mandarinOptions) const
{
    switch (options) {
    case EntryPhoneticOptions::ONLY_CANTONESE:
    case EntryPhoneticOptions::PREFER_CANTONESE:
        return getCantonesePhonetic(cantoneseOptions);
    case EntryPhoneticOptions::ONLY_MANDARIN:
    case EntryPhoneticOptions::PREFER_MANDARIN:
        return getMandarinPhonetic(mandarinOptions);
    }
    return getCantonesePhonetic(cantoneseOptions);
}

const std::string &Example::getCantonesePhonetic(
    CantoneseOptions cantoneseOptions) const
{
    switch (cantoneseOptions) {
    case CantoneseOptions::PRETTY_YALE: {
        return _yale.value();
    }
    case CantoneseOptions::CANTONESE_IPA: {
        return _cantoneseIPA.value();
    }
    case CantoneseOptions::RAW_JYUTPING:
    default:
        return _jyutping;
    }
}

const std::string &Example::getMandarinPhonetic(
    MandarinOptions mandarinOptions) const
{
    switch (mandarinOptions) {
    case MandarinOptions::PRETTY_PINYIN: {
        return _prettyPinyin.value();
    }
    case MandarinOptions::NUMBERED_PINYIN: {
        return _numberedPinyin.value();
    }
    case MandarinOptions::ZHUYIN: {
        return _zhuyin.value();
    }
    case MandarinOptions::MANDARIN_IPA: {
        return _mandarinIPA.value();
    }
    default: {
        return _pinyin;
    }
    }
}

const std::string &Example::getJyutping(void) const
{
    return _jyutping;
}

void Example::setJyutping(const std::string &jyutping)
{
    _jyutping = jyutping;
}

const std::string &Example::getPinyin(void) const
{
    return _pinyin;
}

const std::string &Example::getPrettyPinyin(void) const
{
    return _prettyPinyin.value();
}

void Example::setPinyin(const std::string &pinyin)
{
    _pinyin = pinyin;
}

std::span<const TranslationSet> Example::getTranslationSets(void) const
{
    return _translationSets;
}

std::string Example::getTranslationSnippet(void) const
{
    if (_translationSets.empty()) {
        return "";
    }

    TranslationSet translationSet = _translationSets.at(0);

    if (translationSet.getTranslationSnippet().empty()) {
        return "";
    }

    std::vector<Translation::Translation>
        snippets{translationSet.getTranslationSnippet().begin(),
                 translationSet.getTranslationSnippet().end()};

    if (snippets.empty()) {
        return "";
    }

    return snippets.at(0).content;
}

std::string Example::getTranslationSnippetLanguage(void) const
{
    if (_translationSets.empty()) {
        return "";
    }

    TranslationSet translationSet = _translationSets.at(0);

    if (translationSet.getTranslationSnippet().empty()) {
        return "";
    }

    std::vector<Translation::Translation>
        snippets{translationSet.getTranslationSnippet().begin(),
                 translationSet.getTranslationSnippet().end()};

    if (snippets.empty()) {
        return "";
    }

    return snippets.at(0).language;
}

void Example::setIsWelcome(const bool isWelcome)
{
    _isWelcome = isWelcome;
}

bool Example::isWelcome(void) const
{
    return _isWelcome;
}

void Example::setIsEmpty(const bool isEmpty)
{
    _isEmpty = isEmpty;
}

bool Example::isEmpty(void) const
{
    return _isEmpty;
}
