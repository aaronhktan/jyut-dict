#ifndef CHINESEUTILS_H
#define CHINESEUTILS_H

#include "logic/entry/entryphoneticoptions.h"

#include <QString>

#include <string>

// The ChineseUtils namespace contains static functions for working with
// various romanization schemes and Han characters.

namespace ChineseUtils {

std::string applyColours(const std::string original,
                         const std::vector<uint8_t> &tones,
                         const std::vector<std::string> &jyutpingToneColours,
                         const std::vector<std::string> &pinyinToneColours,
                         const EntryColourPhoneticType type
                         = EntryColourPhoneticType::CANTONESE);

// The function first converts both the simplified and traditional strings into
// u32strings.
//
// There is no guarantee of byte length per "character" in each string
// (most Latin characters are one byte, Chinese characters are usually
// 2-3 bytes due to multibyte encoding schemes of UTF-8 + several entries
// consist of both latin and chinese character combinations). Converting
// the string to u32string makes each index correspond to exactly one grapheme,
// allowing us to iterate through the string grapheme by grapheme by
// incrementing the index, which is not true of a UTF-8 or UTF-16-encoded string.
//
// For each string, compare each of the graphemes between the simplified and
// traditional versions; if graphemes are different, add that grapheme to the
// comparison string.
// Otherwise, mark that grapheme as the same between simplified and traditional
// by concatenating a full-width dash character to the comparison string.
//
// Example values with an entry Traditional: 身體, Simplified: 身体
// With EntryCharactersOptions::PREFER_SIMPLIFIED:  "身体 {－體}"
// With EntryCharactersOptions::PREFER_TRADITIONAL: "身體 {－体}"
std::string compareStrings(const std::string &original,
                           const std::string &comparison);

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

} // namespace ChineseUtils

#endif // CHINESEUTILS_H
