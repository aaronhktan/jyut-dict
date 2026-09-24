//
//  InputMethod.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-12.
//

enum InputMethod: Int, CaseIterable, Identifiable {
  case simplified = 0
  case traditional = 1

  case jyutping = 2
  case fuzzyJyutping = 3
  case pinyin = 4
  case fuzzyPinyin = 5

  case english = 6

  case autoDetect = 1000
  case none = 1001

  var id: Int { rawValue }
}

nonisolated let InputMethodNames: [InputMethod: String] = [
  .simplified: "Simplified Chinese",
  .traditional: "Traditional Chinese",
  .jyutping: "Jyutping",
  .fuzzyJyutping: "Fuzzy Jyutping",
  .pinyin: "Pinyin",
  .fuzzyPinyin: "Fuzzy Pinyin",
  .english: "English",
  .autoDetect: "Auto-detect language",
  .none: "None",
]
