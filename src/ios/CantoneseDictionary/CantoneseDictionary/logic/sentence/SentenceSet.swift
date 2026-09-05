//
//  SentenceSet.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

struct TargetSentence {
    var sentence: String
    var language: String
    var directTarget: Bool
}

class SentenceSet {
    private var _source: String
    private var _sourceShortString: String
    private var _snippet: [TargetSentence]?
    private var _sentences: [TargetSentence]

    init(source: String, sentences: [TargetSentence]) {
        self._source = source
        // TODO: Actually implement sourceShortString
        self._sourceShortString = String(
            source.split(separator: " ").first ?? ""
        )
        self._sentences = sentences
    }

    func isEmpty() -> Bool {
        _sentences.isEmpty
            || _sentences.allSatisfy({
                $0.sentence.isEmpty && $0.language.isEmpty
                    && $0.directTarget == false
            })
    }

    func appendSentence(_ sentence: TargetSentence) {
        _sentences.append(sentence)
    }

    func getSource() -> String {
        _source
    }

    func getSourceShortString() -> String {
        _sourceShortString
    }

    func getSentenceSnippet() -> [TargetSentence] {
        if let snippet = _snippet {
            return snippet
        }

        _snippet = []
        for i in 0...4 {
            _snippet?.append(_sentences[i])
        }

        return _snippet ?? []
    }

    func getSentences() -> [TargetSentence] {
        _sentences
    }
}
