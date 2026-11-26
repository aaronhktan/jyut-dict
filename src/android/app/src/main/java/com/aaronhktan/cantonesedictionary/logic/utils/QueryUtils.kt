package com.aaronhktan.cantonesedictionary.logic.utils

import android.util.Log
import androidx.sqlite.SQLiteStatement
import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.example.Example
import com.aaronhktan.cantonesedictionary.logic.example.Translation
import com.aaronhktan.cantonesedictionary.logic.example.TranslationSet
import kotlinx.coroutines.currentCoroutineContext
import kotlinx.coroutines.ensureActive
import kotlinx.coroutines.yield
import kotlinx.serialization.SerializationException
import kotlinx.serialization.json.Json
import kotlinx.serialization.json.JsonArray
import kotlinx.serialization.json.JsonNull
import kotlinx.serialization.json.contentOrNull
import kotlinx.serialization.json.intOrNull
import kotlinx.serialization.json.jsonArray
import kotlinx.serialization.json.jsonObject
import kotlinx.serialization.json.jsonPrimitive

private const val LOG_TAG = "QueryUtils"

fun prepareCharacterBindValues(characters: String): String {
    val searchExactMatch =
        (characters.length >= 3 && characters.startsWith("\"") && characters.endsWith("\""))
    val appendWildcard = !characters.endsWith("$")

    val result = if (searchExactMatch) {
        characters.drop(1).dropLast(1)
    } else if (!appendWildcard) {
        characters.dropLast(1)
    } else {
        "$characters*"
    }

    return result
}

fun prepareJyutpingBindValues(
    jyutping: String,
    useFuzzyJyutping: Boolean,
    unsafeFuzzyJyutping: Boolean
): String {
    val searchExactMatch =
        (jyutping.length >= 3 && jyutping.startsWith("\"") && jyutping.endsWith("\""))
    val appendWildcard = !jyutping.endsWith("$")

    var correctedTerm = jyutping
    if (!searchExactMatch && useFuzzyJyutping) {
        correctedTerm = if (appendWildcard) {
            jyutpingAutocorrect(text = correctedTerm, unsafeSubstitutions = unsafeFuzzyJyutping)
        } else {
            jyutpingAutocorrect(
                text = correctedTerm.dropLast(1),
                unsafeSubstitutions = unsafeFuzzyJyutping
            )
        }
    }

    var jyutpingSyllables: List<String> = listOf()
    if (searchExactMatch) {
        jyutpingSyllables = correctedTerm.drop(1).dropLast(1).split(" ")
    } else {
        val (_, syllables) = segmentJyutping(
            text = correctedTerm,
            removeSpecialCharacters = true,
            removeGlobCharacters = false,
            removeRegexCharacters = !useFuzzyJyutping
        )
        jyutpingSyllables = syllables
    }

    if (!searchExactMatch && useFuzzyJyutping) {
        jyutpingSyllables = jyutpingSoundChanges(jyutpingSyllables)
    }

    var result = if (searchExactMatch) {
        jyutpingSyllables.joinToString(" ")
    } else {
        constructRomanisationQuery(syllables = jyutpingSyllables, delimiter = "?")
    }

    if (useFuzzyJyutping) {
        result = result
            .replace("*", ".*")
            .replace("?", ".")
            .replace("!", "?")
        result = if (searchExactMatch || !appendWildcard) {
            "$result$"
        } else {
            "$result.*"
        }
    } else {
        if (appendWildcard && !searchExactMatch) {
            result += "*"
        }
    }

    return result
}

fun preparePinyinBindValues(
    pinyin: String,
    useFuzzyPinyin: Boolean,
): String {
    var processedPinyin = pinyin
        .replace("v", "u:")
        .replace("ü", "u:")
    val searchExactMatch =
        (processedPinyin.length >= 3 && processedPinyin.startsWith("\"") && processedPinyin.endsWith(
            "\""
        ))
    val appendWildcard = !processedPinyin.endsWith("$")

    var pinyinSyllables: List<String> = listOf()
    if (searchExactMatch) {
        pinyinSyllables = processedPinyin.drop(1).dropLast(1).split(" ")
    } else {
        val (_, syllables) = segmentPinyin(
            text = processedPinyin,
            removeSpecialCharacters = true,
            removeGlobCharacters = false,
        )
        pinyinSyllables = syllables
    }

    if (!searchExactMatch && useFuzzyPinyin) {
        pinyinSyllables = pinyinSoundChanges(pinyinSyllables)
    }

    var result = if (searchExactMatch) {
        pinyinSyllables.joinToString(" ")
    } else {
        constructRomanisationQuery(syllables = pinyinSyllables, delimiter = "?")
    }

    if (useFuzzyPinyin) {
        result = result
            .replace("*", ".*")
            .replace("?", ".")
            .replace("!", "?")
        result = if (searchExactMatch || !appendWildcard) {
            "$result$"
        } else {
            "$result.*"
        }
    } else {
        if (appendWildcard && !searchExactMatch) {
            result += "*"
        }
    }

    return result
}

fun prepareEnglishBindValues(english: String): Pair<String, String> {
    val searchExactMatch =
        (english.length >= 3 && english.startsWith("\"") && english.endsWith("\""))

    val phrase = if (searchExactMatch) {
        english.drop(1).dropLast(1)
    } else {
        english
    }
    val ftsParam = "\"${phrase.replace("\"", "\"\"")}\""
    val likeParam = if (searchExactMatch) phrase else "%$english%"

    return Pair(ftsParam, likeParam)
}

suspend fun parseReturnedEntryRecords(statement: SQLiteStatement): List<Entry> {
    val result: MutableList<Entry> = mutableListOf()

    val columnIndices = statement.columnIndices()
    val rowIdIdx = columnIndices["entry_id"]!!
    val traditionalIdx = columnIndices["traditional"]!!
    val simplifiedIdx = columnIndices["simplified"]!!
    val jyutpingIdx = columnIndices["jyutping"]!!
    val pinyinIdx = columnIndices["pinyin"]!!
    val definitionsIdx = columnIndices["definitions"]!!

    var statementIdx = 0
    while (statement.step()) {
        if (++statementIdx % 500 == 0) {
            currentCoroutineContext().ensureActive()
        }
        val definitionsSets: MutableList<DefinitionsSet> = mutableListOf()
        var definitionsListArray: JsonArray
        try {
            definitionsListArray =
                Json.parseToJsonElement(statement.getText(definitionsIdx)).jsonArray
            for (definitionGroupsJSON in definitionsListArray) {
                val definitionGroupsElement = definitionGroupsJSON.jsonObject
                val source = definitionGroupsElement["source"]?.jsonPrimitive?.contentOrNull

                val definitionsArray: JsonArray?
                val definitions: MutableList<Definition> = mutableListOf()
                try {
                    definitionsArray = definitionGroupsElement["definitions"]?.jsonArray
                    if (definitionsArray == null) {
                        throw SerializationException("Couldn't parse definitions as array")
                    }

                    for (definitionJSON in definitionsArray) {
                        val definitionElement = definitionJSON.jsonObject
                        val definition =
                            definitionElement["definition"]?.jsonPrimitive?.contentOrNull
                        val label = definitionElement["label"]?.jsonPrimitive?.contentOrNull ?: ""

                        val examplesArray: JsonArray?
                        val examples: MutableList<Example> = mutableListOf()
                        try {
                            examplesArray = definitionElement["examples"]?.jsonArray
                            if (examplesArray == null) {
                                throw SerializationException("Couldn't parse examples as array")
                            }

                            for (exampleJSON in examplesArray) {
                                if (exampleJSON == JsonNull) {
                                    continue
                                }
                                val exampleElement = exampleJSON.jsonObject
                                val traditional =
                                    exampleElement["traditional"]?.jsonPrimitive?.contentOrNull
                                val simplified =
                                    exampleElement["simplified"]?.jsonPrimitive?.contentOrNull
                                val jyutping =
                                    exampleElement["jyutping"]?.jsonPrimitive?.contentOrNull
                                val pinyin = exampleElement["pinyin"]?.jsonPrimitive?.contentOrNull
                                val language =
                                    exampleElement["language"]?.jsonPrimitive?.contentOrNull

                                val translationsArray: JsonArray?
                                val translations: MutableList<Translation> = mutableListOf()
                                try {
                                    translationsArray = exampleElement["translations"] as? JsonArray
                                    if (translationsArray == null) {
                                        throw SerializationException("Couldn't parse translations as array")
                                    }

                                    for (translationJSON in translationsArray) {
                                        val translationElement = translationJSON.jsonObject
                                        val translation =
                                            translationElement["translation"]?.jsonPrimitive?.contentOrNull
                                        val language =
                                            translationElement["language"]?.jsonPrimitive?.contentOrNull
                                        val direct =
                                            (translationElement["direct"]?.jsonPrimitive?.intOrNull
                                                ?: 0) == 1

                                        translations.add(
                                            Translation(
                                                content = translation!!,
                                                language = language!!,
                                                directTarget = direct,
                                            )
                                        )
                                    }
                                } catch (s: SerializationException) {
                                    Log.e(
                                        LOG_TAG,
                                        "Translations array could not be parsed! ${s.message}"
                                    )
                                    continue
                                }

                                val translationSet = TranslationSet(
                                    source = source!!,
                                    translations = translations,
                                )

                                examples.add(
                                    Example(
                                        sourceLanguage = language!!,
                                        traditional = traditional!!,
                                        simplified = simplified!!,
                                        jyutping = jyutping!!,
                                        pinyin = pinyin!!,
                                        translations = listOf(translationSet)
                                    )
                                )
                            }
                        } catch (s: SerializationException) {
                            Log.e(
                                LOG_TAG,
                                "Examples array could not be parsed! ${s.message}"
                            )
                            continue
                        } finally {
                            definitions.add(
                                Definition(
                                    definitionContent = definition!!,
                                    label = label,
                                    examples = examples,
                                )
                            )
                        }

                    }
                } catch (s: SerializationException) {
                    Log.e(
                        LOG_TAG,
                        "Definitions set was not a dict!, ${s.message}"
                    )
                    continue
                } finally {
                    definitionsSets.add(
                        DefinitionsSet(
                            source = source!!,
                            sourceShortString = source,
                            definitions = definitions,
                        )
                    )
                }
            }
        } catch (s: SerializationException) {
            Log.e(
                LOG_TAG,
                "Could not parse definitions ${statement.getText(definitionsIdx)} as JSON"
            )
            continue
        } finally {
            result.add(
                Entry(
                    rowId = statement.getInt(rowIdIdx),
                    traditional = statement.getText(traditionalIdx),
                    simplified = statement.getText(simplifiedIdx),
                    jyutping = statement.getText(jyutpingIdx),
                    pinyin = statement.getText(pinyinIdx),
                    definitions = definitionsSets,
                )
            )
        }
    }

    return result.toList()
}

suspend fun parseReturnedExampleRecords(statement: SQLiteStatement): List<Example> {
    val result: MutableList<Example> = mutableListOf()

    val columnIndices = statement.columnIndices()
    val traditionalIdx = columnIndices["traditional"]!!
    val simplifiedIdx = columnIndices["simplified"]!!
    val jyutpingIdx = columnIndices["jyutping"]!!
    val pinyinIdx = columnIndices["pinyin"]!!
    val languageIdx = columnIndices["language"]!!
    val translationsIdx = columnIndices["translations"]!!

    var statementIdx = 0
    while (statement.step()) {
        if (++statementIdx % 500 == 0) {
            currentCoroutineContext().ensureActive()
        }
        val translationSets: MutableList<TranslationSet> = mutableListOf()
        try {
            val translationSetArray =
                Json.parseToJsonElement(statement.getText(translationsIdx)).jsonArray

            for (translationSetJSON in translationSetArray) {
                val translationSetElement = translationSetJSON.jsonObject
                val source = translationSetElement["source"]?.jsonPrimitive?.contentOrNull

                val translationsArray: JsonArray?
                val translations: MutableList<Translation> = mutableListOf()
                try {
                    translationsArray = translationSetElement["translations"]?.jsonArray
                    if (translationsArray == null) {
                        throw SerializationException("Could not parse translations as array")
                    }
                    for (translationJSON in translationsArray) {
                        val translationElement = translationJSON.jsonObject
                        val translation =
                            translationElement["translation"]?.jsonPrimitive?.contentOrNull
                        val language =
                            translationElement["language"]?.jsonPrimitive?.contentOrNull
                        val direct =
                            (translationElement["direct"]?.jsonPrimitive?.intOrNull
                                ?: 0) == 1

                        translations.add(
                            Translation(
                                content = translation ?: "x",
                                language = language ?: "",
                                directTarget = direct,
                            )
                        )
                    }
                } catch (s: SerializationException) {
                    Log.e(
                        LOG_TAG,
                        "Translations array could not be parsed! ${s.message}"
                    )
                    continue
                } finally {
                    val translationSet = TranslationSet(
                        source = source ?: "",
                        translations = translations,
                    )

                    translationSets.add(translationSet)
                }
            }
        } catch (s: SerializationException) {
            Log.e(
                LOG_TAG,
                "Could not parse translation sets array ${statement.getText(translationsIdx)}, error: ${s.message}"
            )
        } finally {
            result.add(
                Example(
                    sourceLanguage = statement.getText(languageIdx),
                    traditional = statement.getText(traditionalIdx).trim(),
                    simplified = statement.getText(simplifiedIdx).trim(),
                    jyutping = statement.getText(jyutpingIdx).trim(),
                    pinyin = statement.getText(pinyinIdx).trim(),
                    translations = translationSets,
                )
            )
        }
    }

    return result
}

fun parseExistenceRecords(statement: SQLiteStatement): Boolean {
    while (statement.step()) {
        return statement.getInt(0) == 1
    }
    return false
}
