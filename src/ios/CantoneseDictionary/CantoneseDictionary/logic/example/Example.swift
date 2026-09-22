//
//  Example.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

nonisolated class Example : @unchecked Sendable {
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
        self._sourceLanguage = sourceLanguage
        self._simplified = simplified
        self._traditional = traditional
        self._jyutping = jyutping
        self._pinyin = pinyin
        self._translations = translations
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
    ) -> Bool {
        if (cantoneseOptions.rawValue & CantoneseOptions.prettyYale.rawValue
            == CantoneseOptions.prettyYale.rawValue) && _yale == nil
        {
            
        }
        if (cantoneseOptions.rawValue & CantoneseOptions.cantoneseIPA.rawValue
            == CantoneseOptions.cantoneseIPA.rawValue) && _cantoneseIPA == nil
        {
            // TODO: Implement
        }

        if (mandarinOptions.rawValue & MandarinOptions.prettyPinyin.rawValue
            == MandarinOptions.prettyPinyin.rawValue) && _prettyPinyin == nil
        {
            // TODO: Implement
        }
        if (mandarinOptions.rawValue & MandarinOptions.numberedPinyin.rawValue
            == MandarinOptions.numberedPinyin.rawValue)
            && _numberedPinyin == nil
        {
            // TODO: Implement
        }
        if (mandarinOptions.rawValue & MandarinOptions.zhuyin.rawValue
            == MandarinOptions.zhuyin.rawValue) && _zhuyin == nil
        {
            // TODO: Implement
        }
        if (mandarinOptions.rawValue & MandarinOptions.mandarinIPA.rawValue
            == MandarinOptions.mandarinIPA.rawValue) && _mandarinIPA == nil
        {
            // TODO: Implement
        }

        return true
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
            return _yale!
        case .cantoneseIPA:
            return _cantoneseIPA!
        case .rawJyutping:
            fallthrough
        default:
            return _jyutping
        }
    }

    func getMandarinPhonetic(mandarinOptions: MandarinOptions) -> String {
        switch mandarinOptions {
        case .prettyPinyin:
            return _prettyPinyin!
        case .numberedPinyin:
            return _numberedPinyin!
        case .zhuyin:
            return _zhuyin!
        case .mandarinIPA:
            return _mandarinIPA!
        case .rawPinyin:
            fallthrough
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
