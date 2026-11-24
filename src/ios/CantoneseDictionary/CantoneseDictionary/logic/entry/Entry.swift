//
//  Entry.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-03.
//

import Foundation
import SwiftUI

nonisolated enum EntryCharactersOptions: Int, CaseIterable, Codable, Equatable, Hashable,
  Identifiable, Sendable
{
  case onlyTraditional
  case onlySimplified
  case preferSimplified
  case preferTraditional

  var id: Self { self }
}

nonisolated enum EntryPhoneticOptions: Int, CaseIterable, Codable, Equatable, Hashable,
  Identifiable, Sendable
{
  case onlyCantonese
  case onlyMandarin
  case preferCantonese
  case preferMandarin

  var id: Self { self }
}

nonisolated enum EntryColourPhoneticType: Int, CaseIterable, Codable, Equatable, Hashable,
  Identifiable, Sendable
{
  case none
  case cantonese
  case mandarin

  var id: Self { self }
}

nonisolated struct CantoneseOptions: OptionSet, Hashable, Sendable {
  let rawValue: UInt8
  static let rawJyutping = Self(rawValue: 0x1)
  static let prettyYale = Self(rawValue: 0x2)
  static let cantoneseIPA = Self(rawValue: 0x4)

  mutating func set(_ member: Self, enabled: Bool) {
    if enabled { insert(member) } else { remove(member) }
  }

  var id: Self { self }
}

nonisolated struct MandarinOptions: OptionSet, Hashable, Sendable {
  let rawValue: UInt8

  static let rawPinyin = Self(rawValue: 0x1)
  static let prettyPinyin = Self(rawValue: 0x2)
  static let numberedPinyin = Self(rawValue: 0x4)
  static let zhuyin = Self(rawValue: 0x8)
  static let mandarinIPA = Self(rawValue: 0x10)
  static let rawPinyinWithV = Self(rawValue: 0x20)

  mutating func set(_ member: Self, enabled: Bool) {
    if enabled { insert(member) } else { remove(member) }
  }

  var id: Self { self }
}

nonisolated struct Entry: Hashable, Identifiable {
  private var _traditional: String
  private var _traditionalDifference: String
  private var _preferTraditional: String
  private var _simplified: String
  private var _simplifiedDifference: String
  private var _preferSimplified: String

  private var _jyutping: String
  private var _pinyin: String

  private var _definitions: [DefinitionsSet]?

  private var _rowid: Int = -1

  init(
    rowid: Int,
    traditional: String,
    simplified: String,
    jyutping: String,
    pinyin: String,
    definitions: [DefinitionsSet]
  ) {
    self._rowid = rowid
    self._traditional = traditional
    self._simplified = simplified
    self._jyutping = jyutping.lowercased()
    self._pinyin = pinyin.lowercased()
    self._definitions = definitions

    self._simplifiedDifference = compareStrings(
      original: _traditional,
      comparison: _simplified
    )
    self._traditionalDifference = compareStrings(
      original: _simplified,
      comparison: _traditional
    )
    self._preferTraditional = "\(_traditional) [\(_simplifiedDifference)]"
    self._preferSimplified = "\(_simplified) [\(_traditionalDifference)]"
  }

  var id: Int {
    _rowid
  }

  public static func == (lhs: Entry, rhs: Entry) -> Bool {
    lhs.id == rhs.id
  }

  public func hash(into hasher: inout Hasher) {
    hasher.combine(id)
  }

  func getCharacters(
    options: EntryCharactersOptions,
    useColours: Bool,
    colourPhoneticType: EntryColourPhoneticType,
    jyutpingToneColours: [Color],
    pinyinToneColours: [Color]
  )
    -> AttributedString
  {
    switch options {
    case .onlyTraditional:
      if useColours {
        var tones: [Int]
        switch colourPhoneticType {
        case .none:
          return AttributedString(_traditional)
        case .cantonese:
          tones = getJyutpingNumbers()
        case .mandarin:
          tones = getPinyinNumbers()
        }
        return applyColours(
          text: _traditional,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
      } else {
        return AttributedString(_traditional)
      }
    case .onlySimplified:
      if useColours {
        var tones: [Int]
        switch colourPhoneticType {
        case .none:
          return AttributedString(_simplified)
        case .cantonese:
          tones = getJyutpingNumbers()
        case .mandarin:
          tones = getPinyinNumbers()
        }
        return applyColours(
          text: _simplified,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
      } else {
        return AttributedString(_simplified)
      }
    case .preferTraditional:
      if useColours {
        var tones: [Int]
        switch colourPhoneticType {
        case .none:
          return AttributedString(_traditional) + AttributedString(" [")
            + AttributedString(_simplifiedDifference) + AttributedString("]")
        case .cantonese:
          tones = getJyutpingNumbers()
        case .mandarin:
          tones = getPinyinNumbers()
        }
        let colouredTraditional = applyColours(
          text: _traditional,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
        let colouredSimplifiedDifference = applyColours(
          text: _simplifiedDifference,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
        return colouredTraditional + AttributedString(" [")
          + colouredSimplifiedDifference + AttributedString("]")
      } else {
        return AttributedString(traditional) + AttributedString(" [")
          + AttributedString(_simplifiedDifference) + AttributedString("]")
      }
    case .preferSimplified:
      if useColours {
        var tones: [Int]
        switch colourPhoneticType {
        case .none:
          return AttributedString(_simplified) + AttributedString(" [")
            + AttributedString(_traditionalDifference) + AttributedString("]")
        case .cantonese:
          tones = getJyutpingNumbers()
        case .mandarin:
          tones = getPinyinNumbers()
        }
        let colouredSimplified = applyColours(
          text: _simplified,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
        let colouredTraditionalDifference = applyColours(
          text: _traditionalDifference,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
        return colouredSimplified + AttributedString(" [")
          + colouredTraditionalDifference + AttributedString("]")
      } else {
        return AttributedString(simplified) + AttributedString(" [")
          + AttributedString(_traditionalDifference) + AttributedString("]")
      }
    }
  }

  func getCharactersNoSecondary(
    options: EntryCharactersOptions,
    useColours: Bool,
    colourPhoneticType: EntryColourPhoneticType,
    jyutpingToneColours: [Color],
    pinyinToneColours: [Color]
  )
    -> AttributedString
  {
    switch options {
    case .onlyTraditional, .preferTraditional:
      if useColours {
        var tones: [Int]
        switch colourPhoneticType {
        case .none:
          return AttributedString(_traditional)
        case .cantonese:
          tones = getJyutpingNumbers()
        case .mandarin:
          tones = getPinyinNumbers()
        }
        return applyColours(
          text: _traditional,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
      } else {
        return AttributedString(_traditional)
      }
    case .onlySimplified, .preferSimplified:
      if useColours {
        var tones: [Int]
        switch colourPhoneticType {
        case .none:
          return AttributedString(_simplified)
        case .cantonese:
          tones = getJyutpingNumbers()
        case .mandarin:
          tones = getPinyinNumbers()
        }
        return applyColours(
          text: _simplified,
          tones: tones,
          jyutpingToneColours: jyutpingToneColours,
          pinyinToneColours: pinyinToneColours,
          type: colourPhoneticType
        )
      } else {
        return AttributedString(_simplified)
      }
    }
  }

  var traditional: String {
    get {
      _traditional
    }
    set {
      _traditional = newValue

      _simplifiedDifference = compareStrings(
        original: _traditional,
        comparison: _simplified
      )
      _traditionalDifference = compareStrings(
        original: _simplified,
        comparison: _traditional
      )
      _preferTraditional =
        "\(_traditional) [\(_simplifiedDifference)]"
      _preferSimplified =
        "\(_simplified) [\(_traditionalDifference)]"
    }
  }

  var simplified: String {
    get {
      _simplified
    }
    set {
      _simplified = newValue

      _simplifiedDifference = compareStrings(
        original: _traditional,
        comparison: _simplified
      )
      _traditionalDifference = compareStrings(
        original: _simplified,
        comparison: _traditional
      )
      _preferTraditional =
        "\(_traditional) [\(_simplifiedDifference)]"
      _preferSimplified =
        "\(_simplified) [\(_traditionalDifference)]"
    }
  }

  func getPhonetic(
    options: EntryPhoneticOptions,
    cantoneseOptions: CantoneseOptions = [],
    mandarinOptions: MandarinOptions = []
  ) -> String {
    switch options {
    case .onlyCantonese:
      return getCantonesePhonetic(
        cantoneseOptions: cantoneseOptions
      )
    case .onlyMandarin:
      return getMandarinPhonetic(mandarinOptions: mandarinOptions)
    case .preferCantonese:
      let cantonese = getCantonesePhonetic(
        cantoneseOptions: cantoneseOptions
      )
      let mandarin = getMandarinPhonetic(
        mandarinOptions: mandarinOptions
      )
      if cantonese.isEmpty {
        return mandarin.isEmpty ? "" : "(\(mandarin))"
      } else if mandarin.isEmpty {
        return cantonese
      } else {
        return "\(cantonese) (\(mandarin))"
      }
    case .preferMandarin:
      let cantonese = getCantonesePhonetic(
        cantoneseOptions: cantoneseOptions
      )
      let mandarin = getMandarinPhonetic(
        mandarinOptions: mandarinOptions
      )
      if mandarin.isEmpty {
        return cantonese.isEmpty ? "" : "(\(cantonese))"
      } else if cantonese.isEmpty {
        return mandarin
      } else {
        return "\(mandarin) (\(cantonese))"
      }
    }
  }

  func getCantonesePhonetic(cantoneseOptions: CantoneseOptions) -> String {
    switch cantoneseOptions {
    case .prettyYale:
      return PhoneticCache.shared.value(for: .prettyYale, jyutping: _jyutping) {
        convertJyutpingToYale(jyutping: _jyutping)
      }
    case .cantoneseIPA:
      return PhoneticCache.shared.value(for: .cantoneseIPA, jyutping: _jyutping) {
        convertJyutpingToIPA(jyutping: _jyutping)
      }
    case .rawJyutping:
      fallthrough
    default:
      return _jyutping
    }
  }

  func getMandarinPhonetic(mandarinOptions: MandarinOptions)
    -> String
  {
    switch mandarinOptions {
    case .prettyPinyin:
      return PhoneticCache.shared.value(for: .prettyPinyin, pinyin: _pinyin) {
        createPrettyPinyin(pinyin: _pinyin)
      }
    case .numberedPinyin:
      return PhoneticCache.shared.value(for: .numberedPinyin, pinyin: _pinyin) {
        createNumberedPinyin(pinyin: _pinyin)
      }
    case .zhuyin:
      return PhoneticCache.shared.value(for: .zhuyin, pinyin: _pinyin) {
        convertPinyinToZhuyin(pinyin: _pinyin)
      }
    case .mandarinIPA:
      return PhoneticCache.shared.value(for: .mandarinIPA, pinyin: _pinyin) {
        convertPinyinToIPA(pinyin: _pinyin)
      }
    case .rawPinyinWithV:
      return PhoneticCache.shared.value(for: .rawPinyinWithV, pinyin: _pinyin) {
        createPinyinWithV(pinyin: _pinyin)
      }
    default:
      return _pinyin
    }
  }

  var jyutping: String {
    get {
      _jyutping
    }
    set {
      _jyutping = newValue.lowercased()
    }
  }

  func getJyutpingNumbers() -> [Int] {
    var jyutpingNumbers: [Int] = []

    let tones = ["0", "1", "2", "3", "4", "5", "6"]
    var toneIdx = _jyutping.firstIndex(where: {
      tones.contains(String($0))
    })

    while toneIdx != nil {
      if let tone = Int(String(_jyutping[toneIdx!])) {
        jyutpingNumbers.append(tone)
      }
      toneIdx = _jyutping[_jyutping.index(after: toneIdx!)...]
        .firstIndex(where: {
          tones.contains(String($0))
        })
    }

    return jyutpingNumbers
  }

  var pinyin: String {
    get {
      _pinyin
    }
    set {
      _pinyin = newValue.lowercased()
    }
  }

  func getPinyinNumbers() -> [Int] {
    var pinyinNumbers: [Int] = []

    let tones = ["0", "1", "2", "3", "4", "5"]
    var toneIdx = _pinyin.firstIndex(where: {
      tones.contains(String($0))
    })

    while toneIdx != nil {
      if let tone = Int(String(_pinyin[toneIdx!])) {
        pinyinNumbers.append(tone)
      }
      toneIdx = _pinyin[_pinyin.index(after: toneIdx!)...]
        .firstIndex(where: {
          tones.contains(String($0))
        })
    }

    return pinyinNumbers
  }

  func getDefinitionsSets() -> [DefinitionsSet]? {
    return _definitions
  }

  var definitionSnippet: String {
    if _definitions == nil || _definitions!.isEmpty {
      return ""
    }

    var definitionSnippet = ""
    for d in _definitions ?? [] {
      guard !d.isEmpty else {
        continue
      }

      definitionSnippet = d.definitionSnippet
      break
    }

    return definitionSnippet
  }
}
