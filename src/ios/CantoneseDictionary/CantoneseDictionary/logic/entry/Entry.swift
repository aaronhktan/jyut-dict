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

nonisolated class Entry : @unchecked Sendable, Identifiable {
    private var _simplified: String
    private var _simplifiedDifference: String?
    private var _preferSimplified: String?
    private var _traditional: String
    private var _traditionalDifference: String?
    private var _preferTraditional: String?

    private var _colouredSimplified: String?
    private var _colouredSimplifiedDifference: String?
    private var _colouredPreferSimplified: String?
    private var _colouredTraditional: String?
    private var _colouredTraditionalDifference: String?
    private var _colouredPreferTraditional: String?

    private var _jyutping: String
    private var _yale: String?
    private var _cantoneseIPA: String?
    private var _jyutpingNumbers: [UInt8]?

    private var _pinyin: String
    private var _prettyPinyin: String?
    private var _numberedPinyin: String?
    private var _zhuyin: String?
    private var _mandarinIPA: String?
    private var _pinyinNumbers: [UInt8]?

    private var _definitions: [DefinitionsSet]?
    private var _definitionSnippet: String?

    private var _isWelcome: Bool = false
    private var _isEmpty: Bool = false
    
    private var _rowid: Int = -1

    init(
        rowid: Int,
        simplified: String,
        traditional: String,
        jyutping: String,
        pinyin: String,
        definitions: [DefinitionsSet]
    ) {
        self._rowid = rowid
        self._simplified = simplified
        self._traditional = traditional
        self._jyutping = jyutping
        self._pinyin = pinyin
        self._definitions = definitions
    }
    
    var id: Int {
        _rowid
    }

    var simplified: String {
        _simplified
    }
}
