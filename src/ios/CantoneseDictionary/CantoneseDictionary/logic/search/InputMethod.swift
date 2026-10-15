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

  var descriptionString: String {
    switch self {
    case .simplified:
      return "Simplified"
    case .traditional:
      return "Traditional"
    case .jyutping:
      return "Jyutping"
    case .fuzzyJyutping:
      return "Fuzzy Jyutping"
    case .pinyin:
      return "Pinyin"
    case .fuzzyPinyin:
      return "Fuzzy Pinyin"
    case .english:
      return "English"
    case .autoDetect:
      return "Auto-detect language"
    case .none:
      return "None"
    }
  }

  var id: Int { rawValue }
}
