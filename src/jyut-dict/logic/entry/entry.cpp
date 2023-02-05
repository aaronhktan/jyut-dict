#include "entry.h"

#include "logic/settings/settings.h"
#include "logic/utils/chineseutils.h"

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
}

Entry::Entry(const Entry &entry)
    : QObject(), _simplified{entry._simplified},
      _simplifiedDifference{entry._simplifiedDifference},
      _traditional{entry._traditional},
      _traditionalDifference{entry._traditionalDifference},
      _colouredSimplified{entry._colouredSimplified},
      _colouredSimplifiedDifference{entry._colouredSimplifiedDifference},
      _colouredTraditional{entry._colouredTraditional},
      _colouredTraditionalDifference{entry._colouredTraditionalDifference},
      _jyutping{entry._jyutping}, _yale{entry._yale},
      _isYaleValid{entry._isYaleValid}, _cantoneseIPA{entry._cantoneseIPA},
      _isCantoneseIPAValid{entry._isCantoneseIPAValid}, _pinyin{entry._pinyin},
      _prettyPinyin{entry._prettyPinyin},
      _isPrettyPinyinValid{entry._isPrettyPinyinValid},
      _numberedPinyin{entry._numberedPinyin},
      _isNumberedPinyinValid{entry._isNumberedPinyinValid},
      _zhuyin{entry._zhuyin}, _isZhuyinValid{entry._isZhuyinValid},
      _mandarinIPA{entry._mandarinIPA},
      _isMandarinIPAValid{entry._isMandarinIPAValid},
      _definitions{entry._definitions},
      _isWelcome{entry._isWelcome}, _isEmpty{entry._isEmpty}
{}

Entry::Entry(Entry &&entry)
    : _simplified{std::move(entry._simplified)},
      _simplifiedDifference{std::move(entry._simplifiedDifference)},
      _traditional{std::move(entry._traditional)},
      _traditionalDifference{std::move(entry._traditionalDifference)},
      _colouredSimplified{std::move(entry._colouredSimplified)},
      _colouredSimplifiedDifference{
          std::move(entry._colouredSimplifiedDifference)},
      _colouredTraditional{std::move(entry._colouredTraditional)},
      _colouredTraditionalDifference{
          std::move(entry._colouredTraditionalDifference)},
      _jyutping{std::move(entry._jyutping)}, _yale{std::move(entry._yale)},
      _isYaleValid{entry._isYaleValid}, _cantoneseIPA{std::move(
                                            entry._cantoneseIPA)},
      _isCantoneseIPAValid{entry._isCantoneseIPAValid}, _pinyin{std::move(
                                                            entry._pinyin)},
      _prettyPinyin{std::move(entry._prettyPinyin)},
      _isPrettyPinyinValid{entry._isPrettyPinyinValid},
      _numberedPinyin{std::move(entry._numberedPinyin)},
      _isNumberedPinyinValid{entry._isNumberedPinyinValid}, _zhuyin{std::move(
                                                                entry._zhuyin)},
      _isZhuyinValid{entry._isZhuyinValid}, _mandarinIPA{std::move(
                                                entry._mandarinIPA)},
      _isMandarinIPAValid{entry._isMandarinIPAValid}, _definitions{std::move(
                                                          entry._definitions)},
      _isWelcome{entry._isWelcome}, _isEmpty{entry._isEmpty}
{}

Entry &Entry::operator=(const Entry &entry)
{
    if (&entry == this) {
        return *this;
    }

    _simplified = entry._simplified;
    _simplifiedDifference = entry._simplifiedDifference;
    _traditional = entry._traditional;
    _traditionalDifference = entry._traditionalDifference;
    _colouredSimplified = entry._colouredSimplified;
    _colouredSimplifiedDifference = entry._colouredSimplifiedDifference;
    _colouredTraditional = entry._colouredTraditional;
    _colouredTraditionalDifference = entry._colouredTraditionalDifference;
    _jyutping = entry._jyutping;
    _yale = entry._yale;
    _isYaleValid = entry._isYaleValid;
    _cantoneseIPA = entry._cantoneseIPA;
    _isCantoneseIPAValid = entry._isCantoneseIPAValid;
    _pinyin = entry._pinyin;
    _prettyPinyin = entry._prettyPinyin;
    _isPrettyPinyinValid = entry._isPrettyPinyinValid;
    _numberedPinyin = entry._numberedPinyin;
    _isNumberedPinyinValid = entry._isNumberedPinyinValid;
    _zhuyin = entry._zhuyin;
    _isZhuyinValid = entry._isZhuyinValid;
    _mandarinIPA = entry._mandarinIPA;
    _isMandarinIPAValid = entry._isMandarinIPAValid;
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
    _traditional = std::move(entry._traditional);
    _traditionalDifference = std::move(entry._traditionalDifference);
    _colouredSimplified = std::move(entry._colouredSimplified);
    _colouredSimplifiedDifference = std::move(entry._colouredSimplifiedDifference);
    _colouredTraditional = std::move(entry._colouredTraditional);
    _colouredTraditionalDifference = std::move(entry._colouredTraditionalDifference);
    _jyutping = std::move(entry._jyutping);
    _yale = std::move(entry._yale);
    _isYaleValid = entry._isYaleValid;
    _cantoneseIPA = std::move(entry._cantoneseIPA);
    _isCantoneseIPAValid = entry._isCantoneseIPAValid;
    _pinyin = std::move(entry._pinyin);
    _prettyPinyin = std::move(entry._prettyPinyin);
    _isPrettyPinyinValid = entry._isPrettyPinyinValid;
    _numberedPinyin = std::move(entry._numberedPinyin);
    _isNumberedPinyinValid = entry._isNumberedPinyinValid;
    _zhuyin = std::move(entry._zhuyin);
    _isZhuyinValid = entry._isZhuyinValid;
    _mandarinIPA = std::move(entry._mandarinIPA);
    _isMandarinIPAValid = entry._isMandarinIPAValid;
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
        && other._jyutping == _jyutping && other._pinyin == _pinyin) {
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

std::string Entry::getCharacters(EntryCharactersOptions options, bool use_colours) const
{
    switch (options) {
        case (EntryCharactersOptions::ONLY_SIMPLIFIED): {
            if (use_colours) {
                return _colouredSimplified;
            }
            return _simplified;
        }
        case (EntryCharactersOptions::ONLY_TRADITIONAL): {
            if (use_colours) {
                return _colouredTraditional;
            }
            return _traditional;
        }
        case (EntryCharactersOptions::PREFER_SIMPLIFIED): {
            if (use_colours) {
                return _colouredSimplified + " [" + _colouredTraditionalDifference + "]";
            }
            return _simplified + " [" + _traditionalDifference + "]";
        }
        case (EntryCharactersOptions::PREFER_TRADITIONAL): {
            if (use_colours) {
                return _colouredTraditional + " [" + _colouredSimplifiedDifference + "]";
            }
            return _traditional + " [" + _simplifiedDifference + "]";
        }
    }

    return _simplified;
}

std::string Entry::getCharactersNoSecondary(EntryCharactersOptions options, bool use_colours) const
{
    switch (options) {
        case (EntryCharactersOptions::PREFER_SIMPLIFIED):
        case (EntryCharactersOptions::ONLY_SIMPLIFIED):
            if (use_colours) {
                return _colouredSimplified;
            }
            return _simplified;
        case (EntryCharactersOptions::PREFER_TRADITIONAL):
        case (EntryCharactersOptions::ONLY_TRADITIONAL):
            if (use_colours) {
                return _colouredTraditional;
            }
            return _traditional;
    }

    return _simplified;
}

std::string Entry::getSimplified(void) const
{
    return _simplified;
}

void Entry::setSimplified(std::string simplified)
{
    _simplified = simplified;

    // Create comparison versions of traditional and simplified entries
    _traditionalDifference = ChineseUtils::compareStrings(_simplified, _traditional);
    _simplifiedDifference = ChineseUtils::compareStrings(_traditional, _simplified);
}

std::string Entry::getTraditional(void) const
{
    return _traditional;
}

void Entry::setTraditional(std::string traditional)
{
    _traditional = traditional;

    // Create comparison versions of traditional and simplified entries
    _traditionalDifference = ChineseUtils::compareStrings(_simplified, _traditional);
    _simplifiedDifference = ChineseUtils::compareStrings(_traditional, _simplified);
}

bool Entry::generatePhonetic(CantoneseOptions cantoneseOptions,
                             MandarinOptions mandarinOptions)
{
    if ((cantoneseOptions & CantoneseOptions::PRETTY_YALE)
            == CantoneseOptions::PRETTY_YALE
        && !_isYaleValid) {
            _yale = ChineseUtils::convertJyutpingToYale(_jyutping);
            _isYaleValid = true;
    }

    if ((cantoneseOptions & CantoneseOptions::CANTONESE_IPA)
            == CantoneseOptions::CANTONESE_IPA
        && !_isCantoneseIPAValid) {
            _cantoneseIPA = ChineseUtils::convertJyutpingToIPA(_jyutping);
            _isCantoneseIPAValid = true;
    }

    if ((mandarinOptions & MandarinOptions::PRETTY_PINYIN)
            == MandarinOptions::PRETTY_PINYIN && !_isPrettyPinyinValid) {
        _prettyPinyin = ChineseUtils::createPrettyPinyin(_pinyin);
        _isPrettyPinyinValid = true;
    }

    if ((mandarinOptions & MandarinOptions::NUMBERED_PINYIN)
            == MandarinOptions::NUMBERED_PINYIN && !_isNumberedPinyinValid) {
        _numberedPinyin = ChineseUtils::createNumberedPinyin(_pinyin);
        _isNumberedPinyinValid = true;
    }

    if ((mandarinOptions & MandarinOptions::ZHUYIN) == MandarinOptions::ZHUYIN
        && !_isZhuyinValid) {
        _zhuyin = ChineseUtils::convertPinyinToZhuyin(_pinyin);
        _isZhuyinValid = true;
    }

    if ((mandarinOptions & MandarinOptions::MANDARIN_IPA)
            == MandarinOptions::MANDARIN_IPA
        && !_isMandarinIPAValid) {
        _mandarinIPA = ChineseUtils::convertPinyinToIPA(_pinyin);
        _isMandarinIPAValid = true;
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
            std::string cantonese = getCantonesePhonetic(cantoneseOptions);
            std::string mandarin = getMandarinPhonetic(mandarinOptions);
            if (cantonese.empty()) {
                return mandarin.empty() ? "" : "(" + mandarin + ")";
            } else if (mandarin.empty()) {
                return cantonese;
            } else {
                return cantonese + " (" + mandarin + ")";
            }
        }
        case EntryPhoneticOptions::PREFER_MANDARIN: {
            std::string cantonese = getCantonesePhonetic(cantoneseOptions);
            std::string mandarin = getMandarinPhonetic(mandarinOptions);
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

std::string Entry::getCantonesePhonetic(CantoneseOptions cantoneseOptions) const
{
    switch (cantoneseOptions) {
    case CantoneseOptions::PRETTY_YALE: {
        return _yale;
    }
    case CantoneseOptions::CANTONESE_IPA: {
        return _cantoneseIPA;
    }
    case CantoneseOptions::RAW_JYUTPING:
    default:
        return _jyutping;
    }
}

std::string Entry::getMandarinPhonetic(MandarinOptions mandarinOptions) const
{
    switch (mandarinOptions) {
    case MandarinOptions::PRETTY_PINYIN: {
        return _prettyPinyin;
    }
    case MandarinOptions::NUMBERED_PINYIN: {
        return _numberedPinyin;
    }
    case MandarinOptions::ZHUYIN: {
        return _zhuyin;
    }
    case MandarinOptions::MANDARIN_IPA: {
        return _mandarinIPA;
    }
    default: {
        return _pinyin;
    }
    }
}

std::string Entry::getJyutping(void) const
{
    return _jyutping;
}

std::string Entry::getYale(void) const
{
    return _yale;
}

void Entry::setJyutping(const std::string &jyutping)
{
    _jyutping = jyutping;
    std::transform(_jyutping.cbegin(),
                   _jyutping.cend(),
                   _jyutping.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}

std::vector<int> Entry::getJyutpingNumbers() const
{
    std::vector<int> jyutpingNumbers;

    if (_jyutping.empty()) {
        return getPinyinNumbers();
    }

    size_t pos = _jyutping.find_first_of("0123456");
    while(pos != std::string::npos) {
        jyutpingNumbers.push_back(_jyutping.at(pos) - '0');
        pos = _jyutping.find_first_of("0123456", pos + 1);
    }

return jyutpingNumbers;
}

std::string Entry::getPinyin(void) const
{
    return _pinyin;
}

std::string Entry::getPrettyPinyin(void) const
{
    return _prettyPinyin;
}

std::string Entry::getNumberedPinyin(void) const
{
    return _numberedPinyin;
}

void Entry::setPinyin(const std::string &pinyin)
{
    _pinyin = pinyin;
    std::transform(_pinyin.begin(), _pinyin.end(), _pinyin.begin(), ::tolower);
}

std::vector<int> Entry::getPinyinNumbers() const
{
    std::vector<int> pinyinNumbers;

    size_t pos = _pinyin.find_first_of("012345");
    while(pos != std::string::npos) {
        pinyinNumbers.push_back(_pinyin.at(pos) - '0');
        pos = _pinyin.find_first_of("012345", pos + 1);
    }

    return pinyinNumbers;
}

std::vector<DefinitionsSet> Entry::getDefinitionsSets(void) const
{
    return _definitions;
}

std::string Entry::getDefinitionSnippet(void) const
{
    if (_definitions.empty()) {
        return "";
    }

    for (const auto &definition: _definitions) {
        if (!definition.isEmpty()) {
            return definition.getDefinitionsSnippet();
        }
    }

    return "";
}

void Entry::addDefinitions(const std::string &source,
                           const std::vector<Definition::Definition> &definitions)
{
    _definitions.push_back(DefinitionsSet{source, definitions});
}

void Entry::refreshColours(const EntryColourPhoneticType type)
{
    std::vector<int> tones;
    switch (type) {
    case EntryColourPhoneticType::NONE: {
        _colouredSimplified = _simplified;
        _colouredTraditional = _traditional;
        _colouredSimplifiedDifference = _simplifiedDifference;
        _colouredTraditionalDifference = _traditionalDifference;
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
