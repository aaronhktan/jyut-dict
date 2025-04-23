#ifndef MANDARINUTILS_H
#define MANDARINUTILS_H

#include "logic/entry/entryphoneticoptions.h"

#include <QString>

#include <string>

// The MandarinUtils namespace contains static functions for working with
// Mandarin romanizations.

namespace MandarinUtils {

std::string createPrettyPinyin(const std::string &pinyin);
std::string createNumberedPinyin(const std::string &pinyin);
std::string createPinyinWithV(const std::string &pinyin);

std::string convertPinyinToZhuyin(const std::string &pinyin,
                                  bool useSpacesToSegment = false);
std::string convertPinyinToIPA(const std::string &pinyin,
                               bool useSpacesToSegment = false);

bool segmentPinyin(const QString &string,
                   std::vector<std::string> &out,
                   bool removeSpecialCharacters = true,
                   bool removeGlobCharacters = true);
} // namespace MandarinUtils

#endif // MANDARINUTILS_H
