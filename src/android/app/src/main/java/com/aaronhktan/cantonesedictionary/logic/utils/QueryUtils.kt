package com.aaronhktan.cantonesedictionary.logic.utils

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
        (processedPinyin.length >= 3 && processedPinyin.startsWith("\"") && processedPinyin.endsWith("\""))
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

    val ftsParam = "\"$english\""
    var likeParam = english
    likeParam = if (searchExactMatch) {
        likeParam.drop(1).dropLast(1)
    } else {
        "%$english%"
    }

    return Pair(ftsParam, likeParam)
}
