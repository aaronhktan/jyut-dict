//
//  QueryUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import Foundation
import GRDB
import os

nonisolated func prepareCharacterBindValues(
    characters: String,
) -> String {
    let searchExactMatch =
        characters.count >= 3 && characters.hasPrefix("\"")
        && characters.hasSuffix("\"")
    let appendWildcard = !characters.hasSuffix("$")

    var result: String
    if searchExactMatch {
        result = String(characters.dropFirst().dropLast())
    } else if !appendWildcard {
        result = String(characters.prefix(characters.count - 1))
    } else {
        result = characters + "*"
    }

    return result
}

@concurrent func prepareJyutpingBindValues(
    jyutping: String,
    useFuzzyJyutping: Bool
) async -> String {
    let searchExactMatch =
        jyutping.count >= 3
        && ((jyutping.hasPrefix("\"")
            && jyutping.hasSuffix("\""))
            || (jyutping.hasPrefix("“") && jyutping.hasSuffix("”")))
    let appendWildcard = !jyutping.hasSuffix("$")

    var correctedTerm = jyutping
    if !searchExactMatch && useFuzzyJyutping {
        if appendWildcard {
            correctedTerm = await jyutpingAutocorrect(text: jyutping)
        } else {
            correctedTerm = await jyutpingAutocorrect(
                text: String(jyutping.prefix(jyutping.count - 1))
            )
        }
    }

    var jyutpingSyllables: [String] = []
    if searchExactMatch {
        jyutpingSyllables = String(
            correctedTerm.dropFirst().dropLast()
        ).components(separatedBy: " ")
    } else {
        (_, jyutpingSyllables) = await segmentJyutping(
            text: correctedTerm,
            removeSpecialCharacters: true,
            removeGlobCharacters: false,
            removeRegexCharacters: !useFuzzyJyutping
        )
    }

    if !searchExactMatch && useFuzzyJyutping {
        jyutpingSyllables = await jyutpingSoundChanges(text: jyutpingSyllables)
    }

    var result: String
    if searchExactMatch {
        result = jyutpingSyllables.joined(separator: " ")
    } else {
        result = constructRomanisationQuery(
            syllables: jyutpingSyllables,
            delimiter: "?"
        )
    }

    if useFuzzyJyutping {
        result = result.replacingOccurrences(of: "*", with: ".*")
            .replacingOccurrences(of: "?", with: ".").replacingOccurrences(
                of: "!",
                with: "?"
            )
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

@concurrent func preparePinyinBindValues(
    pinyin: String,
    useFuzzyPinyin: Bool
) async -> String {
    let searchExactMatch =
        pinyin.count >= 3
        && ((pinyin.hasPrefix("\"")
            && pinyin.hasSuffix("\""))
            || (pinyin.hasPrefix("“") && pinyin.hasSuffix("”")))
    let appendWildcard = !pinyin.hasSuffix("$")

    var pinyinSyllables: [String] = []
    if searchExactMatch {
        pinyinSyllables = String(
            pinyin.dropFirst().dropLast()
        ).components(separatedBy: " ")
    } else {
        (_, pinyinSyllables) = segmentPinyin(
            text: pinyin,
            removeSpecialCharacters: true,
            removeGlobCharacters: false
        )
    }

    if !searchExactMatch && useFuzzyPinyin {
        pinyinSyllables = pinyinSoundChanges(text: pinyinSyllables)
    }

    var result: String
    if searchExactMatch {
        result = pinyinSyllables.joined(separator: " ")
    } else {
        result = constructRomanisationQuery(
            syllables: pinyinSyllables,
            delimiter: "?"
        )
    }

    if useFuzzyPinyin {
        result = result.replacingOccurrences(of: "*", with: ".*")
            .replacingOccurrences(of: "?", with: ".").replacingOccurrences(
                of: "!",
                with: "?"
            )
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

@concurrent func prepareEnglishBindValues(
    english: String,
) async -> (String, String) {
    let searchExactMatch =
        english.count >= 3
        && ((english.hasPrefix("\"")
            && english.hasSuffix("\""))
            || (english.hasPrefix("“") && english.hasSuffix("”")))

    let ftsParam = "\"\(english)\""
    var likeParam = english
    if searchExactMatch {
        likeParam = "\(String(likeParam.dropFirst().dropLast()))"
    } else {
        likeParam = "%\(english)%"
    }

    return (ftsParam, likeParam)
}

nonisolated func parseReturnedRecords(rows: [Row]) -> [Entry] {
    var result: [Entry] = []

    for row in rows {
        var definitionsSets: [DefinitionsSet] = []
        do {
            guard
                let definitionsListJSON = try JSONSerialization.jsonObject(
                    with: Data(String(row["definitions"]).utf8)
                ) as? [[String: Any]]
            else {
                logger.error(
                    "Could not parse definitions \(String(row["definitions"])) as JSON"
                )
                continue
            }

            for definitionGroupsJSON in definitionsListJSON {
                let source = definitionGroupsJSON["source"] as! String
                guard
                    let definitionsJSON = definitionGroupsJSON["definitions"]
                        as? [[String: Any]]
                else {
                    logger.error("Definition set was not a dict")
                    continue
                }

                var definitions: [Definition] = []
                for definitionJSON in definitionsJSON {
                    let content = definitionJSON["definition"] as! String
                    let label =
                        definitionJSON.keys.contains("label")
                        ? (definitionJSON["label"] as! String) : ""

                    var examples: [Example] = []
                    if let examplesJSON = definitionJSON["sentences"]
                        as? [[String: Any]]
                    {
                        for exampleJSON in examplesJSON {
                            let traditional =
                                exampleJSON["traditional"] as! String
                            let simplified =
                                exampleJSON["simplified"] as! String
                            let pinyin = exampleJSON["pinyin"] as! String
                            let jyutping = exampleJSON["jyutping"] as! String
                            let language = exampleJSON["language"] as! String

                            guard
                                let translationsJSON = exampleJSON[
                                    "translations"
                                ]
                                    as? [[String: Any]]
                            else {
                                logger.error(
                                    "Translations was not an array of dicts"
                                )
                                continue
                            }

                            var translations: [Translation] = []
                            for translationJSON in translationsJSON {
                                let sentence =
                                    translationJSON["sentence"] as! String
                                let language =
                                    translationJSON["language"] as! String
                                let direct =
                                    translationJSON["direct"] as! String == "1"
                                translations.append(
                                    Translation(
                                        content: sentence,
                                        language: language,
                                        directTarget: direct
                                    )
                                )
                            }

                            let translationSet = TranslationSet(
                                source: source,
                                translations: translations
                            )

                            examples.append(
                                Example(
                                    sourceLanguage: language,
                                    simplified: simplified,
                                    traditional: traditional,
                                    jyutping: jyutping,
                                    pinyin: pinyin,
                                    translations: [translationSet]
                                )
                            )
                        }
                    } else {
                        logger.warning("Sentences was not an array of dicts")
                    }

                    definitions.append(
                        Definition(
                            definitionContent: content,
                            label: label,
                            examples: examples
                        )
                    )
                }
                definitionsSets.append(
                    DefinitionsSet(
                        source: source,
                        sourceShortString: source,
                        definitions: definitions
                    )
                )
            }
        } catch {
            logger.error(
                "Could not parse definitions \(String(row["definitions"])) as JSON"
            )
        }

        result.append(
            Entry(
                rowid: row["entry_id"],
                traditional: row["traditional"],
                simplified: row["simplified"],
                jyutping: row["jyutping"],
                pinyin: row["pinyin"],
                definitions: definitionsSets
            )
        )
    }

    return result
}
