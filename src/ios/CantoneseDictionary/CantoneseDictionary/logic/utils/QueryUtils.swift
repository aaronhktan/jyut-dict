//
//  QueryUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import Foundation
nonisolated func prepareJyutpingBindValues(jyutping: String, useFuzzyJyutping: Bool) -> String {
    let searchExactMatch = jyutping.count >= 3 && jyutping.hasPrefix("\"") && jyutping.hasSuffix("\"")
    let appendWildcard = !jyutping.hasSuffix("$")
    
    var correctedTerm = jyutping
    if !searchExactMatch && useFuzzyJyutping {
        if appendWildcard {
            correctedTerm = jyutpingAutocorrect(text: jyutping)
        } else {
            correctedTerm = jyutpingAutocorrect(text: String(jyutping.prefix(jyutping.count - 1)))
        }
    }
    
    var jyutpingSyllables: [String] = []
    if searchExactMatch {
        jyutpingSyllables = String(correctedTerm[correctedTerm.index(after: correctedTerm.startIndex)..<correctedTerm.index(before: correctedTerm.endIndex)]).components(separatedBy: " ")
    } else {
        (_, jyutpingSyllables) = segmentJyutping(text: correctedTerm, removeSpecialCharacters: true, removeGlobCharacters: false, removeRegexCharacters: !useFuzzyJyutping)
    }
    
    if !searchExactMatch && useFuzzyJyutping {
        jyutpingSyllables = jyutpingSoundChanges(text: jyutpingSyllables)
    }
    
    var result: String
    if searchExactMatch {
        result = jyutpingSyllables.joined(separator: " ")
    } else {
        result = constructRomanisationQuery(syllables: jyutpingSyllables, delimiter: "?")
    }
    
    if useFuzzyJyutping {
        result = result.replacingOccurrences(of: "*", with: ".*").replacingOccurrences(of: "?", with: ".").replacingOccurrences(of: "!", with: "?")
        if searchExactMatch || !appendWildcard {
            result += "$"
        } else {
            result += ".*"
        }
    } else {
        if appendWildcard && !searchExactMatch {
            result += "*"
        }
    }
    
    return result
}
