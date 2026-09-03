//
//  Entry.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-03.
//

import Foundation

class Entry {
    private var simplified: String
    private var simplifiedDifference: String?
    private var preferSimplified: String?
    private var traditional: String
    private var traditionalDifference: String?
    private var preferTraditional: String?
    
    private var colouredSimplified: String?
    private var colouredSimplifiedDifference: String?
    private var colouredPreferSimplified: String?
    private var colouredTraditional: String?
    private var colouredTraditionalDifference: String?
    private var colouredPreferTraditional: String?
    
    private var jyutping: String
    private var yale: String?
    private var cantoneseIPA: String?
    private var jyutpingNumbers: [UInt8]?
    
    private var pinyin: String
    private var prettyPinyin: String?
    private var numberedPinyin: String?
    private var zhuyin: String?
    private var mandarinIPA: String?
    private var pinyinNumbers: [UInt8]?
    
    // To be changed later
    private var definitions: [DefinitionsSet]?
    private var definitionSnippet: String?
    
    private var isWelcome: Bool = false
    private var isEmpty: Bool = false
    
    public init(simplified: String, traditional: String, jyutping: String, pinyin: String, definitions: [DefinitionsSet]) {
        self.simplified = simplified
        self.traditional = traditional
        self.jyutping = jyutping
        self.pinyin = pinyin
        self.definitions = definitions
    }
    
}
