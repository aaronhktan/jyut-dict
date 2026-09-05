//
//  SourceSentence.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

class SourceSentence {
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

    private var _sentences: [SentenceSet]
    
    private var _isWelcome: Bool = false
    private var _isEmpty: Bool = false

    init(
        sourceLanguage: String,
        simplified: String,
        traditional: String,
        jyutping: String,
        pinyin: String,
        sentences: [SentenceSet]
    ) {
        self._sourceLanguage = sourceLanguage
        self._simplified = simplified
        self._traditional = traditional
        self._jyutping = jyutping
        self._pinyin = pinyin
        self._sentences = sentences
    }

    var sourceLanguage: String {
        get {
            _sourceLanguage
        }
        set {
            _sourceLanguage = newValue
        }
    }

    func getCharacters() -> String {
        // TODO: Implement with options
        ""
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

    func generatePhonetic() -> Bool {
        // TODO: Implement
        return false
    }

    func getPhonetic() -> String {
        // TODO: Implement
        ""
    }

    func getCantonesePhonetic() -> String {
        // TODO: Implement
        ""
    }

    func getMandarinPhonetic() -> String {
        // TODO: Implement
        ""
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

    func getSentenceSets() -> [SentenceSet] {
        _sentences
    }
    
    func getSentenceSnippet() -> String {
        if (_sentences.isEmpty) {
            return ""
        }
        
        let sentenceSet: SentenceSet = _sentences[0]
        if (sentenceSet.getSentenceSnippet().isEmpty) {
            return ""
        }
        
        let snippets: [TargetSentence] = sentenceSet.getSentenceSnippet()
        
        if (snippets.isEmpty) {
            return ""
        }
        
        return snippets[0].sentence
    }
    
    func getSentenceSnippetLanguage() -> String {
        if (_sentences.isEmpty) {
            return ""
        }
        
        let sentenceSet: SentenceSet = _sentences[0]
        if (sentenceSet.getSentenceSnippet().isEmpty) {
            return ""
        }
        
        let snippets: [TargetSentence] = sentenceSet.getSentenceSnippet()
        
        if (snippets.isEmpty) {
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
