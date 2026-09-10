//
//  CantoneseUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import Foundation
import os

let logger = Logger()

let specialCharacters: Set = [
    ".", "。", ",", "，", "！", "？", "%", "－", "…", "⋯",
    ".", "·", "\"", "“", "”", "$", "｜", "：", "１", "２",
    "３", "４", "５", "６", "７", "８", "９", "０",
]
let regexCharacters: Set = ["!", "(", ")", "|"]

let initials: Set = [
    "b", "p", "m", "f", "d",
    "t", "n", "l", "g", "k",
    "ng", "h", "gw", "kw", "w",
    "z", "c", "s", "j", "m",
]
let finals: Set = [
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

let jyutpingToYaleSpecialSyllables: [String: [String]] = [
    "m": ["m̄", "ḿ", "m", "m̀h", "ḿh", "mh"],
    "ng": ["n̄g", "ńg", "ng", "ǹgh", "ńgh", "ngh"],
]
let jyutpingToYaleSpecialFinals: [String: String] = [
    "aa": "a",
    "oe": "eu",
    "oeng": "eung",
    "oek": "euk",
    "eoi": "eui",
    "eon": "eun",
    "eot": "eut",
]
let yaleToneReplacements: [String: [String]] = [
    "a": ["ā", "á", "a", "à", "á", "a"],
    "e": ["ē", "é", "e", "è", "é", "e"],
    "i": ["ī", "í", "i", "ì", "í", "i"],
    "o": ["ō", "ó", "o", "ò", "ó", "o"],
    "u": ["ū", "ú", "u", "ù", "ú", "u"],
]
let yaleYInitialRegex: String = "jy?"
let yaleJInitialRegex: String = "z"
let yaleChInitialRegex: String = "c"
let yaleLightToneClusterRegex: NSRegularExpression = try! NSRegularExpression(
    pattern: "([ptkmn]?g?)[123456]$"
)

let cantoneseIPASpecialSyllables: [(String, String)] = [
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
let cantoneseIPAInitials: [String: String] = [
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
let cantoneseIPANuclei: [String: String] = [
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
let cantoneseIPACodas: [String: String] = [
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

private func unfoldJyutpingRegex(jyutping: String) -> [String] {
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

func segmentJyutping(
    text: String,
    removeSpecialCharacters: Bool = true,
    removeGlobCharacters: Bool = true,
    removeRegexCharacters: Bool = true
) -> [String] {
    // TODO: Implement
    [text]
}

func jyutpingAutocorrect(text: String, unsafeSubstitutions: Bool = false)
    -> String
{
    text
}

func jyutpingSoundChanges(text: [String]) -> [String] {
    text
}
