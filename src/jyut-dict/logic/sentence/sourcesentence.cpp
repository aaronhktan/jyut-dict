#include "sourcesentence.h"

#include <sstream>
#include <unordered_map>
#include <unordered_set>

static std::unordered_map<std::string, std::vector<std::string>> replacementMap = {
    {"a", {"ā", "á", "ǎ", "à", "a"}},
    {"e", {"ē", "é", "ě", "è", "e"}},
    {"i", {"ī", "í", "ǐ", "ì", "i"}},
    {"o", {"ō", "ó", "ǒ", "ò", "o"}},
    {"u", {"ū", "ú", "ǔ", "ù", "u"}},
    {"ü", {"ǖ", "ǘ", "ǚ", "ǜ", "ü"}},
    };

static std::unordered_set<std::string> specialCharacters = {
    "，", "%", "－", "…", "·",
    };

SourceSentence::SourceSentence()
{
}

SourceSentence::SourceSentence(std::string sourceLanguage,
                               std::string simplified,
                               std::string traditional,
                               std::string jyutping,
                               std::string pinyin,
                               std::vector<SentenceSet> sentences)
    : _sourceLanguage{sourceLanguage}
    , _simplified{simplified}
    , _traditional{traditional}
    , _jyutping{jyutping}
    , _pinyin{pinyin}
    , _sentences{sentences}
{
    // Create pretty pinyin
    _prettyPinyin = createPrettyPinyin();
}

SourceSentence::SourceSentence(const SourceSentence &sourceSentence)
    : _sourceLanguage{std::move(sourceSentence._sourceLanguage)}
    , _simplified{sourceSentence.getSimplified()}
    , _traditional{sourceSentence.getTraditional()}
    , _jyutping{sourceSentence.getJyutping()}
    , _pinyin{sourceSentence.getPinyin()}
    , _prettyPinyin{sourceSentence.getPrettyPinyin()}
    , _sentences{sourceSentence.getSentenceSets()}
{}

SourceSentence::SourceSentence(const SourceSentence &&sourceSentence)
    : _sourceLanguage{std::move(sourceSentence._sourceLanguage)}
    , _simplified{std::move(sourceSentence._simplified)}
    , _traditional{std::move(sourceSentence._traditional)}
    , _jyutping{std::move(sourceSentence._jyutping)}
    , _pinyin{std::move(sourceSentence._pinyin)}
    , _prettyPinyin{std::move(sourceSentence._prettyPinyin)}
    , _sentences{std::move(sourceSentence._sentences)}
{}

SourceSentence::~SourceSentence() {}

SourceSentence &SourceSentence::operator=(const SourceSentence &sourceSentence)
{
    if (&sourceSentence == this) {
        return *this;
    }

    _sourceLanguage = std::move(sourceSentence._sourceLanguage);
    _simplified = sourceSentence._simplified;
    _traditional = sourceSentence._traditional;
    _jyutping = sourceSentence._jyutping;
    _pinyin = sourceSentence._pinyin;
    _prettyPinyin = sourceSentence._prettyPinyin;
    _sentences = sourceSentence._sentences;

    return *this;
}

SourceSentence &SourceSentence::operator=(const SourceSentence &&sourceSentence)
{
    if (&sourceSentence == this) {
        return *this;
    }

    _sourceLanguage = std::move(sourceSentence._sourceLanguage);
    _simplified = std::move(sourceSentence._simplified);
    _traditional = std::move(sourceSentence._traditional);
    _jyutping = std::move(sourceSentence._jyutping);
    _pinyin = std::move(sourceSentence._pinyin);
    _prettyPinyin = std::move(sourceSentence._prettyPinyin);
    _sentences = std::move(sourceSentence._sentences);

    return *this;
}

std::ostream &operator<<(std::ostream &out, const SourceSentence &sourceSentence)
{
    out << "Simplified: " << sourceSentence.getSimplified() << "\n";
    out << "Traditional: " << sourceSentence.getTraditional() << "\n";
    out << "Jyutping: " << sourceSentence.getJyutping() << "\n";
    out << "Pinyin: " << sourceSentence.getPinyin() << "\n";
    for (size_t i = 0; i < sourceSentence.getSentenceSets().size(); i++) {
        out << sourceSentence.getSentenceSets()[i] << "\n";
    }
    return out;
}

std::string SourceSentence::getSourceLanguage(void) const
{
    return _sourceLanguage;
}

void SourceSentence::setSourceLanguage(std::string sourceLanguage)
{
    _sourceLanguage = sourceLanguage;
}

std::string SourceSentence::getCharacters(EntryCharactersOptions options) const
{
    switch (options) {
    case EntryCharactersOptions::ONLY_SIMPLIFIED:
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
        return _simplified;
    case EntryCharactersOptions::ONLY_TRADITIONAL:
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        return _traditional;
    }
}

std::string SourceSentence::getSimplified(void) const
{
    return _simplified;
}

void SourceSentence::setSimplified(std::string simplified)
{
    _simplified = simplified;
}

std::string SourceSentence::getTraditional(void) const
{
    return _traditional;
}

void SourceSentence::setTraditional(std::string traditional)
{
    _traditional = traditional;
}

std::string SourceSentence::getPhonetic(EntryPhoneticOptions options,
                                        CantoneseOptions cantoneseOptions,
                                        MandarinOptions mandarinOptions) const
{
    switch (options) {
    case EntryPhoneticOptions::ONLY_JYUTPING:
    case EntryPhoneticOptions::PREFER_JYUTPING:
        return getCantonesePhonetic(cantoneseOptions);
    case EntryPhoneticOptions::ONLY_PINYIN:
    case EntryPhoneticOptions::PREFER_PINYIN:
        return getMandarinPhonetic(mandarinOptions);
    }
}

std::string SourceSentence::getCantonesePhonetic(
    CantoneseOptions cantoneseOptions) const
{
    switch (cantoneseOptions) {
    case CantoneseOptions::RAW_JYUTPING:
    default:
        return _jyutping;
    }
}

std::string SourceSentence::getMandarinPhonetic(
    MandarinOptions mandarinOptions) const
{
    switch (mandarinOptions) {
    case MandarinOptions::PRETTY_PINYIN:
        return _prettyPinyin;
    case MandarinOptions::RAW_PINYIN:
    default:
        return _pinyin;
    }
}

std::string SourceSentence::getJyutping(void) const
{
    return _jyutping;
}

void SourceSentence::setJyutping(std::string jyutping)
{
    _jyutping = jyutping;
}

std::string SourceSentence::getPinyin(void) const
{
    return _pinyin;
}

std::string SourceSentence::getPrettyPinyin(void) const
{
    return _prettyPinyin;
}

void SourceSentence::setPinyin(std::string pinyin)
{
    _pinyin = pinyin;
}

std::vector<SentenceSet> SourceSentence::getSentenceSets(void) const
{
    return _sentences;
}

std::string SourceSentence::getSentenceSnippet(void) const
{
    if (_sentences.empty()) {
        return "";
    }

    SentenceSet sentenceSet = _sentences.at(0);

    if (sentenceSet.getSentenceSnippet().empty()) {
        return "";
    }

    std::vector<Sentence::TargetSentence> snippets = sentenceSet
                                                         .getSentenceSnippet();

    if (snippets.empty()) {
        return "";
    }

    return snippets.at(0).sentence;
}

void SourceSentence::setIsWelcome(const bool isWelcome)
{
    _isWelcome = isWelcome;
}

bool SourceSentence::isWelcome(void) const
{
    return _isWelcome;
}

void SourceSentence::setIsEmpty(const bool isEmpty)
{
    _isEmpty = isEmpty;
}

bool SourceSentence::isEmpty(void) const
{
    return _isEmpty;
}

std::vector<std::string> SourceSentence::explodePhonetic(
    const std::string &string, const char delimiter) const
{
    std::vector<std::string> words;
    std::stringstream ss(string);
    std::string word;

    while (std::getline(ss, word, delimiter)) {
        words.push_back(word);
    }

    return words;
}

std::string SourceSentence::createPrettyPinyin(void)
{
    std::string result;

    // Create a vector of each space-separated value in pinyin
    std::vector<std::string> syllables = explodePhonetic(_pinyin, ' ');
    if (syllables.empty()) {
        return _pinyin;
    }

    for (auto syllable : syllables) {
        // Skip the punctuation, they have no tone
        if (specialCharacters.find(syllable) != specialCharacters.end()) {
            result += syllable + " ";
            continue;
        }

        // Extract the tone from the syllable
        size_t tone_location = syllable.find_first_of("012345");
        if (tone_location == std::string::npos) {
            result += syllable + " ";
            continue;
        }
        int tone = syllable.at(tone_location) - '0';

        // Convert u: to ü
        size_t location = syllable.find("u:");
        if (location != std::string::npos) {
            syllable.erase(location, 2);
            syllable.insert(location, "ü");
            location = std::string::npos;
        }

        // Convert v to ü
        location = syllable.find("v");
        if (location != std::string::npos) {
            syllable.erase(location, 1);
            syllable.insert(location, "ü");
            location = std::string::npos;
        }

        // The rule for pinyin diacritic location is:
        // - If a, e, or o exists in the syllable, it takes the diacritic.
        // - Otherwise, the last u, ü, or i takes it.
        location = syllable.find_first_of("aeo");
        size_t character_size = 1;

        if (location == std::string::npos) {
            location = syllable.find("ü");
            // ü is stored as two bytes, so change the number of characters that
            // we need to delete if we find a ü
            character_size = location == std::string::npos ? 1 : 2;
        }

        if (location == std::string::npos) {
            location = syllable.find_last_of("ui");
        }

        if (location == std::string::npos) {
            result += syllable + " ";
            continue;
        }

        // replacementMap maps a character to its replacements with diacritics.
        auto search = replacementMap.find(syllable.substr(location, character_size));
        if (search != replacementMap.end()) {
            std::string replacement = search->second.at(static_cast<size_t>(tone) - 1);
            syllable.erase(location, character_size);
            syllable.insert(location, replacement);
        } else {
            result += syllable + " ";
            continue;
        }

        // Remove the tone from the pinyin
        tone_location = syllable.find_first_of("012345");
        syllable.erase(tone_location, 1);
        result += syllable + " ";
    }

    // Remove trailing space
    result.erase(result.end() - 1);

    return result;
}
