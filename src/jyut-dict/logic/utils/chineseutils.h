#ifndef CHINESEUTILS_H
#define CHINESEUTILS_H

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"

#include <string>

namespace ChineseUtils {

std::string applyColours(
    std::string original,
    std::vector<int> &tones,
    std::vector<std::string> &jyutpingToneColours,
    std::vector<std::string> &pinyinToneColours,
    EntryColourPhoneticType type = EntryColourPhoneticType::JYUTPING);

// The function first converts both the simplified and traditional strings into
// u32strings on macOS and Linux, or wstrings on Windows.
//
// Since there is no guarantee of byte length per "character" in each string
// (as most Latin characters are one byte and Chinese characters are usually
// 3 bytes due to multibyte encoding schemes of UTF-8, and several entries
// consist of both latin and chinese character combinations), converting
// the string to u32string/wstring allows us to advance through the string
// "character" by "character", as a human would view it,
// even though the actual byte array is not stored that way.
//
// For each character, compare each of the characters between the simplified and
// traditional versions; if characters are different
// add that character to the comparison string.
// Otherwise, mark that character as the same between simplified and traditional
// by concatenating a full-width dash character to the comparison string.
//
// Finally, concatenate the preferred option with the comparison string,
// with the comparison string surrounded in curly braces.
//
// Example return values with an entry Traditional: 身體, Simplified: 身体
// With EntryCharactersOptions::PREFER_SIMPLIFIED:  "身体 {－體}"
// With EntryCharactersOptions::PREFER_TRADITIONAL: "身體 {－体}"
std::string compareStrings(std::string original,
                           std::string comparison);

std::string createPrettyPinyin(std::string pinyin);


}

#endif // CHINESEUTILS_H
