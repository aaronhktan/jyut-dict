//
//  Entry.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-03.
//

import Foundation

enum EntryCharactersOptions: CaseIterable {
    case onlySimplified
    case onlyTraditional
    case preferSimplified
    case preferTraditional
}

enum EntryPhoneticOptions: CaseIterable {
    case onlyCantonese
    case onlyMandarin
    case preferCantonese
    case preferMandarin
}

enum EntryColourPhoneticType: CaseIterable {
    case none
    case cantonese
    case mandarin
}

enum CantoneseOptions: UInt8, CaseIterable {
    case none = 0x0
    case rawJyutping = 0x1
    case prettyYale = 0x2
    case cantoneseIPA = 0x4
}

enum MandarinOptions: UInt8, CaseIterable {
    case none = 0x0
    case rawPinyin = 0x1
    case prettyPinyin = 0x2
    case numberedPinyin = 0x4
    case zhuyin = 0x8
    case mandarinIPA = 0x10
}

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

    private var definitions: [DefinitionsSet]?
    private var definitionSnippet: String?

    private var isWelcome: Bool = false
    private var isEmpty: Bool = false

    public init(
        simplified: String,
        traditional: String,
        jyutping: String,
        pinyin: String,
        definitions: [DefinitionsSet]
    ) {
        self.simplified = simplified
        self.traditional = traditional
        self.jyutping = jyutping
        self.pinyin = pinyin
        self.definitions = definitions
    }

}
