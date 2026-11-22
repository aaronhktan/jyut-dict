//
//  DefinitionsSet.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-03.
//

import Foundation

nonisolated struct Definition: Hashable {
  var definitionContent: String
  var label: String
  var examples: [Example]
}

nonisolated struct DefinitionsSet: Hashable, Identifiable {
  let id: UUID
  private var _source: String
  private var _sourceShortString: String
  private var _definitions: [Definition]

  init(source: String, sourceShortString: String, definitions: [Definition]) {
    self.id = UUID()
    self._source = source
    self._sourceShortString = sourceShortString
    self._definitions = definitions
  }

  public static func == (lhs: DefinitionsSet, rhs: DefinitionsSet) -> Bool {
    lhs.id == rhs.id
  }

  public func hash(into hasher: inout Hasher) {
    hasher.combine(id)
  }

  var isEmpty: Bool {
    _definitions.isEmpty
  }

  var source: String {
    _source
  }

  var sourceShortString: String {
    _sourceShortString
  }

  var definitionSnippet: String {
    if isEmpty {
      return ""
    }

    var snippet = ""
    for i in 0..<_definitions.count - 1 {
      let firstLineBreak =
        _definitions[i].definitionContent.firstIndex(where: {
          ["\r", "\n"].contains($0)
        }) ?? _definitions[i].definitionContent.endIndex
      snippet.append(
        contentsOf: _definitions[i].definitionContent[..<firstLineBreak]
          + "; "
      )
    }
    if let lastDefinition = _definitions.last {
      let firstLineBreak =
        lastDefinition.definitionContent.firstIndex(where: {
          ["\r", "\n"].contains($0)
        }) ?? lastDefinition.definitionContent.endIndex
      snippet.append(
        contentsOf: lastDefinition.definitionContent[..<firstLineBreak]
      )
    }
    return snippet
  }

  var definitions: [Definition] {
    _definitions
  }
}
