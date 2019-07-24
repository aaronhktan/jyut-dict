#ifndef DICTIONARYSOURCE_H
#define DICTIONARYSOURCE_H

#include <QObject>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

// DictionarySource is an enum that contains possible dictionary sources
enum DictionarySource {
    CEDICT,
    CCCANTO,
    CFDICT,
};

static std::unordered_map<std::string, DictionarySource> string_to_dictionarysource{
    {"CC_CEDICT", CEDICT},
    {"CC_CANTO", CCCANTO},
    {"CFDICT", CFDICT},
};

static std::unordered_map<DictionarySource, std::string> dictionarysource_to_string{
    {CEDICT, "CC_CEDICT"},
    {CCCANTO, "CC_CANTO"},
    {CFDICT, "CFDICT"},
};

static std::unordered_map<DictionarySource, std::string> dictionarysource_to_short_string{
    {CEDICT, "CC"},
    {CCCANTO, "CCY"},
    {CFDICT, "CF"},
};

namespace DictionarySourceUtils {

std::string getSourceLongString(DictionarySource source);
std::string getSourceShortString(DictionarySource source);

}

Q_DECLARE_METATYPE(DictionarySource);

#endif // DICTIONARYSOURCE_H
