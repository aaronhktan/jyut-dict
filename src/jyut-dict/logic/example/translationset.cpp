#include "translationset.h"

#include "logic/source/sourceutils.h"

#include <QtSystemDetection>

#ifndef Q_OS_MAC
#include <algorithm>
#endif

TranslationSet::TranslationSet(const std::string &source)
    : _source{source}
    , _sourceShortString{SourceUtils::getSourceShortString(source)}
{
}

TranslationSet::TranslationSet(const std::string &source,
                         const std::vector<Translation::Translation> &translations)
    : _source{source}
    , _sourceShortString{SourceUtils::getSourceShortString(source)}
    , _translations{translations}
{
}

std::ostream &operator<<(std::ostream &out, const TranslationSet &TranslationSet)
{
    out << "======== source\n" << TranslationSet.getSource() << "\n";
    for (const auto &translation : TranslationSet.getTranslations()) {
        out << "=========== translation\n" << translation.content << "\n";
        out << "=========== language\n" << translation.language << "\n";
        out << "=========== directTarget\n" << translation.directTarget << "\n";
    }
    return out;
}

bool TranslationSet::isEmpty(void) const
{
    return _translations.empty()
           || std::all_of(_translations.begin(),
                          _translations.end(),
                          [](const Translation::Translation &translation) {
                              return translation.content == ""
                                     && translation.language == ""
                                     && translation.directTarget == false;
                          });
}

bool TranslationSet::pushTranslation(const Translation::Translation &translation)
{
    _translations.push_back(translation);
    return true;
}

const std::string &TranslationSet::getSource(void) const
{
    return _source;
}

const std::string &TranslationSet::getSourceLongString(void) const
{
    return _source;
}

const std::string &TranslationSet::getSourceShortString(void) const
{
    return _sourceShortString;
}

std::span<const Translation::Translation> TranslationSet::getTranslationSnippet(
    void) const
{
    if (!_snippet.empty()) {
        return _snippet;
    }

    for (int i = 0; i < std::min(static_cast<size_t>(5), _translations.size());
         ++i) {
        _snippet.emplace_back(_translations.at(i));
    }

    return _snippet;
}

std::span<const Translation::Translation> TranslationSet::getTranslations() const
{
    return _translations;
}
