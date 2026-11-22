//
//  Example.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import Foundation

nonisolated struct Example: Hashable, Identifiable {
  let id = UUID()
  private var _sourceLanguage: String
  private var _traditional: String
  private var _simplified: String

  private var _jyutping: String
  private var _pinyin: String

  private var _translations: [TranslationSet]

  init(
    sourceLanguage: String,
    traditional: String,
    simplified: String,
    jyutping: String,
    pinyin: String,
    translations: [TranslationSet]
  ) {
    self._sourceLanguage = sourceLanguage
    self._traditional = traditional
    self._simplified = simplified
    self._jyutping = jyutping.lowercased()
    self._pinyin = pinyin.lowercased()
    self._translations = translations
  }

  public static func == (lhs: Example, rhs: Example) -> Bool {
    lhs.id == rhs.id
  }

  public func hash(into hasher: inout Hasher) {
    hasher.combine(id)
  }

  var sourceLanguage: String {
    get {
      _sourceLanguage
    }
    set {
      _sourceLanguage = newValue
    }
  }

  func getCharacters(options: EntryCharactersOptions) -> String {
    switch options {
    case .onlyTraditional, .preferTraditional:
      return _traditional
    case .onlySimplified, .preferSimplified:
      return _simplified
    }
  }

  var traditional: String {
    get {
      _traditional
    }
    set {
      _traditional = newValue
    }
  }

  var simplified: String {
    get {
      _simplified
    }
    set {
      _simplified = newValue
    }
  }

  func getPhonetic(
    options: EntryPhoneticOptions,
    cantoneseOptions: CantoneseOptions = [],
    mandarinOptions: MandarinOptions = []
  ) -> String {
    switch options {
    case .onlyCantonese, .preferCantonese:
      return getCantonesePhonetic(cantoneseOptions: cantoneseOptions)
    case .onlyMandarin, .preferMandarin:
      return getMandarinPhonetic(mandarinOptions: mandarinOptions)
    }
  }

  func getCantonesePhonetic(cantoneseOptions: CantoneseOptions) -> String {
    switch cantoneseOptions {
    case .prettyYale:
      return PhoneticCache.shared.value(for: .prettyYale, jyutping: _jyutping) {
        convertJyutpingToYale(jyutping: _jyutping)
      }
    case .cantoneseIPA:
      return PhoneticCache.shared.value(for: .prettyYale, jyutping: _jyutping) {
        convertJyutpingToIPA(jyutping: _jyutping)
      }
    case .rawJyutping:
      fallthrough
    default:
      return _jyutping
    }
  }

  func getMandarinPhonetic(mandarinOptions: MandarinOptions) -> String {
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

  var pinyin: String {
    get {
      _pinyin
    }
    set {
      _pinyin = newValue.lowercased()
    }
  }

  func getTranslationSets() -> [TranslationSet] {
    _translations
  }

  func getTranslationSnippet() -> String {
    if _translations.isEmpty {
      return ""
    }

    let translationSet: TranslationSet = _translations[0]
    let snippets: [Translation] = translationSet.getTranslationSnippet()
    if snippets.isEmpty {
      return ""
    }

    return snippets[0].content
  }
}
