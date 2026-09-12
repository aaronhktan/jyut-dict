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
nonisolated(unsafe) let jyutpingFinalRegex: Regex = try! Regex(
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
nonisolated let yaleLightToneClusterRegex: String = "([ptkmn]?g?)[123456]$"

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
    ">": "e",
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
nonisolated(unsafe) let cantoneseIPASyllableRegex: Regex = try! Regex(
    "([bcdfghjklmnpqrstvwxyz]?[bcdfghjklmnpqrstvwxyz]?)([a@e>i|o~u^y][eo]?)([iuymngptk]?g?)([1-9])"
)
nonisolated let cantoneseIPAHyuRegex: String = "([zcs])yu"
nonisolated let cantoneseIPAHoeRegex: String = "([zc])oe"
nonisolated let cantoneseIPAHeoRegex: String = "([zc])eo"
nonisolated(unsafe) let cantoneseIPASpecialSyllableRegex: Regex = try! Regex(
    "^(h?)([mn]g?)([1-6])$"
)
nonisolated let cantoneseIPAToneRegex: String = "[1-6]"
nonisolated(unsafe) let cantoneseIPACheckedToneRegex: Regex = try! Regex(
    "([ptk])([136])"
)
nonisolated let jyutpingToIPATones: [String] = [
    "˥", "˧˥", "˧", "˨˩", "˩˧", "˨", "˥", "˧", "˨",
]

nonisolated private func unfoldJyutpingRegex(jyutping: String) -> [String] {
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

nonisolated private func convertYaleInitial(syllable: String) -> String {
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

nonisolated private func convertYaleFinal(syllable: String) -> String {
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
        yaleSyllable = yaleSyllable.replacingOccurrences(
            of: yaleLightToneClusterRegex,
            with: "h$0",
            options: [.regularExpression]
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
nonisolated func convertJyutpingToYale(
    jyutping: String,
    useSpacesToSegment: Bool = false
) -> String {
    if jyutping.isEmpty {
        return jyutping
    }

    var syllables: [String] = []
    var jyutpingCopy = ""
    if useSpacesToSegment {
        // Insert a space before and after every special character, so that the
        // IPA conversion doesn't attempt to convert special characters.
        specialCharacters.forEach { c in
            jyutpingCopy = jyutping.split(separator: c).joined(
                separator: " " + c + " "
            )
        }
        syllables = jyutpingCopy.split(separator: " ").map(String.init)
    } else {
        let (validJyutping, result) = segmentJyutping(
            text: jyutping,
            removeSpecialCharacters: false,
            removeGlobCharacters: false
        )
        if !validJyutping {
            return "x"
        }
        syllables = result
    }

    var yaleSyllables: [String] = []
    for syllable in syllables {
        // Most numbers, single characters, etc. are not Jyutping.
        // Filter those out.
        if syllable.count == 1 {
            yaleSyllables.append(syllable)
            continue
        }

        // Skip syllables that are just punctuation
        if specialCharacters.contains(syllable) {
            yaleSyllables.append(syllable)
            continue
        }

        // Skip syllables that don't have tone
        let tones = ["1", "2", "3", "4", "5", "6"]
        guard
            let toneIdx = syllable.firstIndex(where: {
                tones.contains(String($0))
            })
        else {
            yaleSyllables.append(syllable)
            continue
        }

        // Handle special-case syllables
        let syllableWithoutTone = String(syllable[..<toneIdx])
        let tone = Int(String(syllable[toneIdx]))!
        if jyutpingToYaleSpecialSyllables.contains(where: {
            $0.key == syllableWithoutTone
        }) {
            yaleSyllables.append(
                jyutpingToYaleSpecialSyllables[syllableWithoutTone]![tone - 1]
            )
            continue
        }

        var yaleSyllable = syllable
        yaleSyllable = convertYaleFinal(syllable: yaleSyllable)
        yaleSyllable = convertYaleInitial(syllable: yaleSyllable)
        yaleSyllables.append(yaleSyllable)
    }

    return yaleSyllables.joined(separator: " ")
}

nonisolated private func convertIPACantoneseSyllable(_ jyutping: String)
    -> String
{
    guard let match = jyutping.firstMatch(of: cantoneseIPASyllableRegex) else {
        logger.error("Invalid jyutping found in IPA conversion: \(jyutping)")
        return jyutping
    }

    var initial: String = ""
    if let matchedInitial = match[1].substring {
        if cantoneseIPAInitials.contains(where: { $0.key == matchedInitial }) {
            initial = cantoneseIPAInitials[String(matchedInitial)]!
        } else {
            initial = String(matchedInitial)
        }
    }

    var nucleus: String = ""
    if let matchedNucleus = match[2].substring {
        if cantoneseIPANuclei.contains(where: { $0.key == matchedNucleus }) {
            nucleus = cantoneseIPANuclei[String(matchedNucleus)]!
        } else {
            nucleus = String(matchedNucleus)
        }
    }

    var coda: String = ""
    if let matchedCoda = match[3].substring {
        if cantoneseIPACodas.contains(where: { $0.key == matchedCoda }) {
            coda = cantoneseIPACodas[String(matchedCoda)]!
        } else {
            coda = String(matchedCoda)
        }
    }

    var tone: String = ""
    if let matchedTone = match[4].substring {
        tone = jyutpingToIPATones[Int(String(matchedTone))! - 1]
    }

    return initial + nucleus + coda + tone
}

nonisolated func convertJyutpingToIPA(
    jyutping: String,
    useSpacesToSegment: Bool = false
) -> String {
    if jyutping.isEmpty {
        return jyutping
    }

    var syllables: [String] = []
    var jyutpingCopy = ""
    if useSpacesToSegment {
        // Insert a space before and after every special character, so that the
        // IPA conversion doesn't attempt to convert special characters.
        specialCharacters.forEach { c in
            jyutpingCopy = jyutping.split(separator: c).joined(
                separator: " " + c + " "
            )
        }
        syllables = jyutpingCopy.split(separator: " ").map(String.init)
    } else {
        let (validJyutping, result) = segmentJyutping(
            text: jyutping,
            removeSpecialCharacters: false,
            removeGlobCharacters: false
        )
        if !validJyutping {
            return "x"
        }
        syllables = result
    }

    var ipaSyllables: [String] = []
    for syllable in syllables {
        // Most numbers, single characters, etc. are not Jyutping.
        // Filter those out.
        if syllable.count == 1 {
            ipaSyllables.append(syllable)
            continue
        }

        // Skip syllables that are just punctuation
        if specialCharacters.contains(syllable) {
            ipaSyllables.append(syllable)
            continue
        }

        // Skip syllables that don't have tone
        let tones = ["1", "2", "3", "4", "5", "6"]
        guard
            syllable.firstIndex(where: {
                tones.contains(String($0))
            }) != nil
        else {
            ipaSyllables.append(syllable)
            continue
        }

        // Do some pre-processing
        var ipaSyllable = syllable
        ipaSyllable = ipaSyllable.replacingOccurrences(
            of: cantoneseIPAHyuRegex,
            with: "$1hyu",
            options: [.regularExpression]
        )
        ipaSyllable = ipaSyllable.replacingOccurrences(
            of: cantoneseIPAHoeRegex,
            with: "$1hoe",
            options: [.regularExpression]
        )
        ipaSyllable = ipaSyllable.replacingOccurrences(
            of: cantoneseIPAHeoRegex,
            with: "$1heo",
            options: [.regularExpression]
        )

        // Convert special syllables
        if let specialMatch = ipaSyllable.firstMatch(
            of: cantoneseIPASpecialSyllableRegex
        ) {
            guard let tone = Int(String(specialMatch[3].substring!)) else {
                return ipaSyllable
            }
            ipaSyllable = ipaSyllable.replacingOccurrences(
                of: "m",
                with: "m̩",
            )
            ipaSyllable = ipaSyllable.replacingOccurrences(
                of: "ng",
                with: "ŋ̍",
            )
            ipaSyllable = ipaSyllable.replacingOccurrences(
                of: cantoneseIPAToneRegex,
                with: jyutpingToIPATones[tone - 1],
                options: [.regularExpression]
            )
        }

        // Replace checked tones
        if ipaSyllable.firstMatch(of: cantoneseIPACheckedToneRegex) != nil {
            ipaSyllable = ipaSyllable.replacingOccurrences(of: "1", with: "7")
            ipaSyllable = ipaSyllable.replacingOccurrences(of: "3", with: "8")
            ipaSyllable = ipaSyllable.replacingOccurrences(of: "6", with: "9")
        }

        // Do some more preprocessing
        for (from, to) in cantoneseIPASpecialSyllables {
            ipaSyllable = ipaSyllable.replacingOccurrences(
                of: from,
                with: to
            )
        }

        ipaSyllables.append(convertIPACantoneseSyllable(ipaSyllable))
    }

    return ipaSyllables.joined(separator: " ")
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
    var out: String = text

    // This is for some romanizations like "shui" for 水
    // And needs to happen before the "sh" -> "s" conversion
    out = out.replacingOccurrences(of: "hui", with: "heoi")

    // The initial + nucleus "cu-" never appears in Jyutping, so the user
    // probably intended to make the IPA [kʰɐ] sound
    // Surround the k with capturing group to prevent replacement with (g|k)
    // if sound changes are enabled
    out = out.replacingOccurrences(of: "cu", with: "(k)u")

    // "x" never appears in Jyutping, the user might be more familiar
    // with Pinyin and assume that it's an "s" sound
    out = out.replacingOccurrences(of: "x", with: "s")

    out = out.replacingOccurrences(of: "ch", with: "c").replacingOccurrences(
        of: "sh",
        with: "s"
    ).replacingOccurrences(of: "zh", with: "z")

    // Change "eung", "erng", "eong" -> "oeng"
    out = out.replacingOccurrences(of: "eung", with: "oeng")
        .replacingOccurrences(of: "erng", with: "oeng")

    out = out.replacingOccurrences(of: "eui", with: "eoi")
    out = out.replacingOccurrences(of: "euk", with: "oek")
    out = out.replacingOccurrences(of: "eun", with: "(eo|yu)n")
    out = out.replacingOccurrences(of: "eut", with: "(eo|yu)t")
    out = out.replacingOccurrences(of: "eu", with: "(e|y)u")
    out = out.replacingOccurrences(of: "ern", with: "eon")

    // Change "-oen" -> "-eon"
    var idx = out.range(of: "oen")
    while idx != nil {
        if idx!.upperBound < out.endIndex && out[idx!.upperBound] == "g" {
            idx = out.range(of: "oen", range: idx!.upperBound..<out.endIndex)
            continue
        }
        out.replaceSubrange(idx!, with: "eon")
        idx = out.range(of: "oen", range: idx!.upperBound..<out.endIndex)
    }
    out = out.replacingOccurrences(of: "oei", with: "eoi")
    out = out.replacingOccurrences(of: "oet", with: "eot")

    out = out.replacingOccurrences(of: "eong ", with: "oeng ")
    out = out.replacingOccurrences(of: "eong'", with: "oeng'")
    if out.hasSuffix("eong") {
        out.replaceSubrange(
            out.range(of: "eong", options: [.backwards])!,
            with: "oeng"
        )
    }
    if unsafeSubstitutions {
        out = out.replacingOccurrences(of: "eong", with: "oeng")  // unsafe because of zeon6 gun2
    }
    out = out.replacingOccurrences(of: "eok", with: "oek")

    out = out.replacingOccurrences(of: "ao ", with: "au ")
    out = out.replacingOccurrences(of: "ao'", with: "au'")
    if out.hasSuffix("ao") {
        out.replaceSubrange(
            out.range(of: "ao", options: [.backwards])!,
            with: "au"
        )
    }
    if unsafeSubstitutions {
        out = out.replacingOccurrences(of: "ao", with: "au")  // unsafe because of maa5 on1
    }

    out = out.replacingOccurrences(of: "ar", with: "aa")  // like in "char siu"
    out = out.replacingOccurrences(of: "ee", with: "i")  // like in "lai see"
    out = out.replacingOccurrences(of: "ay", with: "ei")  // like in "gong hay fat choy"
    out = out.replacingOccurrences(of: "oy", with: "oi")  // like in "choy sum"
    out = out.replacingOccurrences(of: "oo", with: "(y!u)")  // like in "soot goh"
    out = out.replacingOccurrences(of: "ong", with: "(o|u)ng")
    out = out.replacingOccurrences(of: "young", with: "jung")  // like in "foo young"
    out = out.replacingOccurrences(of: "yue", with: "(j)yu")  // like "yuet yue" (粵語)
    out = out.replacingOccurrences(of: "ue", with: "(yu)")  // like "tsuen wan" (轉彎)
    out = out.replacingOccurrences(of: "tsz", with: "zi")  // like "tsat tsz mui" (七姊妹)
    out = out.replacingOccurrences(of: "ck", with: "k")  // like "back" (白)

    // The following changes may be unsafe because it is ambiguous whether
    // they are final + initial or a "misspelling" of a final
    // However, it is unambiguous if there is a separator at the end of the
    // syllable, or it is the end of the string

    // Check if the user intends to write an [-ɔː h-] or [-ou̯] cluster
    do {
        out = out.replacingOccurrences(of: "oh ", with: "ou ")
            .replacingOccurrences(of: "oh'", with: "ou'")
        if out.hasSuffix("oh") {
            out.replaceSubrange(
                out.range(of: "oh", options: [.backwards])!,
                with: "ou"
            )
        }

        // Initials for which <initial> + "-ou" exist in Jyutping
        let closeBackVowelCluster: Set = ["n", "j"]
        // Initials for which both "-ou" and "-o h-" exist in Jyutping
        let ambiguousVowelCluster: Set = [
            "b", "p", "m", "f", "d", "t", "l", "g", "h", "w", "z", "c", "s",
        ]

        var replacementIdx = out.range(of: "oh")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "ou")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeBackVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "ou")
                } else if ambiguousVowelCluster.contains(
                    String(out[initialIdx])
                ) {
                    // The [h-] cluster can only occur if what follows is not an initial
                    var initialFound = false
                    for initialLen in (1...2).reversed() {
                        let offset = min(
                            out.distance(
                                from: replacementIdx!.lowerBound,
                                to: out.endIndex
                            ),
                            2 + initialLen
                        )
                        let endIdx = out.index(
                            replacementIdx!.lowerBound,
                            offsetBy: offset
                        )
                        let s = String(
                            out[
                                out.index(
                                    replacementIdx!.lowerBound,
                                    offsetBy: 2
                                )..<endIdx
                            ]
                        )
                        if initials.contains(s) || s == "y" {
                            initialFound = true
                        }
                    }

                    if initialFound {
                        out.replaceSubrange(replacementIdx!, with: "ou")
                    } else {
                        out.replaceSubrange(replacementIdx!, with: "o h")
                    }
                }
                break
            }
            replacementIdx = out.range(
                of: "oh",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-ɔː w-] or [-auː] cluster
    do {
        out = out.replacingOccurrences(of: "ow ", with: "au ")
            .replacingOccurrences(of: "ow'", with: "au'")
        if out.hasSuffix("ow") {
            out.replaceSubrange(
                out.range(of: "ow", options: [.backwards])!,
                with: "au"
            )
        }

        // Initials for which <initial> + "-(a)au" exist in Jyutping
        let closeBackVowelCluster: Set = ["b", "m", "k", "s"]
        // Initials for which both exist in Jyutping
        let ambiguousVowelCluster: Set = [
            "p", "m", "f", "d", "t", "n", "l", "g", "h", "z", "c", "s",
        ]

        var replacementIdx = out.range(of: "ow")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "au")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeBackVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "au")
                } else if ambiguousVowelCluster.contains(
                    String(out[initialIdx])
                ) {
                    // The [w-] cluster can only occur if what follows is not an initial
                    var initialFound = false
                    for initialLen in (1...2).reversed() {
                        let offset = min(
                            out.distance(
                                from: replacementIdx!.lowerBound,
                                to: out.endIndex
                            ),
                            2 + initialLen
                        )
                        let endIdx = out.index(
                            replacementIdx!.lowerBound,
                            offsetBy: offset
                        )
                        let s = String(
                            out[
                                out.index(
                                    replacementIdx!.lowerBound,
                                    offsetBy: 2
                                )..<endIdx
                            ]
                        )
                        if initials.contains(s) || s == "y" {
                            initialFound = true
                        }
                    }

                    if initialFound {
                        out.replaceSubrange(replacementIdx!, with: "au")
                    } else {
                        out.replaceSubrange(replacementIdx!, with: "o w")
                    }
                }
                break
            }
            replacementIdx = out.range(
                of: "ow",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [ɐm] or [-uː  m-] cluster
    do {
        out = out.replacingOccurrences(of: "um ", with: "am ")
            .replacingOccurrences(of: "um'", with: "am'")
        if out.hasSuffix("um") {
            out.replaceSubrange(
                out.range(of: "um", options: [.backwards])!,
                with: "am"
            )
        }

        // Initials for which <initial> + "am" exist in Jyutping
        let openMidCentralVowelCluster: Set = [
            "b", "p", "m", "d", "t", "n", "l", "k", "h", "z", "c", "s", "j",
        ]
        // Initials for which <initial> + "-u m-" exist in Jyutping
        let closeBackVowelCluster: Set = ["f", "w", "a", "e", "i", "o"]

        var replacementIdx = out.range(of: "um")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "am")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if openMidCentralVowelCluster.contains(String(out[initialIdx]))
                {
                    out.replaceSubrange(replacementIdx!, with: "am")
                } else if closeBackVowelCluster.contains(
                    String(out[initialIdx])
                ) {
                    ()
                } else if String(out[initialIdx]) == "g" {
                    // The [m-] cluster can only occur if what follows is not an initial
                    var initialFound = false
                    for initialLen in (1...2).reversed() {
                        let offset = min(
                            out.distance(
                                from: replacementIdx!.lowerBound,
                                to: out.endIndex
                            ),
                            2 + initialLen
                        )
                        let endIdx = out.index(
                            replacementIdx!.lowerBound,
                            offsetBy: offset
                        )
                        let s = String(
                            out[
                                out.index(
                                    replacementIdx!.lowerBound,
                                    offsetBy: 2
                                )..<endIdx
                            ]
                        )
                        if initials.contains(s) || s == "y" {
                            initialFound = true
                        }
                    }

                    if initialFound {
                        out.replaceSubrange(replacementIdx!, with: "am")
                    } else {
                        out.replaceSubrange(replacementIdx!, with: "u m")
                    }
                }
                break
            }
            replacementIdx = out.range(
                of: "um",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-ɛː j-] or [-ei̯] cluster
    do {
        out = out.replacingOccurrences(of: "ey ", with: "ei ")
            .replacingOccurrences(of: "ey'", with: "ei'")
        if out.hasSuffix("ey") {
            out.replaceSubrange(
                out.range(of: "ey", options: [.backwards])!,
                with: "ei"
            )
        }

        // Initials for which <initial> + "-ei" exist in Jyutping
        let closeFrontVowelCluster: Set = ["p", "f", "d", "n", "l", "h", "w"]
        // Initials for which <initial> + "-e j-" exist in Jyutping
        let openMidFrontVowelCluster: Set = ["c", "j", "y"]
        // Initials for which <initial> + "-u m-" exist in Jyutping
        let ambiguousVowelCluster: Set = ["b", "m", "g", "k", "z", "s"]

        var replacementIdx = out.range(of: "ey")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "ei")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeFrontVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "ei")
                } else if openMidFrontVowelCluster.contains(
                    String(out[initialIdx])
                ) {
                    out.replaceSubrange(replacementIdx!, with: "e (j)")
                } else if ambiguousVowelCluster.contains(
                    String(out[initialIdx])
                ) {
                    // The [j-] cluster can only occur if what follows is not an initial
                    var initialFound = false
                    for initialLen in (1...2).reversed() {
                        let offset = min(
                            out.distance(
                                from: replacementIdx!.lowerBound,
                                to: out.endIndex
                            ),
                            2 + initialLen
                        )
                        let endIdx = out.index(
                            replacementIdx!.lowerBound,
                            offsetBy: offset
                        )
                        let s = String(
                            out[
                                out.index(
                                    replacementIdx!.lowerBound,
                                    offsetBy: 2
                                )..<endIdx
                            ]
                        )
                        if initials.contains(s) || s == "y" {
                            initialFound = true
                        }
                    }

                    if initialFound {
                        out.replaceSubrange(replacementIdx!, with: "ei")
                    } else {
                        out.replaceSubrange(replacementIdx!, with: "e (j)")
                    }
                }
                break
            }
            replacementIdx = out.range(
                of: "ey",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-yː j-] or [-ɐm] cluster
    do {
        // Initials for which <initial> + "-yu m-" exist in Jyutping
        let closeFrontVowelCluster: Set = ["z", "c", "s", "j"]
        var replacementIdx = out.range(of: "yum")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "(j)am")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeFrontVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "yu m")
                } else {
                    out.replaceSubrange(replacementIdx!, with: "(j)am")
                }
                break
            }
            replacementIdx = out.range(
                of: "yum",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-yː p-] or [-ɐp] cluster
    do {
        // Initials for which <initial> + "-yu p-" exist in Jyutping
        let closeFrontVowelCluster: Set = ["z", "s", "j"]
        var replacementIdx = out.range(of: "yup")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "(j)ap")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeFrontVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "yu p")
                } else {
                    out.replaceSubrange(replacementIdx!, with: "(j)ap")
                }
                break
            }
            replacementIdx = out.range(
                of: "yup",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-yː k-] or [jʊk] cluster
    do {
        // Initials for which <initial> + "-yu k-" exist in Jyutping
        let closeFrontVowelCluster: Set = ["z", "s", "c", "j"]
        var replacementIdx = out.range(of: "yuk")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "(j)uk")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeFrontVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "yu k")
                } else {
                    out.replaceSubrange(replacementIdx!, with: "(j)uk")
                }
                break
            }
            replacementIdx = out.range(
                of: "yuk",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-yn g-] or [jʊŋ] cluster
    do {
        // Initials for which <initial> + "-yun g-" exist in Jyutping
        let closeFrontVowelCluster: Set = ["z", "s", "c", "j"]
        var replacementIdx = out.range(of: "yung")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "(j)ung")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeFrontVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "(yu)n g")
                } else {
                    out.replaceSubrange(replacementIdx!, with: "(j)ung")
                }
                break
            }
            replacementIdx = out.range(
                of: "yung",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-yn] or [jɐn], [jyn], [yn] cluster
    do {
        // Initials for which <initial> + "-yu n-" exist in Jyutping
        let closeFrontVowelCluster: Set = ["z", "s", "c", "j"]
        var replacementIdx = out.range(of: "yun")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "(j)(a|yu)n")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeFrontVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "yun")
                } else {
                    out.replaceSubrange(replacementIdx!, with: "(ja|jyu|yu)n")
                }
                break
            }
            replacementIdx = out.range(
                of: "yun",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Check if the user intends to write an [-yt] or [jɐt], [jyt], [yt] cluster
    do {
        // Initials for which <initial> + "-yu n-" exist in Jyutping
        let closeFrontVowelCluster: Set = ["z", "s", "c", "j"]
        var replacementIdx = out.range(of: "yut")
        while replacementIdx != nil {
            switch replacementIdx!.lowerBound {
            case out.startIndex:
                out.replaceSubrange(replacementIdx!, with: "(j)(a|yu)t")
                break
            default:
                var initialIdx = out.index(before: replacementIdx!.lowerBound)
                if replacementIdx!.lowerBound.utf16Offset(in: out) > 1
                    && out[initialIdx] == ")"
                {
                    initialIdx = out.index(before: initialIdx)
                }

                if closeFrontVowelCluster.contains(String(out[initialIdx])) {
                    out.replaceSubrange(replacementIdx!, with: "(yu)t")
                } else {
                    out.replaceSubrange(replacementIdx!, with: "(ja|jyu|yu)t")
                }
                break
            }
            replacementIdx = out.range(
                of: "yut",
                range: replacementIdx!.upperBound..<out.endIndex
            )
        }
    }

    // Unsafe because it is ambiguous whether these are final + initial
    // or a "misspelling" of an initial
    // But unambiguous if they are at the start of a syllable
    if let r = out.range(of: "ts") {
        if r.lowerBound == out.startIndex {
            out.replaceSubrange(r, with: "c")
        }
    }
    out = out.replacingOccurrences(of: " ts", with: " c")
    if let r = out.range(of: "kwu") {
        if r.lowerBound == out.startIndex {
            out.replaceSubrange(r, with: "(g|k)w!u")
        }
    }
    out = out.replacingOccurrences(of: " kwu", with: " (g|k)w!u")
    if unsafeSubstitutions {
        out = out.replacingOccurrences(of: "ts", with: "c")
        out = out.replacingOccurrences(of: "kwu", with: "(g|k)w!u")
    }

    // Change any "y" that is not followed by a "u" to "j"
    // This needs to happen before the final replacements
    var yIdx = out.range(of: "y")
    while yIdx != nil {
        if String(
            out[yIdx!.lowerBound..<out.index(yIdx!.lowerBound, offsetBy: 2)]
        ) == "yu"
            || String(
                out[yIdx!.lowerBound..<out.index(yIdx!.lowerBound, offsetBy: 3)]
            ) == "y!u"
            || String(
                out[yIdx!.lowerBound..<out.index(yIdx!.lowerBound, offsetBy: 3)]
            ) == "y)u"
        {
            yIdx = out.range(
                of: "y",
                range: out.index(after: yIdx!.lowerBound)..<out.endIndex
            )
            continue
        }
        out.replaceSubrange(yIdx!, with: "(j)")
        yIdx = out.range(
            of: "y",
            range: out.index(after: yIdx!.lowerBound)..<out.endIndex
        )
    }

    out = out.replacingOccurrences(of: "ui", with: "(eo|u)i")
    out = out.replacingOccurrences(of: "un", with: "(y!u|a|eo)n")
    out = out.replacingOccurrences(of: "ut", with: "(a|y!u)t")

    out = out.replacingOccurrences(of: "o ", with: "(ou!) ")
    out = out.replacingOccurrences(of: "o'", with: "(ou!)'")
    if out.hasSuffix("o") {
        out.replaceSubrange(
            out.index(before: out.endIndex)..<out.endIndex,
            with: "(ou!)"
        )
    }
    var oIdx = out.range(of: "o")
    while oIdx != nil {
        let letterAfter = String(out[out.index(after: oIdx!.lowerBound)])
        if !["k", "t", "e", "i", "u", "n", ")", "|"].contains(letterAfter) {
            out.replaceSubrange(oIdx!, with: "(ou!)")
            oIdx = out.range(
                of: "o",
                range: out.index(after: oIdx!.lowerBound)..<out.endIndex
            )
            continue
        }
        oIdx = out.range(
            of: "o",
            range: out.index(after: oIdx!.lowerBound)..<out.endIndex
        )
    }

    return out
}

func jyutpingSoundChanges(text: [String]) -> [String] {
    text
}
