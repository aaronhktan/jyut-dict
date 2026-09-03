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
    // var sentences: [SourceSentence]
}

class DefinitionsSet {
    private var source: String
    private var sourceShortString: String
    private var snippet: String?
    private var definitions: [Definition]
    
    init(source: String, sourceShortString: String, definitions: [Definition]) {
        self.source = source
        self.sourceShortString = sourceShortString
        self.definitions = definitions
    }
}
