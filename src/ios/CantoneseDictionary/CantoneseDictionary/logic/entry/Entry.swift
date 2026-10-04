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

  mutating func set(_ member: Self, enabled: Bool) {
    if enabled { insert(member) } else { remove(member) }
  }

  var id: Self { self }
}

nonisolated class Entry: Hashable, Identifiable, @unchecked Sendable {
  private var _traditional: String
  private var _traditionalDifference: String
  private var _preferTraditional: String
  private var _simplified: String
  private var _simplifiedDifference: String
  private var _preferSimplified: String

  private var _colouredTraditional: AttributedString?
  private var _colouredTraditionalDifference: AttributedString?
  private var _colouredPreferTraditional: AttributedString?
  private var _colouredSimplified: AttributedString?
  private var _colouredSimplifiedDifference: AttributedString?
  private var _colouredPreferSimplified: AttributedString?
  private var _cachedCharacterHash: Int?

  private var _jyutping: String
  private var _yale: String?
  private var _cantoneseIPA: String?
  private var _jyutpingNumbers: [Int]?

  private var _pinyin: String
  private var _prettyPinyin: String?
  private var _numberedPinyin: String?
  private var _zhuyin: String?
  private var _mandarinIPA: String?
  private var _pinyinNumbers: [Int]?

  private var _definitions: [DefinitionsSet]?
  private var _definitionSnippet: String?

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
    self._jyutping = jyutping
    self._pinyin = pinyin
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
    var hasher = Hasher()
    hasher.combine(options)
    hasher.combine(useColours)
    hasher.combine(colourPhoneticType)
    let colours = (colourPhoneticType == .mandarin) ? pinyinToneColours : jyutpingToneColours
    for colour in colours {
      hasher.combine(colour)
    }
    let hash = hasher.finalize()

    if _cachedCharacterHash == nil || _cachedCharacterHash != hash {
      refreshColours(
        type: colourPhoneticType, jyutpingToneColours: jyutpingToneColours,
        pinyinToneColours: pinyinToneColours)
      _cachedCharacterHash = hash
    }

    switch options {
    case .onlyTraditional:
      return
        (useColours
        ? _colouredTraditional : AttributedString(_traditional)) ?? ""
    case .onlySimplified:
      return
        (useColours
        ? _colouredSimplified : AttributedString(_simplified)) ?? ""
    case .preferTraditional:
      return
        (useColours
        ? _colouredPreferTraditional
        : AttributedString(_preferTraditional))
        ?? ""
    case .preferSimplified:
      return
        (useColours
        ? _colouredPreferSimplified
        : AttributedString(_preferSimplified))
        ?? ""
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
    var hasher = Hasher()
    hasher.combine(options)
    hasher.combine(useColours)
    hasher.combine(colourPhoneticType)
    let colours = (colourPhoneticType == .mandarin) ? pinyinToneColours : jyutpingToneColours
    for colour in colours {
      hasher.combine(colour)
    }
    let hash = hasher.finalize()

    if _cachedCharacterHash == nil || _cachedCharacterHash != hash {
      refreshColours(
        type: colourPhoneticType, jyutpingToneColours: jyutpingToneColours,
        pinyinToneColours: pinyinToneColours)
      _cachedCharacterHash = hash
    }

    switch options {
    case .onlyTraditional, .preferTraditional:
      return
        (useColours
        ? _colouredPreferTraditional
        : AttributedString(_preferTraditional))
        ?? ""
    case .onlySimplified, .preferSimplified:
      return
        (useColours
        ? _colouredSimplified : AttributedString(_simplified)) ?? ""
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

  func generatePhonetic(
    cantoneseOptions: CantoneseOptions,
    mandarinOptions: MandarinOptions
  ) {
    if (cantoneseOptions.rawValue & CantoneseOptions.prettyYale.rawValue
      == CantoneseOptions.prettyYale.rawValue) && _yale == nil
    {
      _yale = convertJyutpingToYale(jyutping: _jyutping)
    }
    if (cantoneseOptions.rawValue & CantoneseOptions.cantoneseIPA.rawValue
      == CantoneseOptions.cantoneseIPA.rawValue) && _cantoneseIPA == nil
    {
      _cantoneseIPA = convertJyutpingToIPA(jyutping: _jyutping)
    }

    if (mandarinOptions.rawValue & MandarinOptions.prettyPinyin.rawValue
      == MandarinOptions.prettyPinyin.rawValue) && _prettyPinyin == nil
    {
      _prettyPinyin = createPrettyPinyin(pinyin: _pinyin)
    }
    if (mandarinOptions.rawValue & MandarinOptions.numberedPinyin.rawValue
      == MandarinOptions.numberedPinyin.rawValue)
      && _numberedPinyin == nil
    {
      _numberedPinyin = createNumberedPinyin(pinyin: _pinyin)
    }
    if (mandarinOptions.rawValue & MandarinOptions.zhuyin.rawValue
      == MandarinOptions.zhuyin.rawValue) && _zhuyin == nil
    {
      _zhuyin = convertPinyinToZhuyin(pinyin: _pinyin)
    }
    if (mandarinOptions.rawValue & MandarinOptions.mandarinIPA.rawValue
      == MandarinOptions.mandarinIPA.rawValue) && _mandarinIPA == nil
    {
      _mandarinIPA = convertPinyinToIPA(pinyin: _pinyin)
    }
  }

  func generateDefinitionsPhonetic(
    cantoneseOptions: CantoneseOptions,
    mandarinOptions: MandarinOptions
  ) {
    for d in _definitions ?? [] {
      d.generatePhonetic(cantoneseOptions, mandarinOptions)
    }
  }

  func getPhonetic(
    options: EntryPhoneticOptions,
    cantoneseOptions: CantoneseOptions,
    mandarinOptions: MandarinOptions
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
      if _yale == nil {
        generatePhonetic(
          cantoneseOptions: .prettyYale,
          mandarinOptions: []
        )
      }
      return _yale ?? "Yale not available"
    case .cantoneseIPA:
      if _cantoneseIPA == nil {
        generatePhonetic(
          cantoneseOptions: .cantoneseIPA,
          mandarinOptions: []
        )
      }
      return _cantoneseIPA ?? "Cantonese IPA not available"
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
      if _prettyPinyin == nil {
        generatePhonetic(
          cantoneseOptions: [],
          mandarinOptions: .prettyPinyin
        )
      }
      return _prettyPinyin ?? "Pretty pinyin not available"
    case .numberedPinyin:
      if _numberedPinyin == nil {
        generatePhonetic(
          cantoneseOptions: [],
          mandarinOptions: .numberedPinyin
        )
      }
      return _numberedPinyin ?? "Numbered pinyin not available"
    case .zhuyin:
      if _zhuyin == nil {
        generatePhonetic(
          cantoneseOptions: [],
          mandarinOptions: .zhuyin
        )
      }
      return _zhuyin ?? "Zhuyin not available"
    case .mandarinIPA:
      if _mandarinIPA == nil {
        generatePhonetic(
          cantoneseOptions: [],
          mandarinOptions: .mandarinIPA
        )
      }
      return _mandarinIPA ?? "Mandarin IPA not available"
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
      _jyutpingNumbers = nil
    }
  }

  func getJyutpingNumbers() -> [Int] {
    if _jyutpingNumbers == nil {
      _jyutpingNumbers = []

      let tones = ["0", "1", "2", "3", "4", "5", "6"]
      var toneIdx = _jyutping.firstIndex(where: {
        tones.contains(String($0))
      })

      while toneIdx != nil {
        if let tone = Int(String(_jyutping[toneIdx!])) {
          _jyutpingNumbers!.append(tone)
        }
        toneIdx = _jyutping[_jyutping.index(after: toneIdx!)...]
          .firstIndex(where: {
            tones.contains(String($0))
          })
      }
    }

    return _jyutpingNumbers!
  }

  var pinyin: String {
    get {
      _pinyin
    }
    set {
      _pinyin = newValue.lowercased()
      _pinyinNumbers = nil
    }
  }

  func getPinyinNumbers() -> [Int] {
    if _pinyinNumbers == nil {
      _pinyinNumbers = []

      let tones = ["0", "1", "2", "3", "4", "5"]
      var toneIdx = _pinyin.firstIndex(where: {
        tones.contains(String($0))
      })

      while toneIdx != nil {
        if let tone = Int(String(_pinyin[toneIdx!])) {
          _pinyinNumbers!.append(tone)
        }
        toneIdx = _pinyin[_pinyin.index(after: toneIdx!)...]
          .firstIndex(where: {
            tones.contains(String($0))
          })
      }
    }

    return _pinyinNumbers!
  }

  func getDefinitionsSets() -> [DefinitionsSet]? {
    return _definitions
  }

  func getDefinitionSnippet() -> String {
    if _definitions == nil || _definitions!.isEmpty {
      return _definitionSnippet ?? ""
    }

    if _definitionSnippet == nil {
      for d in _definitions ?? [] {
        guard !d.isEmpty else {
          continue
        }

        _definitionSnippet = d.getDefinitionSnippet()
        break
      }
    }

    return _definitionSnippet ?? ""
  }

  func refreshColours(
    type: EntryColourPhoneticType,
    jyutpingToneColours: [Color],
    pinyinToneColours: [Color]
  ) {
    var tones: [Int]
    switch type {
    case .none:
      _colouredTraditional = AttributedString(_traditional)
      _colouredSimplified = AttributedString(_simplified)
      _colouredTraditionalDifference = AttributedString(
        _traditionalDifference
      )
      _colouredSimplifiedDifference = AttributedString(
        _simplifiedDifference
      )
      _colouredPreferTraditional =
        _colouredTraditional! + AttributedString("[")
        + _colouredSimplifiedDifference! + AttributedString("]")
      _colouredPreferTraditional =
        _colouredSimplified! + AttributedString("[")
        + _colouredTraditionalDifference! + AttributedString("]")
      return
    case .cantonese:
      tones = getJyutpingNumbers()
    case .mandarin:
      tones = getPinyinNumbers()
    }
    _colouredTraditional = applyColours(
      text: _traditional,
      tones: tones,
      jyutpingToneColours: jyutpingToneColours,
      pinyinToneColours: pinyinToneColours,
      type: type
    )
    _colouredSimplified = applyColours(
      text: _simplified,
      tones: tones,
      jyutpingToneColours: jyutpingToneColours,
      pinyinToneColours: pinyinToneColours,
      type: type
    )
    _colouredTraditionalDifference = applyColours(
      text: _traditionalDifference,
      tones: tones,
      jyutpingToneColours: jyutpingToneColours,
      pinyinToneColours: pinyinToneColours,
      type: type
    )
    _colouredSimplifiedDifference = applyColours(
      text: _simplifiedDifference,
      tones: tones,
      jyutpingToneColours: jyutpingToneColours,
      pinyinToneColours: pinyinToneColours,
      type: type
    )

    _colouredPreferTraditional =
      _colouredTraditional! + AttributedString("[")
      + _colouredSimplifiedDifference! + AttributedString("]")
    _colouredPreferSimplified =
      _colouredSimplified! + AttributedString("[")
      + _colouredTraditionalDifference! + AttributedString("]")
  }
}
