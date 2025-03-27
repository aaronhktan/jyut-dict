#ifndef CHINESEUTILS_H
#define CHINESEUTILS_H

#include "logic/entry/entryphoneticoptions.h"

#include <QString>

#include <string>

// The ChineseUtils namespace contains static functions for working with
// various romanization schemes and Chinese queries.

namespace ChineseUtils {

std::string applyColours(
    const std::string original,
    const std::vector<int> &tones,
    const std::vector<std::string> &jyutpingToneColours,
    const std::vector<std::string> &pinyinToneColours,
    const EntryColourPhoneticType type = EntryColourPhoneticType::CANTONESE);

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
std::string compareStrings(const std::string &original,
                           const std::string &comparison);

std::string convertJyutpingToYale(const std::string &jyutping,
                                  bool useSpacesToSegment = false);
std::string convertJyutpingToIPA(const std::string &jyutping,
                                 bool useSpacesToSegment = false);

std::string createPrettyPinyin(const std::string &pinyin);
std::string createNumberedPinyin(const std::string &pinyin);
std::string createPinyinWithV(const std::string &pinyin);

std::string convertPinyinToZhuyin(const std::string &pinyin,
                                  bool useSpacesToSegment = false);
std::string convertPinyinToIPA(const std::string &pinyin,
                               bool useSpacesToSegment = false);

// constructRomanisationQuery takes a vector of strings and stitches them
// together with a delimiter.
//
// Since this is used for searching, we check whether to add a single wildcard
// character at the end of each string: if the last character of the string
// is a number, or the last character is already the same wildcard,
// then we do not add a wildcard, otherwise, we do.
//
// The reason for this is as follows: when searching via romanization systems,
// it can be assumed that adding a digit to the end of a word "terminates" it,
// as it represents a tone. Without a digit, the user may not have completed
// typing the word or neglected to type a tone.
//
// So by adding a single wildcard character, we match against any word that
// 1) has that pronunciation in any of the tones, or
// 2) (if one word) any word with at least one more character following the word
// 3) (if multiple words) any words with any/specific tones, except for the last
//    word, which matches against all words that start with that spelling.
//
// Example 1 - Single word:
// Searching jyutping with "se" does one call to constructRomanisationQuery:
// 1) For GLOB, the phrase is suffixed with the wildcard character "?",
//    which allows it to be matched with "se1, se2, se3, se4, se5, se6".
//    The return value is se?.
//
//    Currently, since searchJyutping also appends the unlimited wildcard "*"
//    at the end of the query, it would also match against "sei1" or "seoi5" or
//    any other word or phrase that starts with "se" and contains at least one
//    more character following the word.
//
// Example 2 - Multiple words, no tone marker:
// Searching jyutping with "daai koi" is first exploded into "daai" and "koi"
// by segmentJyutping, then does one call to constructRomanisationQuery:
// 1) For GLOB, each phrase is affixed with the single character wildcard.
//    The return value is daai? koi?.
//
// Example 3 - Multiple words, some tone markers:
// Searching pinyin with "ke3 ai" is first exploded into "ke3" and "ai" by
// segmentPinyin, then does one call to constructRomanisationQuery:
// 1) For GLOB, the first phrase is not affixed with a single character
//    wildcard, as it is terminated by a digit. The second one is not, so it
//    is affixed with the single character wildcard. The return value is
//    ke3 ai?.
std::string constructRomanisationQuery(const std::vector<std::string> &words,
                                       const char *delimiter);

bool jyutpingAutocorrect(const QString &in,
                         QString &out,
                         bool unsafeSubstitutions = false);
bool jyutpingSoundChanges(std::vector<std::string> &inOut);

bool segmentPinyin(const QString &string,
                   std::vector<std::string> &out,
                   bool removeSpecialCharacters = true,
                   bool removeGlobCharacters = true);
bool segmentJyutping(const QString &string,
                     std::vector<std::string> &out,
                     bool removeSpecialCharacters = true,
                     bool removeGlobCharacters = true);
} // namespace ChineseUtils

#endif // CHINESEUTILS_H
