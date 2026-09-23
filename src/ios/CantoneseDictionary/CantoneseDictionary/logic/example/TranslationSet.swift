//
//  TranslationSet.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import Foundation

struct Translation {
    var content: String
    var language: String
    var directTarget: Bool
}

nonisolated class TranslationSet : Hashable, Identifiable, @unchecked Sendable {
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

    func getSource() -> String {
        _source
    }

    func getSourceShortString() -> String {
        _sourceShortString
    }

    func getTranslationSnippet() -> [Translation] {
        if let snippet = _snippet {
            return snippet
        }

        _snippet = []
        for i in 0...4 {
            _snippet?.append(_translations[i])
        }

        return _snippet ?? []
    }

    func getTranslations() -> [Translation] {
        _translations
    }
}
