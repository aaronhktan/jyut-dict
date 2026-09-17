//
//  ChineseUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-14.
//

import Foundation

nonisolated let specialCharacters: Set = [
    ".", "。", ",", "，", "！", "？", "%", "－", "…", "⋯",
    ".", "·", "\"", "“", "”", "$", "｜", "：", "１", "２",
    "３", "４", "５", "６", "７", "８", "９", "０",
]
nonisolated let regexCharacters: Set = ["!", "(", ")", "|"]

nonisolated func applyColours(
    text: String,
    tones: [Int],
    jyutpingToneColours: [String],
    pinyinToneColours: [String],
    type: EntryColourPhoneticType
) -> String {
    var colouredString: String = ""

    var toneIdx = 0
    for c in text {
        let currentString = String(c).precomposedStringWithCanonicalMapping
        let codepoints = c.unicodeScalars

        if currentString == sameCharacterString || codepoints.count > 1 {
            colouredString += currentString
            toneIdx += 1
            continue
        }

        let isSpecialCharacter = specialCharacters.contains(currentString)
        let codepoint = codepoints[codepoints.startIndex].value
        let isIdeograph =
            (codepoint >= 0x4E00
                && codepoint <= 0x9FFF)  // CJK Unified Ideographs
            || (codepoint >= 0x3400
                && codepoint <= 0x4DBF)  // CJK Unified Ideographs Extension A
            || (codepoint >= 0x20000
                && codepoint <= 0x2A6DF)  // CJK Unified Ideographs Extension B
            || (codepoint >= 0x2A700
                && codepoint <= 0x2B73F)  // CJK Unified Ideographs Extension C
            || (codepoint >= 0x2B740
                && codepoint <= 0x2B81F)  // CJK Unified Ideographs Extension D
            || (codepoint >= 0x2B820
                && codepoint <= 0x2CEAF)  // CJK Unified Ideographs Extension E
            || (codepoint >= 0x2CEB0
                && codepoint <= 0x2EBEF)  // CJK Unified Ideographs Extension F
        if isSpecialCharacter || !isIdeograph {
            colouredString += currentString
            continue
        }

        if toneIdx >= tones.count {
            colouredString += currentString
            continue
        }
        let tone = tones[toneIdx]

        // ... and apply tone colour formatting to the string
        switch type {
        case .cantonese:
            if tone >= jyutpingToneColours.count {
                colouredString += currentString
                continue
            }
            colouredString +=
                "<font color=\""
                + jyutpingToneColours[tone]
                + "\">"
            break
        case .mandarin:
            if tone >= pinyinToneColours.count {
                colouredString += currentString
                continue
            }
            colouredString +=
                "<font color=\""
                + pinyinToneColours[tone]
                + "\">"
            break
        default:
            colouredString += "<font>"
            break
        }

        colouredString += currentString
        colouredString += "</font>"
        toneIdx += 1
    }

    return colouredString
}

nonisolated func compareStrings(original: String, comparison: String) -> String
{
    var result: String = ""

    guard original.count == comparison.count else {
        return result
    }

    for (x, y) in zip(original, comparison) {
        if specialCharacters.contains(String(y)) || x != y {
            result += String(y)
            continue
        }

        result += sameCharacterString
    }

    return result
}

nonisolated func constructRomanisationQuery(
    syllables: [String],
    delimiter: String
) -> String {
    if syllables.isEmpty {
        return ""
    }

    var addedDelimiter: Bool = false
    var space: String = ""

    var result: String = ""
    for s in syllables {
        let trimmedSyllable = s.trimmingCharacters(in: .whitespacesAndNewlines)
        if trimmedSyllable.suffix(1).first!.isNumber {
            result += space + trimmedSyllable
            space = " "
            addedDelimiter = false
        } else if ["*", "?"].contains(trimmedSyllable) {
            if ["*", "?", "* ", "? "].contains(s) && addedDelimiter {
                // Replace delimiter with GLOB wildcard if GLOB wildcard
                // was attached to end of previous word (i.e. wildcard did not
                // start with a space)
                result = String(result.prefix(result.count - 1))
            }
            result += s
            // GLOB characters handle their own spacing, so don't insert any
            // extra spaces.
            space = ""
            addedDelimiter = false
        } else {
            result += space + s + delimiter
            space = " "
            addedDelimiter = true
        }
    }

    return result
}
