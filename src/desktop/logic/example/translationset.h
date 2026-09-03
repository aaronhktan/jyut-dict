#ifndef TRANSLATIONSET_H
#define TRANSLATIONSET_H

#include <ostream>
#include <span>
#include <string>
#include <vector>

// The TranslationSet class contains a grouping of Translations, where all of
// the translations are related by source.

namespace Translation {

// The Translation struct provides:
// the language, the translation itself, and whether the translation was a
// direct translation.
struct Translation
{
    std::string content;
    std::string language;
    bool directTarget;

    Translation(const std::string &content,
                const std::string &language, bool directTarget):
        content{content},
        language{language},
        directTarget(directTarget)
    {}

    bool operator==(const Translation &other) const = default;
};
}

class TranslationSet
{
public:
    TranslationSet(const std::string &source);
    TranslationSet(const std::string &source,
                   const std::vector<Translation::Translation> &examples);
    friend std::ostream &operator<<(std::ostream &out,
                                    const TranslationSet &translationSet);
    bool operator==(const TranslationSet &other) const
    {
        return _source == other._source && _translations == other._translations;
    }
    bool isEmpty(void) const;

    bool pushTranslation(const Translation::Translation &translation);

    const std::string &getSource(void) const;
    const std::string &getSourceLongString(void) const;
    const std::string &getSourceShortString(void) const;
    std::span<const Translation::Translation> getTranslationSnippet(void) const;
    std::span<const Translation::Translation> getTranslations(void) const;

private:
    std::string _source;
    std::string _sourceShortString;
    mutable std::vector<Translation::Translation> _snippet;
    std::vector<Translation::Translation> _translations;
};

#endif // TRANSLATIONSET_H
