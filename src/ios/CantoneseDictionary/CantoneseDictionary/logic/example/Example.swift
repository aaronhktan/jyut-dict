//
//  Example.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import Foundation

nonisolated class Example: Hashable, Identifiable, @unchecked Sendable {
  let id: UUID
  private var _sourceLanguage: String
  private var _simplified: String
  private var _traditional: String

  private var _jyutping: String
  private var _yale: String?
  private var _cantoneseIPA: String?

  private var _pinyin: String
  private var _prettyPinyin: String?
  private var _numberedPinyin: String?
  private var _zhuyin: String?
  private var _mandarinIPA: String?

  private var _translations: [TranslationSet]

  private var _isWelcome: Bool = false
  private var _isEmpty: Bool = false

  init(
    sourceLanguage: String,
    simplified: String,
    traditional: String,
    jyutping: String,
    pinyin: String,
    translations: [TranslationSet]
  ) {
    self.id = UUID()
    self._sourceLanguage = sourceLanguage
    self._simplified = simplified
    self._traditional = traditional
    self._jyutping = jyutping
    self._pinyin = pinyin
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
    case .onlySimplified, .preferSimplified:
      return _simplified
    case .onlyTraditional, .preferTraditional:
      return _traditional
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

  var traditional: String {
    get {
      _traditional
    }
    set {
      _traditional = newValue
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

  func getPhonetic(
    options: EntryPhoneticOptions,
    cantoneseOptions: CantoneseOptions,
    mandarinOptions: MandarinOptions
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
      if _yale == nil {
        generatePhonetic(
          cantoneseOptions: .prettyYale,
          mandarinOptions: .none
        )
      }
      return _yale ?? "Yale not available"
    case .cantoneseIPA:
      if _cantoneseIPA == nil {
        generatePhonetic(
          cantoneseOptions: .cantoneseIPA,
          mandarinOptions: .none
        )
      }
      return _cantoneseIPA ?? "Cantonese IPA not available"
    case .rawJyutping:
      fallthrough
    default:
      return _jyutping
    }
  }

  func getMandarinPhonetic(mandarinOptions: MandarinOptions) -> String {
    switch mandarinOptions {
    case .prettyPinyin:
      if _prettyPinyin == nil {
        generatePhonetic(
          cantoneseOptions: .none,
          mandarinOptions: .prettyPinyin
        )
      }
      return _prettyPinyin ?? "Pretty pinyin not available"
    case .numberedPinyin:
      if _numberedPinyin == nil {
        generatePhonetic(
          cantoneseOptions: .none,
          mandarinOptions: .numberedPinyin
        )
      }
      return _numberedPinyin ?? "Numbered pinyin not available"
    case .zhuyin:
      if _zhuyin == nil {
        generatePhonetic(
          cantoneseOptions: .none,
          mandarinOptions: .zhuyin
        )
      }
      return _zhuyin ?? "Zhuyin not available"
    case .mandarinIPA:
      if _mandarinIPA == nil {
        generatePhonetic(
          cantoneseOptions: .none,
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
      _jyutping = newValue
    }
  }

  var pinyin: String {
    get {
      _pinyin
    }
    set {
      _pinyin = newValue
    }
  }

  func getPrettyPinyin() -> String {
    _prettyPinyin ?? ""
  }

  func getTranslationSets() -> [TranslationSet] {
    _translations
  }

  func getTranslationSnippet() -> String {
    if _translations.isEmpty {
      return ""
    }

    let translationSet: TranslationSet = _translations[0]
    if translationSet.getTranslationSnippet().isEmpty {
      return ""
    }

    let snippets: [Translation] = translationSet.getTranslationSnippet()

    if snippets.isEmpty {
      return ""
    }

    return snippets[0].content
  }

  func getSentenceSnippetLanguage() -> String {
    if _translations.isEmpty {
      return ""
    }

    let translationSet: TranslationSet = _translations[0]
    if translationSet.getTranslationSnippet().isEmpty {
      return ""
    }

    let snippets: [Translation] = translationSet.getTranslationSnippet()

    if snippets.isEmpty {
      return ""
    }

    return snippets[0].language
  }

  var isWelcome: Bool {
    get {
      _isWelcome
    }
    set {
      _isWelcome = newValue
    }
  }

  var isEmpty: Bool {
    get {
      _isEmpty
    }
    set {
      _isEmpty = newValue
    }
  }
}
