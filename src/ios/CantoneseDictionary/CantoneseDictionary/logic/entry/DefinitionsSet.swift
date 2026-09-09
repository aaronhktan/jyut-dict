//
//  DefinitionsSet.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-03.
//

import Foundation

struct Definition {
    var definitionContent: String
    var label: String
    var sentences: [SourceSentence]
}

class DefinitionsSet {
    private var _source: String
    private var _sourceShortString: String
    private var _snippet: String?
    private var _definitions: [Definition]

    init(source: String, sourceShortString: String, definitions: [Definition]) {
        self._source = source
        self._sourceShortString = sourceShortString
        self._definitions = definitions
    }

    var isEmpty: Bool {
        _definitions.isEmpty
    }

    func appendDefinition(_ definition: Definition) {
        _definitions.append(definition)
    }

    func generatePhonetic(
        _ cantoneseOptions: CantoneseOptions,
        _ mandarinOptions: MandarinOptions
    ) {
        _definitions.forEach { definition in
            definition.sentences.forEach { sentence in
                _ = sentence.generatePhonetic(
                    cantoneseOptions: cantoneseOptions,
                    mandarinOptions: mandarinOptions
                )
            }
        }
    }

    var source: String {
        _source
    }

    var sourceShortString: String {
        _sourceShortString
    }

    func getDefinitionSnippet() -> String {
        if isEmpty {
            return _snippet ?? ""
        }

        if let snippet = _snippet {
            return snippet
        }

        _snippet = ""
        for i in 0..<_definitions.count - 1 {
            let firstLineBreak =
                _definitions[i].definitionContent.firstIndex(where: {
                    ["\r", "\n"].contains($0)
                }) ?? _definitions[i].definitionContent.endIndex
            _snippet?.append(
                contentsOf: _definitions[i].definitionContent[..<firstLineBreak]
                    + "; "
            )
        }
        if let lastDefinition = _definitions.last {
            let firstLineBreak =
                lastDefinition.definitionContent.firstIndex(where: {
                    ["\r", "\n"].contains($0)
                }) ?? lastDefinition.definitionContent.endIndex
            _snippet?.append(
                contentsOf: lastDefinition.definitionContent[..<firstLineBreak]
            )
        }
        return _snippet ?? ""
    }
}
