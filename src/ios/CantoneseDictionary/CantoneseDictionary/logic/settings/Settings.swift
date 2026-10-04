//
//  Settings.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-15.
//

import SwiftUI

nonisolated let defaultJyutpingToneColours: [Color] = [
  Color(red: 0.5, green: 0.5, blue: 0.5),
  Color(red: 0.0, green: 0.737, blue: 0.831),  // #00bcd4
  Color(red: 0.486, green: 0.702, blue: 0.259),  // #7cb342
  Color(red: 0.396, green: 0.498, blue: 0.945),  // #657ff1
  Color(red: 0.761, green: 0.094, blue: 0.357),  // #c2185b
  Color(red: 0.024, green: 0.537, blue: 0.0),  // #068900
  Color(red: 0.463, green: 0.318, blue: 0.816),  // #7651d0
]

nonisolated let defaultPinyinToneColours: [Color] = [
  Color(red: 0.5, green: 0.5, blue: 0.5),
  Color(red: 0.988, green: 0.263, blue: 0.235),  // #fc433c
  Color(red: 0.008, green: 0.729, blue: 0.122),  // #02ba1f
  Color(red: 0.094, green: 0.651, blue: 0.949),  // #18a6f2
  Color(red: 0.620, green: 0.467, blue: 1.000),  // #9e77ff
  Color(red: 0.5, green: 0.5, blue: 0.5),
]

nonisolated let languageColours: [String: Color] = [
  "cmn": Color(red: 14 / 255, green: 139 / 255, blue: 83 / 255),
  "deu": Color(red: 186 / 255, green: 200 / 255, blue: 95 / 255),
  "eng": Color(red: 100 / 255, green: 76 / 255, blue: 143 / 255),
  "fra": Color(red: 0, green: 48 / 255, blue: 143 / 255),
  "yue": Color(red: 173 / 255, green: 31 / 255, blue: 90 / 255),
]

// Stolen from https://nilcoalescing.com/blog/EncodeAndDecodeSwiftUIColor/
nonisolated struct CodableColor: Codable {
  let cgColor: CGColor

  enum CodingKeys: String, CodingKey {
    case colorSpace
    case components
  }

  init(cgColor: CGColor) {
    self.cgColor = cgColor
  }

  init(from decoder: Decoder) throws {
    let container =
      try decoder
      .container(keyedBy: CodingKeys.self)
    let colorSpace =
      try container
      .decode(String.self, forKey: .colorSpace)
    let components =
      try container
      .decode([CGFloat].self, forKey: .components)

    guard
      let cgColorSpace = CGColorSpace(name: colorSpace as CFString),
      let cgColor = CGColor(
        colorSpace: cgColorSpace, components: components
      )
    else {
      throw CodingError.wrongData
    }

    self.cgColor = cgColor
  }

  func encode(to encoder: Encoder) throws {
    var container = encoder.container(keyedBy: CodingKeys.self)
    guard
      let colorSpace = cgColor.colorSpace?.name,
      let components = cgColor.components
    else {
      throw CodingError.wrongData
    }

    try container.encode(colorSpace as String, forKey: .colorSpace)
    try container.encode(components, forKey: .components)
  }
}

nonisolated enum CodingError: Error {
  case wrongColor
  case wrongData
}

@Observable @MainActor final class Settings {
  init() {
    self.jyutpingToneColours = Self.loadJyutpingToneColours()
    self.pinyinToneColours = Self.loadPinyinToneColours()

    self.previewPhoneticOptions = Self.loadPreviewPhoneticOptions()
    self.previewCantonesePhoneticOptions = Self.loadPreviewCantonesePhoneticOptions()
    self.previewMandarinPhoneticOptions = Self.loadPreviewMandarinPhoneticOptions()

    self.entryCantonesePhoneticOptions = Self.loadEntryCantonesePhoneticOptions()
    self.entryMandarinPhoneticOptions = Self.loadEntryMandarinPhoneticOptions()

    self.unsafeFuzzyJyutping = Self.loadUnsafeFuzzyJyutping()

    self.entryCharactersOptions = Self.loadEntryCharactersOptions()
    self.entryColourPhoneticType = Self.loadEntryColourPhoneticType()

    self.checkForUpdates = Self.loadCheckForUpdates()
  }

  static func loadJyutpingToneColours() -> [Color] {
    guard UserDefaults.standard.object(forKey: "jyutpingColours") != nil,
      let data = UserDefaults.standard.data(forKey: "jyutpingColours"),
      let codable = try? JSONDecoder().decode([CodableColor].self, from: data),
      !codable.isEmpty
    else {
      return defaultJyutpingToneColours
    }
    return codable.map { Color($0.cgColor) }
  }

  var jyutpingToneColours: [Color] {
    didSet {
      let codable = jyutpingToneColours.compactMap {
        $0.cgColor.map {
          CodableColor(cgColor: $0)
        }
      }
      UserDefaults.standard.set(try? JSONEncoder().encode(codable), forKey: "jyutpingColours")
    }
  }

  static func loadPinyinToneColours() -> [Color] {
    guard UserDefaults.standard.object(forKey: "pinyinColours") != nil,
      let data = UserDefaults.standard.data(forKey: "pinyinColours"),
      let codable = try? JSONDecoder().decode([CodableColor].self, from: data),
      !codable.isEmpty
    else {
      return defaultPinyinToneColours
    }
    return codable.map { Color($0.cgColor) }
  }

  var pinyinToneColours: [Color] {
    didSet {
      let codable = pinyinToneColours.compactMap {
        $0.cgColor.map {
          CodableColor(cgColor: $0)
        }
      }
      UserDefaults.standard.set(try? JSONEncoder().encode(codable), forKey: "pinyinColours")
    }
  }

  static func loadPreviewPhoneticOptions() -> EntryPhoneticOptions {
    guard UserDefaults.standard.object(forKey: "preview/entryPhoneticOptions") != nil else {
      return .preferCantonese
    }
    let option =
      EntryPhoneticOptions(
        rawValue: UserDefaults.standard.integer(forKey: "preview/entryPhoneticOptions"))
      ?? .preferCantonese
    return option
  }

  var previewPhoneticOptions: EntryPhoneticOptions {
    didSet {
      UserDefaults.standard.set(
        previewPhoneticOptions.rawValue, forKey: "preview/entryPhoneticOptions")
    }
  }

  static func loadPreviewCantonesePhoneticOptions() -> CantoneseOptions {
    guard UserDefaults.standard.object(forKey: "preview/cantonesePhoneticOptions") != nil else {
      return .rawJyutping
    }
    let options =
      CantoneseOptions(
        rawValue: UInt8(
          truncatingIfNeeded: UserDefaults.standard.integer(
            forKey: "preview/cantonesePhoneticOptions")))
    return options
  }

  var previewCantonesePhoneticOptions: CantoneseOptions {
    didSet {
      UserDefaults.standard.set(
        previewCantonesePhoneticOptions.rawValue, forKey: "preview/cantonesePhoneticOptions")
    }
  }

  static func loadPreviewMandarinPhoneticOptions() -> MandarinOptions {
    guard UserDefaults.standard.object(forKey: "preview/mandarinPhoneticOptions") != nil else {
      return .prettyPinyin
    }
    let options =
      MandarinOptions(
        rawValue: UInt8(
          truncatingIfNeeded: UserDefaults.standard.integer(
            forKey: "preview/mandarinPhoneticOptions")))
    return options
  }

  var previewMandarinPhoneticOptions: MandarinOptions {
    didSet {
      UserDefaults.standard.set(
        previewMandarinPhoneticOptions.rawValue, forKey: "preview/mandarinPhoneticOptions")
    }
  }

  static func loadEntryCantonesePhoneticOptions() -> CantoneseOptions {
    guard UserDefaults.standard.object(forKey: "entry/cantonesePhoneticOptions") != nil else {
      return .rawJyutping
    }
    let options =
      CantoneseOptions(
        rawValue: UInt8(
          truncatingIfNeeded: UserDefaults.standard.integer(
            forKey: "entry/cantonesePhoneticOptions")))
    return options
  }

  var entryCantonesePhoneticOptions: CantoneseOptions {
    didSet {
      UserDefaults.standard.set(
        entryCantonesePhoneticOptions.rawValue, forKey: "entry/cantonesePhoneticOptions")
    }
  }

  static func loadEntryMandarinPhoneticOptions() -> MandarinOptions {
    guard UserDefaults.standard.object(forKey: "entry/mandarinPhoneticOptions") != nil else {
      return .prettyPinyin
    }
    let options =
      MandarinOptions(
        rawValue: UInt8(
          truncatingIfNeeded: UserDefaults.standard.integer(
            forKey: "entry/mandarinPhoneticOptions")
        ))
    return options
  }

  var entryMandarinPhoneticOptions: MandarinOptions {
    didSet {
      UserDefaults.standard.set(
        entryMandarinPhoneticOptions.rawValue, forKey: "entry/mandarinPhoneticOptions")
    }
  }

  static func loadUnsafeFuzzyJyutping() -> Bool {
    guard UserDefaults.standard.object(forKey: "unsafeFuzzyJyutping") != nil else {
      return false
    }
    let unsafeFuzzyJyutping = UserDefaults.standard.bool(forKey: "unsafeFuzzyJyutping")
    return unsafeFuzzyJyutping
  }

  var unsafeFuzzyJyutping: Bool {
    didSet {
      UserDefaults.standard.set(unsafeFuzzyJyutping, forKey: "unsafeFuzzyJyutping")
    }
  }

  static func loadEntryCharactersOptions() -> EntryCharactersOptions {
    guard UserDefaults.standard.object(forKey: "entryCharactersOptions") != nil else {
      return .preferTraditional
    }
    let options =
      EntryCharactersOptions(
        rawValue: UserDefaults.standard.integer(forKey: "entryCharactersOptions"))
      ?? .preferTraditional
    return options
  }

  var entryCharactersOptions: EntryCharactersOptions {
    didSet {
      UserDefaults.standard.set(entryCharactersOptions.rawValue, forKey: "entryCharactersOptions")
    }
  }

  static func loadEntryColourPhoneticType() -> EntryColourPhoneticType {
    guard UserDefaults.standard.object(forKey: "entryColourPhoneticType") != nil else {
      return .cantonese
    }
    let type =
      EntryColourPhoneticType(
        rawValue: UserDefaults.standard.integer(forKey: "entryColourPhoneticType")) ?? .cantonese
    return type
  }

  var entryColourPhoneticType: EntryColourPhoneticType {
    didSet {
      UserDefaults.standard.set(entryColourPhoneticType.rawValue, forKey: "entryColourPhoneticType")
    }
  }

  static func loadCheckForUpdates() -> Bool {
    guard UserDefaults.standard.object(forKey: "checkForUpdates") != nil else {
      return true
    }
    let checkForUpdates = UserDefaults.standard.bool(forKey: "checkForUpdates")
    return checkForUpdates
  }

  var checkForUpdates: Bool {
    didSet {
      UserDefaults.standard.set(checkForUpdates, forKey: "checkForUpdates")
    }
  }
}
