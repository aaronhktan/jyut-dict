//
//  TranslationSet.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import Foundation

struct Translation: Hashable, Identifiable {
  let id = UUID()
  var content: String
  var language: String
  var directTarget: Bool

  public func hash(into hasher: inout Hasher) {
    hasher.combine(id)
  }
}

nonisolated class TranslationSet: Hashable, Identifiable, @unchecked Sendable {
  let id: UUID
  private var _source: String
  private var _sourceShortString: String
  private var _snippet: [Translation]?
  private var _translations: [Translation]

  init(source: String, translations: [Translation]) {
    self.id = UUID()
    self._source = source
    // TODO: Actually implement sourceShortString
    self._sourceShortString = String(
      source.split(separator: " ").first ?? ""
    )
    self._translations = translations
  }

  public static func == (lhs: TranslationSet, rhs: TranslationSet) -> Bool {
    lhs.id == rhs.id
  }

  public func hash(into hasher: inout Hasher) {
    hasher.combine(id)
  }

  func isEmpty() -> Bool {
    _translations.isEmpty
      || _translations.allSatisfy({
        $0.content.isEmpty && $0.language.isEmpty
          && $0.directTarget == false
      })
  }

  func appendTranslation(_ translation: Translation) {
    _translations.append(translation)
  }

  var source: String {
    _source
  }

  var sourceShortString: String {
    _sourceShortString
  }

  func getTranslationSnippet() -> [Translation] {
    if let snippet = _snippet {
      return snippet
    }

    _snippet = []
    for i in 0...4 {
      if i < _translations.count {
        _snippet?.append(_translations[i])
      }
    }

    return _snippet ?? []
  }

  var translations: [Translation] {
    _translations
  }
}
