package com.aaronhktan.cantonesedictionary.logic.utils

import android.util.Log
import androidx.core.text.isDigitsOnly
import kotlin.math.min

private const val LOG_TAG = "CantoneseUtils"

private val jyutpingInitials: Set<String> = setOf(
    "b", "p", "m", "f", "d",
    "t", "n", "l", "g", "k",
    "ng", "h", "gw", "kw", "w",
    "z", "c", "s", "j",
)

private val jyutpingFinals: Set<String> = setOf(
    "a", "aa", "aai", "aau", "aam", "aan", "aang", "aap", "aat", "aak",
    "ai", "au", "am", "an", "ang", "ap", "at", "ak", "e", "ei",
    "eu", "em", "en", "eng", "ep", "ek", "i", "iu", "im", "in",
    "ing", "ip", "it", "ik", "o", "oi", "ou", "on", "ong", "ot",
    "ok", "u", "ui", "un", "ung", "ut", "uk", "oe", "oet", "eoi",
    "eon", "oeng", "eot", "oek", "yu", "yun", "yut", "m", "ng",
)

private val jyutpingFinalRegex = Regex(pattern = "([aeiou][aeiou]?[iumngptk]?[g]?)([1-6])")

private val jyutpingToYaleSpecialSyllables: Map<String, List<String>> = mapOf(
    "m" to listOf("m̄", "ḿ", "m", "m̀h", "ḿh", "mh"),
    "ng" to listOf("n̄g", "ńg", "ng", "ǹgh", "ńgh", "ngh"),
)
private val jyutpingToYaleSpecialFinals: Map<String, String> = mapOf(
    "aa" to "a",
    "oe" to "eu",
    "oeng" to "eung",
    "oek" to "euk",
    "eoi" to "eui",
    "eon" to "eun",
    "eot" to "eut",
)
private val yaleToneReplacements: Map<Char, List<String>> = mapOf(
    'a' to listOf("ā", "á", "a", "à", "á", "a"),
    'e' to listOf("ē", "é", "e", "è", "é", "e"),
    'i' to listOf("ī", "í", "i", "ì", "í", "i"),
    'o' to listOf("ō", "ó", "o", "ò", "ó", "o"),
    'u' to listOf("ū", "ú", "u", "ù", "ú", "u"),
)
private val yaleYInitialRegex = Regex(pattern = "jy?")
private val yaleJInitialRegex = Regex(pattern = "z")
private val yaleChInitialRegex = Regex(pattern = "c")
private val yaleLightToneClusterRegex = Regex(pattern = "([ptkmn]?g?)[123456]$")

private val cantoneseIPASpecialSyllables: List<Pair<String, String>> = listOf(
    Pair("a", "@"),
    Pair("yu", "y"),
    Pair("@@", "a"),
    Pair("uk", "^k"),
    Pair("ik", "|k"),
    Pair("ou", "~u"),
    Pair("eoi", "eoy"),
    Pair("ung", "^ng"),
    Pair("ing", "|ng"),
    Pair("ei", ">i"),
)
private val cantoneseIPAInitials: Map<String, String> = mapOf(
    "b" to "p",
    "p" to "pʰ",
    "d" to "t",
    "t" to "tʰ",
    "g" to "k",
    "k" to "kʰ",
    "ng" to "ŋ",
    "gw" to "kʷ",
    "kw" to "kʷʰ",
    "zh" to "t͡ʃ",
    "ch" to "t͡ʃʰ",
    "sh" to "ʃ",
    "z" to "t͡s",
    "c" to "t͡sʰ",
)
private val cantoneseIPANuclei: Map<String, String> = mapOf(
    "a" to "äː",
    "@" to "ɐ",
    "e" to "ɛː",
    ">" to "e",
    "i" to "iː",
    "|" to "ɪ",
    "o" to "ɔː",
    "~" to "o",
    "oe" to "œ̽ː",
    "eo" to "ɵ",
    "u" to "uː",
    "^" to "ʊ",
    "y" to "yː",
)
private val cantoneseIPACodas: Map<String, String> = mapOf(
    "i" to "i̯",
    "u" to "u̯",
    "y" to "y̯",
    "ng" to "ŋ",
    "p" to "p̚",
    "t" to "t̚",
    "k" to "k̚",
)
private val cantoneseIPASyllableRegex =
    Regex("([bcdfghjklmnpqrstvwxyz]?[bcdfghjklmnpqrstvwxyz]?)([a@e>i|o~u^y][eo]?)([iuymngptk]?g?)([1-9])")
private val cantoneseIPAHyuRegex = Regex("([zcs])yu")
private val cantoneseIPAHoeRegex = Regex("([zc])oe")
private val cantoneseIPAHeoRegex = Regex("([zc])eo")
private val cantoneseIPASPecialSyllableRegex = Regex("^(h?)([mn]g?)([1-6])$")
private val cantoneseIPAToneRegex = Regex("[1-6]")
private val cantoneseIPACheckedToneRegex = Regex("([ptk])([136])")
private val jyutpingToIPATones: List<String> = listOf(
    "˥", "˧˥", "˧", "˨˩", "˩˧", "˨", "˥", "˧", "˨",
)

private fun unfoldJyutpingRegex(jyutping: String): List<String> {
    var out: MutableList<String> = mutableListOf<String>()
    var stringPossibilities: MutableList<String> = mutableListOf<String>()

    // Invariant: there must be only one set of parentheses and one exclamation
    if (jyutping.count { it == '(' } > 1
        || jyutping.count { it == ')' } > 1
        || jyutping.count { it == '!' } > 1
    ) {
        return out
    }

    // If there is a parenthesis, then all of the options must be checked
    if (jyutping.indexOf('(') != -1 && jyutping.indexOf(')') != -1) {
        val openParenIdx = jyutping.indexOf('(')
        var startIdx = openParenIdx + 1
        val endIdx = jyutping.indexOf(')')
        var orIdx = jyutping.indexOf('|')

        while (orIdx != -1 && orIdx > startIdx && orIdx < endIdx) {
            val preParen = jyutping.substring(0, openParenIdx)
            val possibility = jyutping.substring(startIdx, orIdx)
            val postParen = jyutping.substring(endIdx + 1)
            stringPossibilities.add(preParen + possibility + postParen)
            startIdx = orIdx + 1
            orIdx = jyutping.indexOf('|', startIndex = startIdx)
        }
        val preParen = jyutping.substring(0, openParenIdx)
        val possibility = jyutping.substring(startIdx, endIdx)
        val postParen = jyutping.substring(endIdx + 1)
        stringPossibilities.add(preParen + possibility + postParen)
    } else {
        stringPossibilities.add(jyutping)
    }

    // If there is a "!", then the initial with and the initial without
    // that optional character should be considered
    for (s in stringPossibilities) {
        val regexIdx = s.indexOf('!')
        if (regexIdx == -1) {
            out.add(s)
        } else if (regexIdx == 0) {
            out.add(s.substring(1))
        } else {
            var tmp = s
            tmp = tmp.removeRange(regexIdx - 1..<regexIdx + 1)
            out.add(tmp)
            tmp = s
            tmp = tmp.removeRange(regexIdx..<regexIdx + 1)
            out.add(tmp)
        }
    }

    return out
}

private fun convertYaleInitial(syllable: String): String {
    return syllable.replace(yaleYInitialRegex, "y")
        .replace(yaleJInitialRegex, "j")
        .replace(yaleChInitialRegex, "ch")
}

private fun convertYaleFinal(syllable: String): String {
    var yaleSyllable = syllable
    val match = jyutpingFinalRegex.find(yaleSyllable)
    if (match == null) {
        Log.e(LOG_TAG, "Could not find final in syllable $yaleSyllable")
        return yaleSyllable
    }

    val (final, tone) = match.destructured

    // Some Jyutping finals have significant differences when mapped to Yale.
    // Switch it out here.
    jyutpingToYaleSpecialFinals[final]?.let {
        yaleSyllable = yaleSyllable.replace(final, it)
    }

    // Insert an "h" before the last consonant cluster for the light tones,
    // as they are indicated in Yale
    if (("4".."6").contains(tone)) {
        yaleSyllable = yaleSyllable.replace(yaleLightToneClusterRegex, "h\$0")
    }

    // Replace the first vowel in the final with its accented version
    val vowels = listOf('a', 'e', 'i', 'o', 'u')
    val replacementIdx = yaleSyllable.indexOfFirst { vowels.contains(it) }
    val firstVowel = yaleSyllable[replacementIdx]
    yaleToneReplacements[firstVowel]?.get(tone.toInt() - 1)?.let {
        yaleSyllable = yaleSyllable.replaceRange(replacementIdx..<replacementIdx + 1, it)
    }

    // Remove the tone number, as Yale doesn't use those
    yaleSyllable = yaleSyllable.dropLast(1)

    return yaleSyllable
}

fun convertJyutpingToYale(jyutping: String, useSpacesToSegment: Boolean = false): String {
    if (jyutping.isEmpty()) {
        return jyutping
    }

    var syllables = mutableListOf<String>()
    var jyutpingCopy = ""
    if (useSpacesToSegment) {
        // Insert a space before and after every special character, so that the
        // IPA conversion doesn't attempt to convert special characters.
        for (c in specialCharacters) {
            jyutpingCopy = jyutping.split(c).joinToString(" $c ")
        }
        syllables = jyutpingCopy.split(" ").toMutableList()
    } else {
        val (validJyutping, result) = segmentJyutping(
            text = jyutping,
            removeSpecialCharacters = false,
            removeGlobCharacters = false,
        )
        if (!validJyutping) {
            return "x"
        }
        syllables = result.toMutableList()
    }

    var yaleSyllables = mutableListOf<String>()
    for (syllable in syllables) {
        // Most numbers, single characters, etc. are not Jyutping.
        // Also skip syllables that are just punctuation.
        if (syllable.length == 1 || specialCharacters.contains(syllable)) {
            yaleSyllables.add(syllable)
            continue
        }

        // Skip syllables that don't have tone
        val toneIdx = syllable.indexOfFirst { it in '1'..'6' }
        if (toneIdx == -1) {
            yaleSyllables.add(syllable)
            continue
        }

        // Handle special-case syllables
        val syllableWithoutTone = syllable.substring(0, toneIdx)
        val tone = syllable[toneIdx].toString().toInt()
        jyutpingToYaleSpecialSyllables[syllableWithoutTone].let { syllable ->
            syllable?.get(tone - 1)?.let {
                yaleSyllables.add(it)
                continue
            }
        }

        var yaleSyllable = syllable
        yaleSyllable = convertYaleFinal(yaleSyllable)
        yaleSyllable = convertYaleInitial(yaleSyllable)
        yaleSyllables.add(yaleSyllable)
    }

    return yaleSyllables.joinToString(" ")
}

private fun convertIPACantoneseSyllable(jyutping: String): String {
    val match = cantoneseIPASyllableRegex.find(jyutping)
    if (match == null) {
        Log.e(LOG_TAG, "Invalid jyutping found in IPA conversion: $jyutping")
        return jyutping
    }

    val (matchedInitial, matchedNucleus, matchedCoda, matchedTone) = match.destructured

    var initial = ""
    if (!matchedInitial.isEmpty()) {
        cantoneseIPAInitials[matchedInitial]?.let {
            initial = it
        } ?: run {
            initial = matchedInitial
        }
    }

    var nucleus = ""
    if (!matchedNucleus.isEmpty()) {
        cantoneseIPANuclei[matchedNucleus]?.let {
            nucleus = it
        } ?: run {
            nucleus = matchedNucleus
        }
    }

    var coda = ""
    if (!matchedCoda.isEmpty()) {
        cantoneseIPACodas[matchedCoda]?.let {
            coda = it
        } ?: run {
            coda = matchedCoda
        }
    }

    var tone = ""
    if (!matchedTone.isEmpty()) {
        tone = jyutpingToIPATones[matchedTone.toInt() - 1]
    }

    return initial + nucleus + coda + tone
}

fun convertJyutpingToIPA(jyutping: String, useSpacesToSegment: Boolean = false): String {
    if (jyutping.isEmpty()) {
        return jyutping
    }

    var syllables = mutableListOf<String>()
    var jyutpingCopy = ""
    if (useSpacesToSegment) {
        // Insert a space before and after every special character, so that the
        // IPA conversion doesn't attempt to convert special characters.
        for (c in specialCharacters) {
            jyutpingCopy = jyutping.split(c).joinToString(" $c ")
        }
        syllables = jyutpingCopy.split(" ").toMutableList()
    } else {
        val (validJyutping, result) = segmentJyutping(
            text = jyutping,
            removeSpecialCharacters = false,
            removeGlobCharacters = false,
        )
        if (!validJyutping) {
            return "x"
        }
        syllables = result.toMutableList()
    }

    var ipaSyllables = mutableListOf<String>()
    for (syllable in syllables) {
        // Most numbers, single characters, etc. are not Jyutping.
        // Also skip syllables that are just punctuation.
        if (syllable.length == 1 || specialCharacters.contains(syllable)) {
            ipaSyllables.add(syllable)
            continue
        }

        // Skip syllables that don't have tone
        val toneIdx = syllable.indexOfFirst { it in '1'..'6' }
        if (toneIdx == -1) {
            ipaSyllables.add(syllable)
            continue
        }

        // Do some pre-processing
        var ipaSyllable = syllable
        ipaSyllable = ipaSyllable
            .replace(cantoneseIPAHyuRegex, "$1hyu")
            .replace(cantoneseIPAHoeRegex, "$1hoe")
            .replace(cantoneseIPAHeoRegex, "$1heo")

        // Convert special syllables
        cantoneseIPASPecialSyllableRegex.find(ipaSyllable)?.let { specialMatch ->
            val tone = specialMatch.destructured.component3()
            ipaSyllable = ipaSyllable
                .replace("m", "m̩")
                .replace("ng", "ŋ̍")
                .replace(cantoneseIPAToneRegex, jyutpingToIPATones[tone.toInt() - 1])
        }

        // Replace checked tones
        cantoneseIPACheckedToneRegex.find(ipaSyllable)?.let {
            ipaSyllable = ipaSyllable
                .replace("1", "7")
                .replace("3", "8")
                .replace("6", "9")
        }

        // Do some more preprocessing
        for ((from, to) in cantoneseIPASpecialSyllables) {
            ipaSyllable = ipaSyllable.replace(from, to)
        }

        ipaSyllables.add(convertIPACantoneseSyllable((ipaSyllable)))
    }

    return ipaSyllables.joinToString(" ")
}

fun segmentJyutping(
    text: String,
    removeSpecialCharacters: Boolean = true,
    removeGlobCharacters: Boolean = true,
    removeRegexCharacters: Boolean = true
): Pair<Boolean, List<String>> {
    var processedText = text
    var syllables: MutableList<String> = mutableListOf<String>()
    var validJyutping = true

    if (removeSpecialCharacters) {
        for (c in specialCharacters) {
            processedText = processedText.replace(c, " ")
        }
    }
    if (removeGlobCharacters) {
        processedText = processedText.replace("*", " ")
        processedText = processedText.replace("?", " ")
    }
    if (removeRegexCharacters) {
        for (c in regexCharacters) {
            processedText = processedText.replace(c, " ")
        }
    }

    var startIdx = 0
    var endIdx = 0
    var initialFound = false

    while (endIdx < processedText.length) {
        var componentFound = false

        var currentString = processedText[endIdx].lowercase()
        val isSpecialCharacter = specialCharacters.contains(currentString)
        val isGlobCharacter = currentString.trim() == "*" || currentString.trim() == "?"
        if (currentString == " " || currentString == "'" || isSpecialCharacter || isGlobCharacter) {
            // The presence of a space, apostrophe, special character, or glob character indicates
            // that a syllable is completed.
            if (initialFound) {
                // If a valid initial was previously found, then the Jyutping
                // sequence [initial] + [separator] is only valid Jyutping if the
                // initial is also a valid final (i.e. [final] + [separator] is
                // OK, but [initial] + [separator] is not).
                val previousInitial = processedText.substring(startIdx, endIdx).lowercase()
                syllables.add(previousInitial)
                if (!jyutpingFinals.contains(previousInitial)) {
                    validJyutping = false
                }
                startIdx = endIdx
                initialFound = false
            }
            if (isGlobCharacter) {
                if (endIdx > 0 && endIdx > startIdx) {
                    val previousInitial = processedText.substring(startIdx, endIdx).lowercase()
                    syllables.add(previousInitial)
                    if (!jyutpingFinals.contains(previousInitial)) {
                        validJyutping = false
                    }
                    initialFound = false
                }

                // Since whitespace matters for glob and regex, consume the
                // next or previous whitespace if it exists (and was not
                // already consumed by another glob character).
                var globStartIdx = endIdx
                if (endIdx > 0
                    && processedText[endIdx - 1] == ' '
                    && !syllables.isEmpty()
                    && syllables.last().last() != ' '
                ) {
                    // Add preceding whitespace to this word
                    globStartIdx = endIdx - 1
                }
                if (endIdx + 1 < processedText.length
                    && processedText[endIdx + 1] == ' '
                ) {
                    // Add succeeding whitespace to this word
                    ++endIdx
                }
                val glob = processedText.substring(globStartIdx..endIdx)
                syllables.add(glob)
                startIdx = endIdx
            } else if (isSpecialCharacter) {
                syllables.add(currentString)
            }
            ++startIdx
            ++endIdx
            continue
        }

        // Digits are only valid after a final (which should be handled in the
        // final-checking code)
        // OR after an initial (that is also a final), like m or ng.
        // This block checks for the latter case.
        if (currentString[0].isDigit()) {
            if (initialFound) {
                var previousInitial = processedText.substring(startIdx, endIdx).lowercase()

                var isValidFinal = false
                if (removeRegexCharacters) {
                    isValidFinal = jyutpingFinals.contains(previousInitial)
                } else {
                    // Regex characters need to be handled in a special way;
                    // essentially, we need to check every possibility. If at
                    // least one possibility is a valid final, then the Jyutping
                    // can be considered valid.
                    val stringsToSearch = unfoldJyutpingRegex(jyutping = previousInitial)
                    for (s in stringsToSearch) {
                        isValidFinal = isValidFinal || jyutpingFinals.contains(s)
                    }
                }

                if (isValidFinal) {
                    ++endIdx
                    previousInitial = processedText.substring(startIdx, endIdx).lowercase()
                    syllables.add(previousInitial)
                    startIdx = endIdx
                    initialFound = false

                    if (!('1'..'6').contains(currentString.first())) {
                        validJyutping = false
                    }

                    continue
                }
            } else {
                // If there was no initial found, then the Jyutping isn't valid
                validJyutping = false
                syllables.add(currentString)
                ++startIdx
                ++endIdx
                continue
            }
        }

        // If initial is valid, then extend the end_index for length of initial
        // cluster of consonants.
        // The longest length of an initial with unfolded regex is 16 UTF-16 bytes.
        var remainingLength = processedText.length - endIdx
        val maxInitialLength =
            if (removeRegexCharacters) min(2, remainingLength) else min(16, remainingLength)
        for (initialLen in (1..maxInitialLength).reversed()) {
            currentString = processedText.substring(endIdx, endIdx + initialLen).lowercase()

            var isValidInitial = false
            if (removeRegexCharacters) {
                isValidInitial = jyutpingInitials.contains(currentString)
            } else {
                val stringsToSearch = unfoldJyutpingRegex(currentString)
                for (s in stringsToSearch) {
                    isValidInitial = isValidInitial || jyutpingInitials.contains(s)
                }
            }

            if (!isValidInitial) {
                continue
            }

            if (initialFound) {
                // Multiple initials in a row are only valid if previous "initial"
                // was actually a final (like m or ng)
                val previousInitial = processedText.substring(startIdx, endIdx).lowercase()
                var previousInitialIsValidFinal = false
                if (removeRegexCharacters) {
                    previousInitialIsValidFinal = jyutpingFinals.contains(previousInitial)
                } else {
                    val stringsToSearch = unfoldJyutpingRegex(previousInitial)
                    for (s in stringsToSearch) {
                        previousInitialIsValidFinal =
                            previousInitialIsValidFinal || jyutpingFinals.contains(s)
                    }
                }

                if (previousInitialIsValidFinal) {
                    syllables.add(previousInitial)
                    startIdx = endIdx
                } else {
                    validJyutping = false
                }
            }

            endIdx += initialLen
            componentFound = true
            initialFound = true
            break
        }

        if (componentFound) {
            continue
        }

        // If final is valid, then extend end_index for length of final.
        // Check for number at end of word as well (this represents tone number).
        //
        // Then add the substring from [start_index, end_index) to vector
        // and reset start_index, so we can start searching after the end_index.
        remainingLength = processedText.length - endIdx
        val maxFinalLength =
            if (removeRegexCharacters) min(4, remainingLength) else min(16, remainingLength)
        for (finalLen in (1..maxFinalLength).reversed()) {
            currentString = processedText.substring(endIdx, endIdx + finalLen).lowercase()

            var isValidFinal = false
            if (removeRegexCharacters) {
                isValidFinal = jyutpingFinals.contains(currentString)
            } else {
                val stringsToSearch = unfoldJyutpingRegex(currentString)
                for (s in stringsToSearch) {
                    isValidFinal = isValidFinal || jyutpingFinals.contains(s)
                }
            }

            if (isValidFinal) {
                endIdx += finalLen
                if (endIdx < processedText.length) {
                    if (processedText[endIdx].isDigit()) {
                        if (!('1'..'6').contains(processedText[endIdx])) {
                            validJyutping = false
                        }
                        ++endIdx
                    }
                }

                val syllable = processedText.substring(startIdx, endIdx).lowercase()
                syllables.add(syllable)
                startIdx = endIdx
                componentFound = true
                initialFound = false
                break
            }
        }

        if (componentFound) {
            continue
        } else {
            validJyutping = false
        }

        ++endIdx
    }

    // Then add whatever's left in the search term, minus whitespace.
    val lastSyllable = processedText.substring(startIdx, endIdx).trim().lowercase()
    if (!lastSyllable.isEmpty() && lastSyllable != "'") {
        syllables.add(lastSyllable)
        if (!jyutpingFinals.contains(lastSyllable)) {
            validJyutping = false
        }
    }

    return Pair(validJyutping, syllables)
}