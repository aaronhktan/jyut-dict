//
//  CantoneseUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import Foundation
import os

nonisolated let logger = Logger()

nonisolated let specialCharacters: Set = [
    ".", "。", ",", "，", "！", "？", "%", "－", "…", "⋯",
    ".", "·", "\"", "“", "”", "$", "｜", "：", "１", "２",
    "３", "４", "５", "６", "７", "８", "９", "０",
]
nonisolated let regexCharacters: Set = ["!", "(", ")", "|"]

nonisolated let initials: Set = [
    "b", "p", "m", "f", "d",
    "t", "n", "l", "g", "k",
    "ng", "h", "gw", "kw", "w",
    "z", "c", "s", "j", "m",
]
nonisolated let finals: Set = [
    "a", "aa", "aai", "aau", "aam", "aan", "aang", "aap", "aat", "aak",
    "ai", "au", "am", "an", "ang", "ap", "at", "ak", "e", "ei",
    "eu", "em", "en", "eng", "ep", "ek", "i", "iu", "im", "in",
    "ing", "ip", "it", "ik", "o", "oi", "ou", "on", "ong", "ot",
    "ok", "u", "ui", "un", "ung", "ut", "uk", "oe", "oet", "eoi",
    "eon", "oeng", "eot", "oek", "yu", "yun", "yut", "m", "ng",
]
let jyutpingFinalRegex: Regex = try! Regex(
    "([aeiou][aeiou]?[iumngptk]?[g]?)([1-6])"
)

nonisolated let jyutpingToYaleSpecialSyllables: [String: [String]] = [
    "m": ["m̄", "ḿ", "m", "m̀h", "ḿh", "mh"],
    "ng": ["n̄g", "ńg", "ng", "ǹgh", "ńgh", "ngh"],
]
nonisolated let jyutpingToYaleSpecialFinals: [String: String] = [
    "aa": "a",
    "oe": "eu",
    "oeng": "eung",
    "oek": "euk",
    "eoi": "eui",
    "eon": "eun",
    "eot": "eut",
]
nonisolated let yaleToneReplacements: [String: [String]] = [
    "a": ["ā", "á", "a", "à", "á", "a"],
    "e": ["ē", "é", "e", "è", "é", "e"],
    "i": ["ī", "í", "i", "ì", "í", "i"],
    "o": ["ō", "ó", "o", "ò", "ó", "o"],
    "u": ["ū", "ú", "u", "ù", "ú", "u"],
]
nonisolated let yaleYInitialRegex: String = "jy?"
nonisolated let yaleJInitialRegex: String = "z"
nonisolated let yaleChInitialRegex: String = "c"
nonisolated let yaleLightToneClusterRegex: NSRegularExpression =
    try! NSRegularExpression(
        pattern: "([ptkmn]?g?)[123456]$"
    )

nonisolated let cantoneseIPASpecialSyllables: [(String, String)] = [
    ("a", "@"),
    ("yu", "y"),
    ("@@", "a"),
    ("uk", "^k"),
    ("ik", "|k"),
    ("ou", "~u"),
    ("eoi", "eoy"),
    ("ung", "^ng"),
    ("ing", "|ng"),
    ("ei", ">i"),
]
nonisolated let cantoneseIPAInitials: [String: String] = [
    "b": "p",
    "p": "pʰ",
    "d": "t",
    "t": "tʰ",
    "g": "k",
    "k": "kʰ",
    "ng": "ŋ",
    "gw": "kʷ",
    "kw": "kʷʰ",
    "zh": "t͡ʃ",
    "ch": "t͡ʃʰ",
    "sh": "ʃ",
    "z": "t͡s",
    "c": "t͡sʰ",
]
nonisolated let cantoneseIPANuclei: [String: String] = [
    "a": "äː",
    "@": "ɐ",
    "e": "ɛː",
    ">": "ɛː",
    "i": "iː",
    "|": "ɪ",
    "o": "ɔː",
    "~": "o",
    "oe": "œ̽ː",
    "eo": "ɵ",
    "u": "uː",
    "^": "ʊ",
    "y": "yː",
]
nonisolated let cantoneseIPACodas: [String: String] = [
    "i": "i̯",
    "u": "u̯",
    "y": "y̯",
    "ng": "ŋ",
    "p": "p̚",
    "t": "t̚",
    "k": "k̚",
]
let cantoneseIPASyllableRegex: Regex = try! Regex(
    "([bcdfghjklmnpqrstvwxyz]?[bcdfghjklmnpqrstvwxyz]?)([a@e>i|o~u^y][eo]?)([iuymngptk]?g?)([1-9])"
)
let cantoneseIPAHyuRegex: Regex = try! Regex("([zcs])yu")
let cantoneseIPAHoeRegex: Regex = try! Regex("([zc])oe")
let cantoneseIPAHeoRegex: Regex = try! Regex("([zc])eo")
let cantoneseIPASpecialSyllableRegex: Regex = try! Regex(
    "^(h?)([mn]g?)([1-6])$"
)
let cantoneseIPASpecialMSpecialSyllableSyllableRegex: Regex = try! Regex("m")
let cantoneseIPASpecialNgSpecialSyllableSyllableRegex: Regex = try! Regex("ng")
let cantoneseIPAToneRegex: Regex = try! Regex("[1-6]")
let cantoneseIPACheckedToneRegex: Regex = try! Regex("([ptk])([136])")
let jyutpingToIPATones: [String] = [
    "˥", "˧˥", "˧", "˨˩", "˩˧", "˨", "˥", "˧", "˨",
]

private nonisolated func unfoldJyutpingRegex(jyutping: String) -> [String] {
    var out: [String] = []
    var stringPossibilities: [String] = []

    // Invariant: there must be only one set of parentheses and one exclamation
    if jyutping.filter({ $0 == "(" }).count > 1
        || jyutping.filter({ $0 == ")" }).count > 1
        || jyutping.filter({ $0 == "!" }).count > 1
    {
        return out
    }

    // If there is a parenthesis, then all of the options must be checked
    if jyutping.firstIndex(of: "(") != nil
        && jyutping.firstIndex(of: ")") != nil
    {
        var startIdx = jyutping.index(after: jyutping.firstIndex(of: "(")!)
        let endIdx = jyutping.firstIndex(of: ")")
        var orIdx = jyutping.firstIndex(of: "|")

        while orIdx != nil {
            let preParen = jyutping[..<jyutping.firstIndex(of: "(")!]
            let possibility = jyutping[startIdx..<orIdx!]
            let postParen = jyutping[jyutping.index(after: endIdx!)...]
            stringPossibilities.append(
                String(preParen + possibility + postParen)
            )
            startIdx = jyutping.index(after: orIdx!)
            orIdx = jyutping.firstIndex(of: "|", at: startIdx)
        }
        let preParen = jyutping[..<jyutping.firstIndex(of: "(")!]
        let possibility = jyutping[startIdx..<endIdx!]
        let postParen = jyutping[jyutping.index(after: endIdx!)...]
        stringPossibilities.append(
            String(preParen + possibility + postParen)
        )
    } else {
        stringPossibilities.append(jyutping)
    }

    // If there is a "!", then the initial with and the initial without
    // that optional character should be considered
    stringPossibilities.forEach { s in
        let regexIdx = s.firstIndex(of: "!")
        if regexIdx == nil {
            out.append(s)
        } else if regexIdx == s.startIndex {
            out.append(String(s[s.index(after: s.startIndex)...]))
        } else {
            var tmp = s
            tmp.removeSubrange(
                s.index(before: regexIdx!)...s.index(after: regexIdx!)
            )
            out.append(tmp)
            tmp = s
            tmp.remove(at: regexIdx!)
            out.append(tmp)
        }
    }

    return out
}

private func convertYaleInitial(syllable: String) -> String {
    var yaleSyllable = syllable
    yaleSyllable = yaleSyllable.replacingOccurrences(
        of: yaleYInitialRegex,
        with: "y",
        options: [.regularExpression]
    )
    yaleSyllable = yaleSyllable.replacingOccurrences(
        of: yaleJInitialRegex,
        with: "j",
        options: [.regularExpression]
    )
    yaleSyllable = yaleSyllable.replacingOccurrences(
        of: yaleChInitialRegex,
        with: "ch",
        options: [.regularExpression]
    )
    return yaleSyllable
}

private func convertYaleFinal(syllable: String) -> String {
    var yaleSyllable = syllable

    // Attempt to isolate the part of the Jyutping syllable that is the final
    guard let match = yaleSyllable.firstMatch(of: jyutpingFinalRegex) else {
        logger.error("Could not find final in syllable \(syllable)")
        return yaleSyllable
    }

    let final = String(match[1].substring!)
    let tone = Int(match[2].substring!)

    let finalIdx = yaleSyllable.range(of: final)?.lowerBound

    // Some Jyutping finals have significant differences when mapped to Yale.
    // Switch it out here.
    if jyutpingToYaleSpecialFinals.contains(where: { $0.key == final }) {
        yaleSyllable.replaceSubrange(
            finalIdx!..<yaleSyllable.index(finalIdx!, offsetBy: final.count),
            with: jyutpingToYaleSpecialFinals[final]!
        )
    }

    // Insert an "h" before the last consonant cluster for the light tones,
    // as they are indicated in Yale
    if tone == 4 || tone == 5 || tone == 6 {
        let range = NSRange(yaleSyllable.startIndex..., in: yaleSyllable)
        yaleSyllable = yaleLightToneClusterRegex.stringByReplacingMatches(
            in: yaleSyllable,
            range: range,
            withTemplate: "h$&"
        )
    }

    // Replace the first vowel in the final with its accented version
    let vowels = ["a", "e", "i", "o", "u"]
    guard
        let replacementIdx = yaleSyllable.firstIndex(where: {
            vowels.contains(String($0))
        })
    else {
        logger.error("No vowel found in syllable \(yaleSyllable)")
        return yaleSyllable
    }
    let firstVowel = String(yaleSyllable[replacementIdx])
    let accentedFirstVowel = yaleToneReplacements[firstVowel]![tone! - 1]
    yaleSyllable.replaceSubrange(
        replacementIdx..<yaleSyllable.index(after: replacementIdx),
        with: accentedFirstVowel
    )

    // Remove the tone number, as Yale doesn't use those
    yaleSyllable = String(
        yaleSyllable[..<yaleSyllable.index(before: yaleSyllable.endIndex)]
    )

    return yaleSyllable
}

// Note that the majority of this function and the convertToIPA function
// is derivative of Wiktionary's conversion code, contained in the module
// "yue-pron" (https://en.wiktionary.org/wiki/Module:yue-pron)
func convertJyutpingToYale(jyutping: String, useSpacesToSegment: Bool) -> String
{
    // TODO: Implement
    jyutping
}

func convertJyutpingToIPA(jyutping: String, useSpacesToSegment: Bool) -> String
{
    // TODO: Implement
    jyutping
}

nonisolated func segmentJyutping(
    text: String,
    removeSpecialCharacters: Bool = true,
    removeGlobCharacters: Bool = true,
    removeRegexCharacters: Bool = true
) -> (Bool, [String]) {
    var processedText = text
    var syllables: [String] = []

    var validJyutping = true

    if removeSpecialCharacters {
        specialCharacters.forEach { c in
            processedText = processedText.split(separator: c).joined(
                separator: " "
            )
        }
    }
    if removeGlobCharacters {
        ["*", "?"].forEach { c in
            processedText = processedText.split(separator: c).joined(
                separator: " "
            )
        }
    }
    if removeRegexCharacters {
        regexCharacters.forEach { c in
            processedText = processedText.split(separator: c).joined(
                separator: " "
            )
        }
    }

    // Keep track of indices for current segmented word; [start_index, end_index)
    // Greedily try to expand end_index by checking for valid sequences
    // of characters
    var startIdx = processedText.startIndex
    var endIdx = processedText.startIndex
    var initialFound = false

    while endIdx < processedText.endIndex {
        var componentFound = false

        var currentString = String(
            processedText[endIdx..<processedText.index(after: endIdx)]
        )
        let isSpecialCharacter = specialCharacters.contains(currentString)
        let isGlobCharacter =
            currentString.trimmingCharacters(in: .whitespacesAndNewlines) == "*"
            || currentString.trimmingCharacters(in: .whitespacesAndNewlines)
                == "?"
        if currentString == " " || currentString == "'" || isSpecialCharacter
            || isGlobCharacter
        {
            // The presence of a space, apostrophe, special character, or
            // glob character indicates that a syllable is completed.
            if initialFound {
                // If a valid initial was previously found, then the Jyutping
                // sequence [initial] + [separator] is only valid Jyutping if the
                // initial is also a valid final (i.e. [final] + [separator] is
                // OK, but [initial] + [separator] is not).
                let previousInitial = String(processedText[startIdx..<endIdx])
                    .lowercased()
                syllables.append(previousInitial)
                if !finals.contains(previousInitial) {
                    validJyutping = false
                }
                startIdx = endIdx
                initialFound = false
            }
            if isGlobCharacter {
                if endIdx > processedText.startIndex && endIdx > startIdx {
                    let previousInitial = String(
                        processedText[startIdx..<endIdx]
                    ).lowercased()
                    syllables.append(previousInitial)
                    if !finals.contains(previousInitial) {
                        validJyutping = false
                    }
                    initialFound = false
                }

                // Since whitespace matters for glob and regex, consume the
                // next or previous whitespace if it exists (and was not
                // already consumed by another glob character).
                var globStartIdx = endIdx
                if endIdx > processedText.startIndex
                    && processedText[processedText.index(before: endIdx)] == " "
                    && !syllables.isEmpty
                    && syllables.last!.last != " "
                {
                    // Add preceding whitespace to this word
                    globStartIdx = processedText.index(before: endIdx)
                }
                if processedText.index(after: endIdx) < processedText.endIndex
                    && processedText[processedText.index(after: endIdx)] == " "
                {
                    // Add succeeding whitespace to this word
                    endIdx = processedText.index(after: endIdx)
                }
                let glob = String(processedText[globStartIdx...endIdx])
                    .lowercased()
                syllables.append(glob)
                startIdx = endIdx
            } else if isSpecialCharacter {
                syllables.append(currentString)
            }
            startIdx = processedText.index(after: startIdx)
            endIdx = processedText.index(after: endIdx)
            continue
        }

        // Digits are only valid after a final (which should be handled in the
        // final-checking code)
        // OR after an initial (that is also a final), like m or ng.
        // This block checks for the latter case.
        if currentString[currentString.startIndex].isNumber {
            if initialFound {
                var previousInitial = String(processedText[startIdx..<endIdx])
                    .lowercased()

                var isValidFinal = false
                if removeRegexCharacters {
                    isValidFinal = finals.contains(previousInitial)
                } else {
                    // Regex characters need to be handled in a special way;
                    // essentially, we need to check every possibility. If at
                    // least one possibility is a valid final, then the Jyutping
                    // can be considered valid.
                    let stringsToSearch: [String] = unfoldJyutpingRegex(
                        jyutping: previousInitial
                    )

                    stringsToSearch.forEach { s in
                        isValidFinal = isValidFinal || finals.contains(s)
                    }
                }

                if isValidFinal {
                    endIdx = processedText.index(after: endIdx)
                    previousInitial = String(processedText[startIdx..<endIdx])
                        .lowercased()
                    syllables.append(previousInitial)
                    startIdx = endIdx
                    initialFound = false

                    if Int(String(currentString[currentString.startIndex])) ?? 0
                        < 1
                        || Int(String(currentString[currentString.startIndex]))
                            ?? 7 > 6
                    {
                        validJyutping = false
                    }

                    continue
                }
            } else {
                // If there was no initial found, then the Jyutping isn't valid
                validJyutping = false
                syllables.append(currentString)
                startIdx = processedText.index(after: startIdx)
                endIdx = processedText.index(after: endIdx)
                continue
            }
        }

        // If initial is valid, then extend the end_index for length of initial
        // cluster of consonants.
        // The longest length of an initial with unfolded regex is 16 UTF-16 bytes.
        var remainingLength: Int = processedText.distance(
            from: endIdx,
            to: processedText.endIndex
        )
        let maxInitialLength: Int =
            removeRegexCharacters
            ? min(2, remainingLength)
            : min(
                16,
                remainingLength
            )
        for initialLen in (1...maxInitialLength).reversed() {
            currentString = String(
                processedText[
                    endIdx..<processedText.index(endIdx, offsetBy: initialLen)
                ]
            ).lowercased()

            var isValidInitial = false
            if removeRegexCharacters {
                isValidInitial = initials.contains(currentString)
            } else {
                let stringsToSearch: [String] = unfoldJyutpingRegex(
                    jyutping: currentString
                )
                stringsToSearch.forEach { s in
                    isValidInitial = isValidInitial || finals.contains(s)
                }
            }

            if !isValidInitial {
                continue
            }

            if initialFound {
                // Multiple initials in a row are only valid if previous "initial"
                // was actually a final (like m or ng)
                let previousInitial = processedText[startIdx..<endIdx]
                    .lowercased()

                var previousInitialIsValidFinal = false
                if removeRegexCharacters {
                    previousInitialIsValidFinal = finals.contains(
                        previousInitial
                    )
                } else {
                    let stringsToSearch: [String] = unfoldJyutpingRegex(
                        jyutping: previousInitial
                    )
                    stringsToSearch.forEach { s in
                        previousInitialIsValidFinal =
                            previousInitialIsValidFinal || finals.contains(s)
                    }
                }

                if previousInitialIsValidFinal {
                    syllables.append(previousInitial)
                    startIdx = endIdx
                } else {
                    validJyutping = false
                }
            }

            endIdx = processedText.index(endIdx, offsetBy: initialLen)
            componentFound = true
            initialFound = true

            if isValidInitial {
                break
            }
        }

        if componentFound {
            continue
        }

        // If final is valid, then extend end_index for length of final.
        // Check for number at end of word as well (this represents tone number).
        //
        // Then add the substring from [start_index, end_index) to vector
        // and reset start_index, so we can start searching after the end_index.
        remainingLength = processedText.distance(
            from: endIdx,
            to: processedText.endIndex
        )
        let maxFinalLength: Int =
            removeRegexCharacters
            ? min(4, remainingLength)
            : min(
                16,
                remainingLength
            )
        for finalLen in (1...maxFinalLength).reversed() {
            currentString = String(
                processedText[
                    endIdx..<processedText.index(endIdx, offsetBy: finalLen)
                ]
            ).lowercased()

            var isValidFinal = false
            if removeRegexCharacters {
                isValidFinal = finals.contains(currentString)
            } else {
                let stringsToSearch: [String] = unfoldJyutpingRegex(
                    jyutping: currentString
                )
                stringsToSearch.forEach { s in
                    isValidFinal = isValidFinal || finals.contains(s)
                }
            }

            if isValidFinal {
                endIdx = processedText.index(endIdx, offsetBy: finalLen)
                if endIdx < processedText.endIndex {
                    if processedText[endIdx].isNumber {
                        if Int(String(processedText[endIdx])) ?? 0
                            < 1
                            || Int(String(processedText[endIdx]))
                                ?? 7 > 6
                        {
                            validJyutping = false
                        }

                        endIdx = processedText.index(after: endIdx)
                    }
                }

                let syllable = processedText[startIdx..<endIdx].lowercased()
                syllables.append(syllable)
                startIdx = endIdx
                componentFound = true
                initialFound = false
                break
            }
        }

        if componentFound {
            continue
        } else {
            validJyutping = false
        }

        endIdx = processedText.index(after: endIdx)
    }

    // Then add whatever's left in the search term, minus whitespace.
    let lastSyllable = processedText[startIdx..<endIdx].trimmingCharacters(
        in: .whitespacesAndNewlines
    ).lowercased()
    if !lastSyllable.isEmpty && lastSyllable != "'" {
        syllables.append(lastSyllable)
        if !finals.contains(lastSyllable) {
            validJyutping = false
        }
    }

    return (validJyutping, syllables)
}

func jyutpingAutocorrect(text: String, unsafeSubstitutions: Bool = false)
    -> String
{
    text
}

func jyutpingSoundChanges(text: [String]) -> [String] {
    text
}
