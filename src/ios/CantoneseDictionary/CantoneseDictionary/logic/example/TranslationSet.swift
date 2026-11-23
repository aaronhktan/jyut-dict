//
//  TranslationSet.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import Foundation

nonisolated struct Translation: Hashable, Identifiable {
  let id = UUID()
  var content: String
  var language: String
  var directTarget: Bool

  public func hash(into hasher: inout Hasher) {
    hasher.combine(id)
  }
}

nonisolated struct TranslationSet: Hashable, Identifiable {
  let id: UUID
  private var _source: String
  private var _snippet: [Translation]?
  private var _translations: [Translation]

  init(source: String, translations: [Translation]) {
    self.id = UUID()
    self._source = source
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

  var source: String {
    _source
  }

  func getTranslationSnippet() -> [Translation] {
    var snippet: [Translation] = []
    for i in 0...4 {
      if i < _translations.count {
        snippet.append(_translations[i])
      }
    }

    return snippet
  }

  var translations: [Translation] {
    _translations
  }
}
