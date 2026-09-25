//
//  MandarinUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-15.
//

import Foundation
import os

nonisolated let pinyinInitials: Set<String> = [
  "b", "p", "m", "f", "d", "t",
  "n", "l", "g", "k", "h", "j",
  "q", "x", "zh", "ch", "sh", "r",
  "z", "c", "s", "y",
]
nonisolated let pinyinFinals: Set<String> = [
  "a", "e", "ai", "ei", "ao", "ou", "an", "ang", "en",
  "eng", "ong", "er", "i", "ia", "ie", "iao", "iu",
  "ian", "in", "iang", "ing", "iong", "ua", "uo", "uai",
  "ui", "uan", "un", "uang", "u", "u:", "ue", "u:e", "o",
]

nonisolated let pinyinToneReplacements: [String: [String]] = [
  "a": ["ā", "á", "ǎ", "à", "a"],
  "e": ["ē", "é", "ě", "è", "e"],
  "i": ["ī", "í", "ǐ", "ì", "i"],
  "o": ["ō", "ó", "ǒ", "ò", "o"],
  "u": ["ū", "ú", "ǔ", "ù", "u"],
  "ü": ["ǖ", "ǘ", "ǚ", "ǜ", "ü"],
]

nonisolated let pinyinVPrecederRegex: String = "([jqx])u"

nonisolated let zhuyinInitials: [String: String] = [
  "b": "ㄅ", "p": "ㄆ", "m": "ㄇ", "f": "ㄈ", "d": "ㄉ",
  "t": "ㄊ", "n": "ㄋ", "l": "ㄌ", "g": "ㄍ", "k": "ㄎ",
  "h": "ㄏ", "j": "ㄐ", "q": "ㄑ", "x": "ㄒ", "z": "ㄗ",
  "c": "ㄘ", "s": "ㄙ", "r": "ㄖ", "zh": "ㄓ", "ch": "ㄔ",
  "sh": "ㄕ",
]
nonisolated let zhuyinFinals: [String: String] = [
  "yuan": "ㄩㄢ", "iang": "ㄧㄤ", "yang": "ㄧㄤ", "uang": "ㄨㄤ",
  "wang": "ㄨㄤ", "ying": "ㄧㄥ", "weng": "ㄨㄥ", "iong": "ㄩㄥ",
  "yong": "ㄩㄥ", "uai": "ㄨㄞ", "wai": "ㄨㄞ", "yai": "ㄧㄞ",
  "iao": "ㄧㄠ", "yao": "ㄧㄠ", "ian": "ㄧㄢ", "yan": "ㄧㄢ",
  "uan": "ㄨㄢ", "wan": "ㄨㄢ", "van": "ㄩㄢ", "ang": "ㄤ",
  "yue": "ㄩㄝ", "wei": "ㄨㄟ", "you": "ㄧㄡ", "yin": "ㄧㄣ",
  "wen": "ㄨㄣ", "yun": "ㄩㄣ", "eng": "ㄥ", "ing": "ㄧㄥ",
  "ong": "ㄨㄥ", "io": "ㄧㄛ", "yo": "ㄧㄛ", "ia": "ㄧㄚ",
  "ya": "ㄧㄚ", "ua": "ㄨㄚ", "wa": "ㄨㄚ", "ai": "ㄞ",
  "ao": "ㄠ", "an": "ㄢ", "ie": "ㄧㄝ", "ye": "ㄧㄝ",
  "uo": "ㄨㄛ", "wo": "ㄨㄛ", "ue": "ㄩㄝ", "ve": "ㄩㄝ",
  "ei": "ㄟ", "ui": "ㄨㄟ", "ou": "ㄡ", "iu": "ㄧㄡ",
  "en": "ㄣ", "in": "ㄧㄣ", "un": "ㄨㄣ", "vn": "ㄩㄣ",
  "yi": "ㄧ", "wu": "ㄨ", "yu": "ㄩ", "a": "ㄚ",
  "e": "ㄜ", "o": "ㄛ", "i": "ㄧ", "u": "ㄨ",
  "v": "ㄩ", "ê": "ㄝ",
]
nonisolated let zhuyinTones: [String] = ["", "", "ˊ", "ˇ", "ˋ", "˙"]

nonisolated let zhuyinIPrecederRegex: String = "([zcs]h?)i"
nonisolated let zhuyinRRegex: String = "([r])i"
nonisolated let zhuyinNgSpecialCaseRegex: String = "^ng([012345])$"
nonisolated let zhuyinHmSpecialCaseRegex: String = "^hm([012345])$"
nonisolated let zhuyinHngSpecialCaseRegex: String = "^hng([012345])$"
nonisolated let zhuyinErSpecialCaseRegex: String = "^er([012345])$"
nonisolated(unsafe) let zhuyinInitialRegex: Regex = try! Regex(
  "^([bpmfdtnlgkhjqxzcsr]?h?)"
)
nonisolated(unsafe) let zhuyinFinalRegex: Regex = try! Regex(
  "([aeiouêvyw]?[aeioun]?[aeioung]?[ng]?)(r?)([012345])$"
)

nonisolated let mandarinIPAGlottal: Set = [
  "a", "o", "e", "ai", "ei", "ao", "ou", "an", "en", "er", "ang", "ong",
  "eng",
]

nonisolated let mandarinIPAInitials: [String: String] = [
  "b": "p", "c": "t͡sʰ", "ch": "ʈ͡ʂʰ", "d": "t", "f": "f",
  "g": "k", "h": "x", "j": "t͡ɕ", "k": "kʰ", "l": "l",
  "m": "m", "n": "n", "ng": "ŋ", "p": "pʰ", "q": "t͡ɕʰ",
  "r": "ʐ", "s": "s", "sh": "ʂ", "t": "tʰ", "x": "ɕ",
  "z": "t͡s", "zh": "ʈ͡ʂ",
]
nonisolated let mandarinIPAFinals: [String: String] = [
  "a": "ä", "ai": "aɪ̯", "air": "ɑɻ", "an": "än",
  "ang": "ɑŋ", "angr": "ɑ̃ɻ", "anr": "ɑɻ", "ao": "ɑʊ̯",
  "aor": "aʊ̯ɻʷ", "ar": "ɑɻ", "e": "ɤ", "ei": "eɪ̯",
  "eir": "əɻ", "en": "ən", "eng": "ɤŋ", "engr": "ɤ̃ɻ",
  "enr": "əɻ", "er": "ɤɻ", "i": "i", "ia": "jä",
  "ian": "jɛn", "iang": "jɑŋ", "iangr": "jɑ̃ɻ", "ianr": "jɑɻ",
  "iao": "jɑʊ̯", "iaor": "jaʊ̯ɻʷ", "iar": "jɑɻ", "ie": "jɛ",
  "ier": "jɛɻ", "in": "in", "ing": "iŋ", "ingr": "iɤ̯̃ɻ",
  "inr": "iə̯ɻ", "io": "jɔ", "iong": "jʊŋ", "iongr": "jʊ̃ɻ",
  "ir": "iə̯ɻ", "iu": "joʊ̯", "iur": "jɤʊ̯ɻʷ", "m": "m̩",
  "n": "n̩", "ng": "ŋ̍", "o": "wɔ", "ong": "ʊŋ",
  "ongr": "ʊ̃ɻ", "or": "wɔɻ", "ou": "oʊ̯", "our": "ɤʊ̯ɻʷ",
  "u": "u", "ua": "wä", "uai": "waɪ̯", "uair": "wɑɻ",
  "uan": "wän", "uang": "wɑŋ", "uangr": "wɑ̃ɻ", "uanr": "wɑɻ",
  "uar": "u̯ɑɻ", "ue": "ɥɛ", "ui": "weɪ̯", "uir": "wəɻ",
  "un": "wən", "unr": "wəɻ", "uo": "wɔ", "uor": "wɔɻ",
  "ur": "uɻʷ", "v": "y", "van": "ɥɛn", "vanr": "ɥɑɻ",
  "ve": "ɥɛ", "ver": "ɥɛɻ", "vn": "yn", "vnr": "yə̯ɻ",
  "vr": "yə̯ɻ", "wa": "wä", "wai": "waɪ̯", "wair": "wɑɻ",
  "wan": "wän", "wang": "wɑŋ", "wangr": "wɑ̃ɻ", "wanr": "wɑɻ",
  "war": "wɑɻ", "wei": "weɪ̯", "weir": "wəɻ", "wen": "wən",
  "weng": "wəŋ", "wengr": "ʊ̃ɻ", "wenr": "wəɻ", "wo": "wɔ",
  "wor": "wɔɻ", "wu": "u", "wur": "uɻʷ", "ya": "jä",
  "yai": "jaɪ̯", "yan": "jɛn", "yang": "jɑŋ", "yangr": "jɑ̃ɻ",
  "yanr": "jɑɻ", "yao": "jɑʊ̯", "yaor": "jaʊ̯ɻʷ", "yar": "jɑɻ",
  "ye": "jɛ", "yer": "jɛɻ", "yi": "i", "yin": "in",
  "ying": "iŋ", "yingr": "iɤ̯̃ɻ", "yinr": "iə̯ɻ", "yir": "iə̯ɻ",
  "yo": "jɔ", "yong": "jʊŋ", "yongr": "jʊ̃ɻ", "yor": "jɔɻ",
  "you": "joʊ̯", "your": "jɤʊ̯ɻʷ", "yu": "y", "yuan": "ɥɛn",
  "yuanr": "ɥɑɻ", "yue": "ɥɛ", "yuer": "ɥɛɻ", "yun": "yn",
  "yunr": "yə̯ɻ", "yur": "yə̯ɻ",
]
nonisolated let mandarinIPAVoicelessInitials: [String: String] = [
  "k": "g̊",
  "p": "b̥",
  "t": "d̥",
  "t͡s": "d͡z̥",
  "t͡ɕ": "d͡ʑ̥",
  "ʈ͡ʂ": "ɖ͡ʐ̥",
]
nonisolated let mandarinIPANeutralTone: [String] = ["˨", "˧", "˦", "˩", "˩"]
nonisolated let mandarinIPAThirdTone: [String] = [
  "˨˩˦꜕꜖꜖", "˨˩˦꜕꜖꜖", "˨˩˦꜔꜒", "˨˩˦꜕꜖꜖", "˨˩˦",
]
nonisolated let mandarinIPATones: [String] = ["˥˥", "˧˥", "˨˩˦", "˥˩", ""]
nonisolated(unsafe) let mandarinIPASyllableRegex: Regex = try! Regex(
  "^([bcdfghjklmnpqrstxz]?h?)(.+)$"
)

nonisolated func createPrettyPinyin(pinyin: String) -> String {
  if pinyin.isEmpty {
    return ""
  }

  var processedSyllables: [String] = []
  let (_, syllables) = segmentPinyin(text: pinyin)
  if syllables.isEmpty {
    return pinyin
  }

  for s in syllables {
    var syllable = s
    if specialCharacters.contains(syllable) {
      processedSyllables.append(syllable)
      continue
    }

    let tones = ["0", "1", "2", "3", "4", "5"]
    guard var toneIdx = syllable.firstIndex(where: { tones.contains(String($0)) }) else {
      processedSyllables.append(syllable)
      continue
    }
    var tone = Int(syllable[toneIdx..<syllable.index(after: toneIdx)]) ?? 0
    if tone < 1 || tone > 5 {
      tone = 5
    }

    syllable = syllable.replacingOccurrences(of: "u:", with: "ü")

    let firstVowels = ["a", "e", "o"]
    var vowelIdx: Range<String.Index>? = nil
    for v in firstVowels {
      vowelIdx = syllable.range(of: v)
      if vowelIdx != nil {
        break
      }
    }
    if vowelIdx == nil {
      let lastVowels = ["i", "u", "ü"]
      for v in lastVowels {
        vowelIdx = syllable.range(of: v, options: [.backwards])
        if vowelIdx != nil {
          break
        }
      }
    }
    guard vowelIdx != nil else {
      processedSyllables.append(syllable)
      continue
    }

    syllable.replaceSubrange(
      vowelIdx!,
      with: pinyinToneReplacements[String(syllable[vowelIdx!])]![tone - 1]
    )

    toneIdx = syllable.firstIndex(where: {
      tones.contains(String($0))
    })!
    syllable.replaceSubrange(
      toneIdx..<syllable.index(after: toneIdx),
      with: ""
    )
    processedSyllables.append(syllable)
  }

  return processedSyllables.joined(separator: " ")
}

nonisolated func createNumberedPinyin(pinyin: String) -> String {
  return pinyin.replacingOccurrences(of: "u:", with: "ü")
}

nonisolated func createPinyinWithV(pinyin: String) -> String {
  return pinyin.replacingOccurrences(of: "u:", with: "v")
}

// Note that the majority of this code is derivative of Wiktionary's conversion
// code, contained in the module cmn-pron
// (https://en.wiktionary.org/wiki/Module:cmn-pron)
nonisolated func convertPinyinToZhuyin(
  pinyin: String,
  useSpacesToSegment: Bool = false
) -> String {
  if pinyin.isEmpty {
    return pinyin
  }

  var syllables: [String] = []
  var pinyinCopy = ""
  if useSpacesToSegment {
    // Insert a space before and after every special character, so that the
    // IPA conversion doesn't attempt to convert special characters.
    specialCharacters.forEach { c in
      pinyinCopy = pinyin.split(separator: c).joined(
        separator: " " + String(c) + " "
      )
    }
    syllables = pinyinCopy.split(separator: " ").map(String.init)
  } else {
    let (_, result) = segmentPinyin(
      text: pinyin,
      removeSpecialCharacters: false,
      removeGlobCharacters: false
    )
    syllables = result
  }

  var zhuyinSyllables: [String] = []
  for syllable in syllables {
    if syllable.count == 1 {
      zhuyinSyllables.append(syllable)
      continue
    }

    // Skip syllables that are just punctuation
    if specialCharacters.contains(syllable) {
      zhuyinSyllables.append(syllable)
      continue
    }

    // Skip syllables that don't have tone
    let tones = ["0", "1", "2", "3", "4", "5"]
    guard
      let toneIdx = syllable.firstIndex(where: {
        tones.contains(String($0))
      })
    else {
      zhuyinSyllables.append(syllable)
      continue
    }
    let tone = Int(syllable[toneIdx..<syllable.index(after: toneIdx)]) ?? 5

    var zhuyinSyllable = syllable
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: "u:",
      with: "v"
    )
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: pinyinVPrecederRegex,
      with: "$1v",
      options: [.regularExpression]
    )
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: zhuyinIPrecederRegex,
      with: "$1",
      options: [.regularExpression]
    )
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: zhuyinRRegex,
      with: "$1",
      options: [.regularExpression]
    )

    // Handle special cases
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: zhuyinNgSpecialCaseRegex,
      with: "ㄫ$1$1",
      options: [.regularExpression]
    )
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: zhuyinHmSpecialCaseRegex,
      with: "ㄏㄇ$1",
      options: [.regularExpression]
    )
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: zhuyinHngSpecialCaseRegex,
      with: "ㄏㄫ$1",
      options: [.regularExpression]
    )
    zhuyinSyllable = zhuyinSyllable.replacingOccurrences(
      of: zhuyinErSpecialCaseRegex,
      with: "ㄦ$1",
      options: [.regularExpression]
    )

    // Handle general case
    // Convert Pinyin initial
    guard
      let initialMatch = zhuyinSyllable.firstMatch(of: zhuyinInitialRegex)
    else {
      logger.error(
        "Invalid pinyin initial found in zhuyin conversion: \(pinyin)"
      )
      return pinyin
    }
    if let initial = initialMatch[1].substring, !initial.isEmpty {
      zhuyinSyllable.replaceSubrange(
        initialMatch.range,
        with: zhuyinInitials[String(initial)]!
      )
    }

    // Convert Pinyin final
    guard let finalMatch = zhuyinSyllable.firstMatch(of: zhuyinFinalRegex)
    else {
      logger.error(
        "Invalid pinyin final found in zhuyin conversion: \(pinyin)"
      )
      zhuyinSyllables.append(syllable)
      return pinyin
    }
    var final: String? = nil
    if let finalComponent = finalMatch[1].substring, !finalComponent.isEmpty {
      final = zhuyinFinals[String(finalComponent)]
      guard final != nil else {
        logger.error(
          "Pinyin final had no valid zhuyin conversion: \(finalComponent)"
        )
        zhuyinSyllables.append(syllable)
        continue
      }
    }
    var er: String = ""
    if let hasErSuffix = finalMatch[2].substring, !hasErSuffix.isEmpty {
      er = "ㄦ"
    }
    zhuyinSyllable.replaceSubrange(
      finalMatch.range,
      with: (final ?? "") + er
    )

    // Add tone to Zhuyin syllable
    if tone == 5 {
      zhuyinSyllable = zhuyinTones[tone] + zhuyinSyllable
    } else {
      if let erIdx = zhuyinSyllable.range(of: "ㄦ"), zhuyinSyllable != "ㄦ" {
        zhuyinSyllable.insert(
          contentsOf: zhuyinTones[tone],
          at: erIdx.lowerBound
        )
      } else {
        zhuyinSyllable = zhuyinSyllable + zhuyinTones[tone]
      }
    }

    zhuyinSyllables.append(zhuyinSyllable)
  }

  return zhuyinSyllables.joined(separator: " ")
}

private nonisolated func convertIPAMandarinSyllable(
  syllable: String
) -> (initial: String, final: String) {
  var ipaInitial: String = ""
  var ipaFinal: String = ""

  if syllable == "ng" {
    ipaFinal = mandarinIPAFinals["ng"]!
  } else {
    guard let match = syllable.firstMatch(of: mandarinIPASyllableRegex)
    else {
      logger.error("Invalid pinyin for IPA conversion: \(syllable)")
      return ("", syllable)
    }

    ipaInitial = mandarinIPAInitials[String(match[1].substring!)] ?? ""
    ipaFinal = mandarinIPAFinals[String(match[2].substring!)] ?? ""

    if ipaInitial.isEmpty && ipaFinal.isEmpty {
      logger.error("Could not find initial or final in: \(syllable)")
      return ("", syllable)
    }
  }

  // Replace close front unrounded vowel with syllabic retroflex sibilant
  // fricative (+ voiced retroflex approximant if erhua)
  // in Pinyin starting with ch, sh, zh, or r
  if ipaInitial == "ʈ͡ʂʰ" || ipaInitial == "ʂ" || ipaInitial == "ʈ͡ʂ"
    || ipaInitial == "ʐ"
  {
    if ipaFinal == "ir" {
      ipaFinal = "ʐ̩ɻ"
    } else if ipaFinal == "i" {
      ipaFinal = "ʐ̩"
    }
  }

  // Replace close front unrounded vowel with syllabic alveolar sibilant
  // fricative (+ voiced retroflex approximant if erhua)
  // in Pinyin starting with c, s, or z
  if ipaInitial == "t͡sʰ" || ipaInitial == "s" || ipaInitial == "t͡s" {
    if ipaFinal == "ir" {
      ipaFinal = "z̩ɻ"
    } else if ipaFinal == "i" {
      ipaFinal = "z̩"
    }
  }

  // Do some cleanup for Pinyin like "ri"
  if ipaInitial == "ʐ" && ipaFinal == "ʐ̩" {
    ipaInitial = ""
  }

  return (ipaInitial, ipaFinal)
}

nonisolated func convertPinyinToIPA(
  pinyin: String,
  useSpacesToSegment: Bool = false
) -> String {
  if pinyin.isEmpty {
    return pinyin
  }

  var syllables: [String] = []
  var pinyinCopy = ""
  if useSpacesToSegment {
    // Insert a space before and after every special character, so that the
    // IPA conversion doesn't attempt to convert special characters.
    specialCharacters.forEach { c in
      pinyinCopy = pinyin.split(separator: c).joined(
        separator: " " + String(c) + " "
      )
    }
    syllables = pinyinCopy.split(separator: " ").map(String.init)
  } else {
    let (_, result) = segmentPinyin(
      text: pinyin,
      removeSpecialCharacters: false,
      removeGlobCharacters: false
    )
    syllables = result
  }

  // Pre-compute list of tones corresponding to each syllable
  // This is used for tone sandhi reasons (3->3 sandhi, x->5 sandhi, etc.)
  var syllableTones: [(Int, String.Index?)] = []
  let tones = ["0", "1", "2", "3", "4", "5"]
  for syllable in syllables {
    let toneIdx = syllable.firstIndex(where: {
      tones.contains(String($0))
    })
    if toneIdx == nil {
      syllableTones.append((-1, nil))
      continue
    } else {
      let tone =
        Int(syllable[toneIdx!..<syllable.index(after: toneIdx!)]) ?? 5
      syllableTones.append((tone, toneIdx))
    }
  }

  var ipaSyllables: [String] = []
  for (i, syllable) in syllables.enumerated() {
    var ipaGlottal: String = ""
    var ipaInitial: String = ""
    var ipaFinal: String = ""
    var ipaTone: String = ""

    if syllable.count == 1 {
      ipaSyllables.append(syllable)
      continue
    }

    // Skip syllables that are just punctuation
    if specialCharacters.contains(syllable) {
      ipaSyllables.append(syllable)
      continue
    }

    // Get syllable without tone
    let (tone, toneIdx) = syllableTones[i]
    guard toneIdx != nil else {
      ipaSyllables.append(syllable)
      continue
    }
    var tonelessSyllable = String(syllable[..<toneIdx!])

    // Figure out whether this syllable needs a glottal stop
    if mandarinIPAGlottal.contains(tonelessSyllable) {
      ipaGlottal = "ˀ"
    }

    // Mark close front rounded vowel with v instead of "u" or "u:"
    tonelessSyllable = tonelessSyllable.replacingOccurrences(
      of: "u:",
      with: "v"
    )
    tonelessSyllable = tonelessSyllable.replacingOccurrences(
      of: pinyinVPrecederRegex,
      with: "$1v",
      options: [.regularExpression]
    )

    // Convert initial and final
    (ipaInitial, ipaFinal) = convertIPAMandarinSyllable(syllable: tonelessSyllable)

    // Convert tones
    let nextTone = (i == syllables.count - 1) ? -1 : syllableTones[i + 1].0
    let previousTone = (i == 0) ? -1 : syllableTones[i - 1].0

    switch tone {
    case 5:
      // When neutral tone, replace some initials with voiceless versions
      ipaInitial = mandarinIPAVoicelessInitials[ipaInitial] ?? ipaInitial
      ipaFinal = (ipaFinal == "ɤ") ? "ə" : ipaFinal
      ipaTone =
        (previousTone == -1)
        ? "" : mandarinIPANeutralTone[previousTone - 1]
    case 3:
      if i == syllables.count - 1 {
        ipaTone = (i == 0) ? "˨˩˦" : "˨˩˦꜕꜖(꜓)"
      } else {
        // If next syllable doesn't have tone, default to no tone sandhi
        // (which is also what happens when the following tone is tone #5
        ipaTone =
          nextTone == -1
          ? mandarinIPAThirdTone[4]
          : mandarinIPAThirdTone[nextTone - 1]
      }
    case 4:
      if nextTone == 4 {
        ipaTone = "˥˩꜒꜔"
      } else {
        ipaTone = mandarinIPATones[tone - 1]
      }
    default:
      ipaTone = mandarinIPATones[tone - 1]
    }

    ipaSyllables.append(ipaGlottal + ipaInitial + ipaFinal + ipaTone)
  }

  return ipaSyllables.joined(separator: " ")
}

nonisolated func segmentPinyin(
  text: String,
  removeSpecialCharacters: Bool = true,
  removeGlobCharacters: Bool = true
) -> (Bool, [String]) {
  let processedText = text
  var syllables: [String] = []

  var validPinyin = true
  var startIdx = processedText.startIndex
  var endIdx = processedText.startIndex
  var initialFound = false

  while endIdx < processedText.endIndex {
    var nextIteration = false
    var currentString = String(processedText[endIdx]).lowercased()
    let isSpecialCharacter = specialCharacters.contains(currentString)
    let isGlobCharacter =
      currentString.trimmingCharacters(in: .whitespacesAndNewlines) == "*"
      || currentString.trimmingCharacters(in: .whitespacesAndNewlines)
        == "?"

    if currentString == " " || currentString == "'" || isSpecialCharacter
      || isGlobCharacter
    {
      if initialFound {
        let previousInitial = String(processedText[startIdx..<endIdx])
          .lowercased()
        syllables.append(previousInitial)

        if !pinyinFinals.contains(previousInitial) {
          validPinyin = false
        }
        startIdx = endIdx
        initialFound = false
      }
      if !removeGlobCharacters && isGlobCharacter {
        if endIdx > processedText.startIndex
          && processedText.distance(from: startIdx, to: endIdx) >= 1
        {
          let previousInitial = String(
            processedText[startIdx..<endIdx]
          ).lowercased()
          syllables.append(previousInitial)
          if !pinyinFinals.contains(previousInitial) {
            validPinyin = false
          }
          initialFound = false
        }

        var globStartIdx = endIdx
        if endIdx > processedText.startIndex
          && processedText[processedText.index(before: endIdx)] == " "
          && syllables.last?.last != " "
        {
          globStartIdx = processedText.index(before: globStartIdx)
        }
        if processedText.distance(
          from: endIdx,
          to: processedText.endIndex
        ) > 1
          && processedText[processedText.index(after: endIdx)] == " "
        {
          endIdx = processedText.index(after: endIdx)
        }
        let glob = String(processedText[globStartIdx...endIdx])
          .lowercased()
        syllables.append(glob)
        startIdx = endIdx
      } else if !removeSpecialCharacters && isSpecialCharacter {
        syllables.append(currentString)
      }

      startIdx = processedText.index(after: startIdx)
      endIdx = processedText.index(after: endIdx)
      continue
    }

    // First, check for initials
    // If initial is valid, then extend the end_index for length of initial
    // cluster of consonants.
    for initialLen in [1, 2].reversed() {
      let offset = min(
        processedText.distance(
          from: endIdx,
          to: processedText.endIndex
        ),
        initialLen
      )
      currentString = String(
        processedText[
          endIdx..<processedText.index(endIdx, offsetBy: offset)
        ]
      ).lowercased()
      if pinyinInitials.contains(currentString) {
        if initialFound {
          validPinyin = false
        }
        endIdx = processedText.index(endIdx, offsetBy: offset)
        nextIteration = true
        initialFound = true
        break
      }
    }

    if nextIteration {
      continue
    }

    // Then, check for finals
    // If final is valid, then extend end_index for length of final.
    // Check for number at end of word as well (this represents tone number).
    //
    // Then add the substring from [start_index, end_index) to list
    // and reset start_index, so we can start searching after the end_index.
    for finalLen in (1...4).reversed() {
      let offset = min(
        processedText.distance(
          from: endIdx,
          to: processedText.endIndex
        ),
        finalLen
      )
      currentString = String(
        processedText[
          endIdx..<processedText.index(endIdx, offsetBy: offset)
        ]
      ).lowercased()
      if pinyinFinals.contains(currentString) {
        endIdx = processedText.index(endIdx, offsetBy: offset)

        if endIdx < processedText.endIndex
          && String(processedText[endIdx]) == "r"
        {
          endIdx = processedText.index(after: endIdx)
        }
        if endIdx < processedText.endIndex
          && processedText[endIdx].isNumber
        {
          let tone = Int(String(processedText[endIdx]))
          if tone == nil || tone! < 1 || tone! > 5 {
            validPinyin = false
          }
          endIdx = processedText.index(after: endIdx)
        }

        let syllable = processedText[startIdx..<endIdx].lowercased()
        syllables.append(syllable)
        startIdx = endIdx
        nextIteration = true
        initialFound = false
        break
      }
    }

    if nextIteration {
      continue
    }

    endIdx = processedText.index(after: endIdx)
  }

  // Then add whatever's left in the search term, minus whitespace
  let lastSyllable = processedText[startIdx..<endIdx].trimmingCharacters(
    in: .whitespacesAndNewlines
  ).lowercased()
  if !lastSyllable.isEmpty && lastSyllable != "'" {
    syllables.append(lastSyllable)
    if !pinyinFinals.contains(lastSyllable) {
      validPinyin = false
    }
  }

  return (validPinyin, syllables)
}

nonisolated func pinyinSoundChanges(text: [String]) -> [String] {
  var syllables: [String] = []

  for s in text {
    var syllable = s
    if syllable.hasPrefix("zh") {
      syllable = "z(h)!" + syllable.dropFirst(2)
    } else if syllable.hasPrefix("z") {
      syllable = "z(h)!" + syllable.dropFirst()
    }
    if syllable.hasPrefix("ch") {
      syllable = "c(h)!" + syllable.dropFirst(2)
    } else if syllable.hasPrefix("c") {
      syllable = "c(h)!" + syllable.dropFirst()
    }
    if syllable.hasPrefix("sh") {
      syllable = "s(h)!" + syllable.dropFirst(2)
    } else if syllable.hasPrefix("s") {
      syllable = "s(h)!" + syllable.dropFirst()
    }
    if syllable.hasPrefix("n") {
      syllable = "(n|l)" + syllable.dropFirst()
    } else if syllable.hasPrefix("r") {
      syllable = "(l|r)" + syllable.dropFirst()
    }
    if syllable.hasPrefix("li") || syllable.hasPrefix("lu:") {
      syllable = "(l|n)" + syllable.dropFirst()
    } else if syllable.hasPrefix("l") {
      syllable = "(l|n|r)" + syllable.dropFirst()
    }

    if syllable.hasSuffix("ang") || syllable.dropLast().hasSuffix("ang"),
      let range = syllable.range(of: "ng", options: [.backwards])
    {
      syllable.replaceSubrange(range, with: "ng!")
    } else if syllable.hasSuffix("an") || syllable.dropLast().hasSuffix("an"),
      let range = syllable.range(of: "n", options: [.backwards])
    {
      syllable.replaceSubrange(range, with: "ng!")
    }
    if syllable.hasSuffix("eng") || syllable.dropLast().hasSuffix("eng"),
      let range = syllable.range(of: "ng", options: [.backwards])
    {
      syllable.replaceSubrange(range, with: "ng!")
    } else if syllable.hasSuffix("en") || syllable.dropLast().hasSuffix("en"),
      let range = syllable.range(of: "n", options: [.backwards])
    {
      syllable.replaceSubrange(range, with: "ng!")
    }
    if syllable.hasSuffix("ing") || syllable.dropLast().hasSuffix("ing"),
      let range = syllable.range(of: "ng", options: [.backwards])
    {
      syllable.replaceSubrange(range, with: "ng!")
    } else if syllable.hasSuffix("in") || syllable.dropLast().hasSuffix("in"),
      let range = syllable.range(of: "n", options: [.backwards])
    {
      syllable.replaceSubrange(range, with: "ng!")
    }

    syllables.append(syllable)
  }

  return syllables
}
