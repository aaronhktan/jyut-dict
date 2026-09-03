#include "entry.h"

#include "logic/settings/settings.h"
#include "logic/utils/cantoneseutils.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/mandarinutils.h"

#include <QVariant>

Entry::Entry(const std::string &simplified, const std::string &traditional,
             const std::string &jyutping, const std::string &pinyin,
             const std::vector<DefinitionsSet> &definitions)
    : _simplified{simplified},
      _traditional{traditional},
      _jyutping{jyutping},
      _pinyin{pinyin},
      _definitions{definitions}
{
    // Normalize pinyin and jyutping to lowercase >:(
    std::transform(_jyutping.cbegin(),
                   _jyutping.cend(),
                   _jyutping.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::transform(_pinyin.cbegin(),
                   _pinyin.cend(),
                   _pinyin.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // Create comparison versions of traditional and simplified entries
    _traditionalDifference = ChineseUtils::compareStrings(_simplified, _traditional);
    _simplifiedDifference = ChineseUtils::compareStrings(_traditional, _simplified);
    _preferSimplified = _simplified + " [" + _traditionalDifference + "]";
    _preferTraditional = _traditional + " [" + _simplifiedDifference + "]";
}

Entry::Entry(const Entry &entry)
    : QObject()
    , _simplified{entry._simplified}
    , _simplifiedDifference{entry._simplifiedDifference}
    , _preferSimplified{entry._preferSimplified}
    , _traditional{entry._traditional}
    , _traditionalDifference{entry._traditionalDifference}
    , _preferTraditional{entry._preferTraditional}
    , _colouredSimplified{entry._colouredSimplified}
    , _colouredSimplifiedDifference{entry._colouredSimplifiedDifference}
    , _colouredPreferSimplified{entry._colouredPreferSimplified}
    , _colouredTraditional{entry._colouredTraditional}
    , _colouredTraditionalDifference{entry._colouredTraditionalDifference}
    , _colouredPreferTraditional{entry._colouredPreferTraditional}
    , _jyutping{entry._jyutping}
    , _yale{entry._yale}
    , _cantoneseIPA{entry._cantoneseIPA}
    , _jyutpingNumbers{entry._jyutpingNumbers}
    , _pinyin{entry._pinyin}
    , _prettyPinyin{entry._prettyPinyin}
    , _numberedPinyin{entry._numberedPinyin}
    , _zhuyin{entry._zhuyin}
    , _mandarinIPA{entry._mandarinIPA}
    , _pinyinNumbers{entry._pinyinNumbers}
    , _definitions{entry._definitions}
    , _isWelcome{entry._isWelcome}
    , _isEmpty{entry._isEmpty}
{}

Entry::Entry(Entry &&entry)
    : _simplified{std::move(entry._simplified)}
    , _simplifiedDifference{std::move(entry._simplifiedDifference)}
    , _preferSimplified{std::move(entry._preferSimplified)}
    , _traditional{std::move(entry._traditional)}
    , _traditionalDifference{std::move(entry._traditionalDifference)}
    , _preferTraditional{std::move(entry._preferTraditional)}
    , _colouredSimplified{std::move(entry._colouredSimplified)}
    , _colouredSimplifiedDifference{std::move(
          entry._colouredSimplifiedDifference)}
    , _colouredPreferSimplified{std::move(entry._colouredPreferSimplified)}
    , _colouredTraditional{std::move(entry._colouredTraditional)}
    , _colouredTraditionalDifference{std::move(
          entry._colouredTraditionalDifference)}
    , _colouredPreferTraditional{std::move(entry._colouredPreferTraditional)}
    , _jyutping{std::move(entry._jyutping)}
    , _yale{std::move(entry._yale)}
    , _cantoneseIPA{std::move(entry._cantoneseIPA)}
    , _jyutpingNumbers{std::move(entry._jyutpingNumbers)}
    , _pinyin{std::move(entry._pinyin)}
    , _prettyPinyin{std::move(entry._prettyPinyin)}
    , _numberedPinyin{std::move(entry._numberedPinyin)}
    , _zhuyin{std::move(entry._zhuyin)}
    , _mandarinIPA{std::move(entry._mandarinIPA)}
    , _pinyinNumbers{std::move(entry._pinyinNumbers)}
    , _definitions{std::move(entry._definitions)}
    , _isWelcome{entry._isWelcome}
    , _isEmpty{entry._isEmpty}
{}

Entry &Entry::operator=(const Entry &entry)
{
    if (&entry == this) {
        return *this;
    }

    _simplified = entry._simplified;
    _simplifiedDifference = entry._simplifiedDifference;
    _preferSimplified = entry._preferSimplified;
    _traditional = entry._traditional;
    _traditionalDifference = entry._traditionalDifference;
    _preferTraditional = entry._preferTraditional;
    _colouredSimplified = entry._colouredSimplified;
    _colouredSimplifiedDifference = entry._colouredSimplifiedDifference;
    _colouredPreferSimplified = entry._colouredPreferSimplified;
    _colouredTraditional = entry._colouredTraditional;
    _colouredTraditionalDifference = entry._colouredTraditionalDifference;
    _colouredPreferTraditional = entry._colouredPreferTraditional;
    _jyutping = entry._jyutping;
    _yale = entry._yale;
    _cantoneseIPA = entry._cantoneseIPA;
    _jyutpingNumbers = entry._jyutpingNumbers;
    _pinyin = entry._pinyin;
    _prettyPinyin = entry._prettyPinyin;
    _numberedPinyin = entry._numberedPinyin;
    _zhuyin = entry._zhuyin;
    _mandarinIPA = entry._mandarinIPA;
    _pinyinNumbers = entry._pinyinNumbers;
    _definitions = entry._definitions;
    _isWelcome = entry._isWelcome;
    _isEmpty = entry._isEmpty;

    return *this;
}

Entry &Entry::operator=(Entry &&entry)
{
    if (&entry == this) {
        return *this;
    }

    _simplified = std::move(entry._simplified);
    _simplifiedDifference = std::move(entry._simplifiedDifference);
    _preferSimplified = std::move(entry._preferSimplified);
    _traditional = std::move(entry._traditional);
    _traditionalDifference = std::move(entry._traditionalDifference);
    _preferTraditional = std::move(entry._preferTraditional);
    _colouredSimplified = std::move(entry._colouredSimplified);
    _colouredSimplifiedDifference = std::move(entry._colouredSimplifiedDifference);
    _colouredPreferSimplified = std::move(entry._colouredPreferSimplified);
    _colouredTraditional = std::move(entry._colouredTraditional);
    _colouredTraditionalDifference = std::move(entry._colouredTraditionalDifference);
    _colouredPreferTraditional = std::move(entry._colouredPreferTraditional);
    _jyutping = std::move(entry._jyutping);
    _yale = std::move(entry._yale);
    _cantoneseIPA = std::move(entry._cantoneseIPA);
    _jyutpingNumbers = std::move(entry._jyutpingNumbers);
    _pinyin = std::move(entry._pinyin);
    _prettyPinyin = std::move(entry._prettyPinyin);
    _numberedPinyin = std::move(entry._numberedPinyin);
    _zhuyin = std::move(entry._zhuyin);
    _mandarinIPA = std::move(entry._mandarinIPA);
    _pinyinNumbers = std::move(entry._pinyinNumbers);
    _definitions = std::move(entry._definitions);
    _isWelcome = entry._isWelcome;
    _isEmpty = entry._isEmpty;

    return *this;
}

bool Entry::operator==(const Entry &other) const
{
    if (&other == this) {
        return true;
    }

    if (other._simplified == _simplified && other._traditional == _traditional
        && other._jyutping == _jyutping && other._pinyin == _pinyin
        && other._definitions == _definitions) {
        return true;
    }

    return false;
}

bool Entry::operator!=(const Entry &other) const
{
    return !(*this == other);
}

std::ostream &operator<<(std::ostream &out, const Entry &entry)
{
    out << "Simplified: " << entry.getSimplified() << "\n";
    out << "Traditional: " << entry.getTraditional() << "\n";
    out << "Jyutping: " << entry.getJyutping() << "\n";
    out << "Pinyin: " << entry.getPinyin() << "\n";
    for (size_t i = 0; i < entry.getDefinitionsSets().size(); i++) {
        out << entry.getDefinitionsSets()[i] << "\n";
    }
    return out;
}

const std::string &Entry::getCharacters(EntryCharactersOptions options,
                                        bool useColours) const
{
    switch (options) {
        case (EntryCharactersOptions::ONLY_SIMPLIFIED): {
            if (useColours) {
                return _colouredSimplified;
            }
            return _simplified;
        }
        case (EntryCharactersOptions::ONLY_TRADITIONAL): {
            if (useColours) {
                return _colouredTraditional;
            }
            return _traditional;
        }
        case (EntryCharactersOptions::PREFER_SIMPLIFIED): {
            if (useColours) {
                return _colouredPreferSimplified;
            }
            return _preferSimplified;
        }
        case (EntryCharactersOptions::PREFER_TRADITIONAL): {
            if (useColours) {
                return _colouredPreferTraditional;
            }
            return _preferTraditional;
        }
    }

    return _simplified;
}

const std::string &Entry::getCharactersNoSecondary(
    EntryCharactersOptions options, bool useColours) const
{
    switch (options) {
        case (EntryCharactersOptions::PREFER_SIMPLIFIED):
        case (EntryCharactersOptions::ONLY_SIMPLIFIED):
            if (useColours) {
                return _colouredSimplified;
            }
            return _simplified;
        case (EntryCharactersOptions::PREFER_TRADITIONAL):
        case (EntryCharactersOptions::ONLY_TRADITIONAL):
            if (useColours) {
                return _colouredTraditional;
            }
            return _traditional;
    }

    return _simplified;
}

const std::string &Entry::getSimplified(void) const
{
    return _simplified;
}

void Entry::setSimplified(const std::string &simplified)
{
    _simplified = simplified;

    // Create comparison versions of traditional and simplified entries
    _traditionalDifference = ChineseUtils::compareStrings(_simplified, _traditional);
    _simplifiedDifference = ChineseUtils::compareStrings(_traditional, _simplified);
    _preferSimplified = _simplified + " [" + _traditionalDifference + "]";
    _preferTraditional = _traditional + " [" + _simplifiedDifference + "]";
}

const std::string &Entry::getTraditional(void) const
{
    return _traditional;
}

void Entry::setTraditional(const std::string &traditional)
{
    _traditional = traditional;

    // Create comparison versions of traditional and simplified entries
    _traditionalDifference = ChineseUtils::compareStrings(_simplified, _traditional);
    _simplifiedDifference = ChineseUtils::compareStrings(_traditional, _simplified);
    _preferTraditional = _traditional + " [" + _simplifiedDifference + "]";
    _preferSimplified = _simplified + " [" + _traditionalDifference + "]";
}

bool Entry::generatePhonetic(CantoneseOptions cantoneseOptions,
                             MandarinOptions mandarinOptions)
{
    if ((cantoneseOptions & CantoneseOptions::PRETTY_YALE)
            == CantoneseOptions::PRETTY_YALE
        && !_yale.has_value()) {
        _yale = CantoneseUtils::convertJyutpingToYale(_jyutping);
    }

    if ((cantoneseOptions & CantoneseOptions::CANTONESE_IPA)
            == CantoneseOptions::CANTONESE_IPA
        && !_cantoneseIPA.has_value()) {
        _cantoneseIPA = CantoneseUtils::convertJyutpingToIPA(_jyutping);
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
        _zhuyin = MandarinUtils::convertPinyinToZhuyin(_pinyin);
    }

    if ((mandarinOptions & MandarinOptions::MANDARIN_IPA)
            == MandarinOptions::MANDARIN_IPA
        && !_mandarinIPA.has_value()) {
        _mandarinIPA = MandarinUtils::convertPinyinToIPA(_pinyin);
    }

    return true;
}

bool Entry::generateDefinitionsPhonetic(CantoneseOptions cantoneseOptions,
                                        MandarinOptions mandarinOptions)
{
    for (auto &definitionsSet : _definitions) {
        definitionsSet.generatePhonetic(cantoneseOptions, mandarinOptions);
    }

    return true;
}

std::string Entry::getPhonetic(EntryPhoneticOptions options) const
{
    return getPhonetic(options, CantoneseOptions::RAW_JYUTPING, MandarinOptions::NUMBERED_PINYIN);
}

std::string Entry::getPhonetic(EntryPhoneticOptions options, MandarinOptions mandarinOptions) const
{
    return getPhonetic(options, CantoneseOptions::RAW_JYUTPING, mandarinOptions);
}

std::string Entry::getPhonetic(EntryPhoneticOptions options,
                               CantoneseOptions cantoneseOptions) const
{
    return getPhonetic(options, cantoneseOptions, MandarinOptions::NUMBERED_PINYIN);
}

std::string Entry::getPhonetic(EntryPhoneticOptions options,
                               CantoneseOptions cantoneseOptions,
                               MandarinOptions mandarinOptions) const
{
    switch (options) {
        case EntryPhoneticOptions::ONLY_CANTONESE: {
            return getCantonesePhonetic(cantoneseOptions);
        }
        case EntryPhoneticOptions::ONLY_MANDARIN: {
            return getMandarinPhonetic(mandarinOptions);
        }
        case EntryPhoneticOptions::PREFER_CANTONESE: {
            const std::string &cantonese = getCantonesePhonetic(
                cantoneseOptions);
            const std::string &mandarin = getMandarinPhonetic(mandarinOptions);
            if (cantonese.empty()) {
                return mandarin.empty() ? "" : "(" + mandarin + ")";
            } else if (mandarin.empty()) {
                return cantonese;
            } else {
                return cantonese + " (" + mandarin + ")";
            }
        }
        case EntryPhoneticOptions::PREFER_MANDARIN: {
            const std::string &cantonese = getCantonesePhonetic(
                cantoneseOptions);
            const std::string &mandarin = getMandarinPhonetic(mandarinOptions);
            if (mandarin.empty()) {
                return cantonese.empty() ? "" : "(" + cantonese + ")";
            } else if (cantonese.empty()) {
                return mandarin;
            } else {
                return mandarin + " (" + cantonese + ")";
            }
        }
    }
    return _jyutping;
}

const std::string &Entry::getCantonesePhonetic(
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

const std::string &Entry::getMandarinPhonetic(
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

const std::string &Entry::getJyutping(void) const
{
    return _jyutping;
}

void Entry::setJyutping(const std::string &jyutping)
{
    _jyutping = jyutping;
    std::transform(_jyutping.cbegin(),
                   _jyutping.cend(),
                   _jyutping.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    _jyutpingNumbers = std::nullopt;
}

const std::vector<uint8_t> &Entry::getJyutpingNumbers()
{
    if (!_jyutpingNumbers.has_value()) {
        _jyutpingNumbers = std::vector<uint8_t>{};
        size_t pos = _jyutping.find_first_of("0123456");
        while (pos != std::string::npos) {
            _jyutpingNumbers.value().push_back(_jyutping.at(pos) - '0');
            pos = _jyutping.find_first_of("0123456", pos + 1);
        }
    }

    return _jyutpingNumbers.value();
}

const std::string &Entry::getPinyin(void) const
{
    return _pinyin;
}

void Entry::setPinyin(const std::string &pinyin)
{
    _pinyin = pinyin;
    std::transform(_pinyin.begin(), _pinyin.end(), _pinyin.begin(), ::tolower);
    _pinyinNumbers = std::nullopt;
}

const std::vector<uint8_t> &Entry::getPinyinNumbers()
{
    if (!_pinyinNumbers.has_value()) {
        _pinyinNumbers = std::vector<uint8_t>{};
        size_t pos = _pinyin.find_first_of("012345");
        while (pos != std::string::npos) {
            _pinyinNumbers.value().push_back(_pinyin.at(pos) - '0');
            pos = _pinyin.find_first_of("012345", pos + 1);
        }
    }

    return _pinyinNumbers.value();
}

std::span<const DefinitionsSet> Entry::getDefinitionsSets(void) const
{
    return _definitions;
}

const std::string &Entry::getDefinitionSnippet(void)
{
    if (_definitions.empty()) {
        return _definitionSnippet;
    }

    if (_definitionSnippet.empty()) {
        for (const auto &definition : _definitions) {
            if (!definition.isEmpty()) {
                _definitionSnippet = definition.getDefinitionsSnippet();
                break;
            }
        }
    }

    return _definitionSnippet;
}

void Entry::addDefinitions(const std::string &source,
                           const std::vector<Definition::Definition> &definitions)
{
    _definitions.push_back(DefinitionsSet{source, definitions});
}

void Entry::refreshColours(const EntryColourPhoneticType type)
{
    std::vector<uint8_t> tones;
    switch (type) {
    case EntryColourPhoneticType::NONE: {
        _colouredSimplified = _simplified;
        _colouredTraditional = _traditional;
        _colouredSimplifiedDifference = _simplifiedDifference;
        _colouredTraditionalDifference = _traditionalDifference;
        _colouredPreferSimplified = _colouredSimplified + " ["
                                    + _colouredTraditionalDifference + "]";
        _colouredPreferTraditional = _colouredTraditional + " ["
                                     + _colouredSimplifiedDifference + "]";
        return;
    }
    case EntryColourPhoneticType::CANTONESE: {
        tones = getJyutpingNumbers();
        break;
    }
    case EntryColourPhoneticType::MANDARIN: {
        tones = getPinyinNumbers();
        break;
    }
    }

    // Create coloured versions of Simplified and Traditional characters
    _colouredSimplified
        = ChineseUtils::applyColours(_simplified,
                                     tones,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours,
                                     type);
    _colouredTraditional
        = ChineseUtils::applyColours(_traditional,
                                     tones,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours,
                                     type);
    _colouredSimplifiedDifference
        = ChineseUtils::applyColours(_simplifiedDifference,
                                     tones,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours,
                                     type);
    _colouredTraditionalDifference
        = ChineseUtils::applyColours(_traditionalDifference,
                                     tones,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours,
                                     type);
    _colouredPreferSimplified = _colouredSimplified + " ["
                                + _colouredTraditionalDifference + "]";
    _colouredPreferTraditional = _colouredPreferTraditional
        = _colouredTraditional + " [" + _colouredSimplifiedDifference + "]";
}

void Entry::setIsWelcome(const bool isWelcome)
{
    _isWelcome = isWelcome;
}

bool Entry::isWelcome(void) const
{
    return _isWelcome;
}

void Entry::setIsEmpty(const bool isEmpty)
{
    _isEmpty = isEmpty;
}

bool Entry::isEmpty(void) const
{
    return _isEmpty;
}
