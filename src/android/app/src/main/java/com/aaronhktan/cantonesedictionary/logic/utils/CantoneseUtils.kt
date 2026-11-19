package com.aaronhktan.cantonesedictionary.logic.utils

import android.util.Log
import kotlin.math.min

private const val LOG_TAG = "CantoneseUtils"

private val jyutpingInitials: Set<String> = setOf(
    "b", "p", "m", "f", "d",
    "t", "n", "l", "g", "k",
    "ng", "h", "gw", "kw", "w",
    "z", "c", "s", "j",
)

@Suppress("SpellCheckingInspection")
private val jyutpingFinals: Set<String> = setOf(
    "a", "aa", "aai", "aau", "aam", "aan", "aang", "aap", "aat", "aak",
    "ai", "au", "am", "an", "ang", "ap", "at", "ak", "e", "ei",
    "eu", "em", "en", "eng", "ep", "ek", "i", "iu", "im", "in",
    "ing", "ip", "it", "ik", "o", "oi", "ou", "on", "ong", "ot",
    "ok", "u", "ui", "un", "ung", "ut", "uk", "oe", "oet", "eoi",
    "eon", "oeng", "eot", "oek", "yu", "yun", "yut", "m", "ng",
)

private val jyutpingFinalRegex = Regex(pattern = "([aeiou][aeiou]?[iumngptk]?g?)([1-6])")

private val jyutpingToYaleSpecialSyllables: Map<String, List<String>> = mapOf(
    "m" to listOf("m̄", "ḿ", "m", "m̀h", "ḿh", "mh"),
    "ng" to listOf("n̄g", "ńg", "ng", "ǹgh", "ńgh", "ngh"),
)

@Suppress("SpellCheckingInspection")
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
private val cantoneseIPASpecialSyllableRegex = Regex("^(h?)([mn]g?)([1-6])$")
private val cantoneseIPAToneRegex = Regex("[1-6]")
private val cantoneseIPACheckedToneRegex = Regex("([ptk])([136])")
private val jyutpingToIPATones: List<String> = listOf(
    "˥", "˧˥", "˧", "˨˩", "˩˧", "˨", "˥", "˧", "˨",
)

private fun unfoldJyutpingRegex(jyutping: String): List<String> {
    val out: MutableList<String> = mutableListOf()
    val stringPossibilities: MutableList<String> = mutableListOf()

    // Invariant: there must be only one set of parentheses and one exclamation
    if (jyutping.count { it == '(' } > 1
        || jyutping.count { it == ')' } > 1
        || jyutping.count { it == '!' } > 1
    ) {
        return out
    }

    // If there is a parenthesis, then all options must be checked
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
        when (val regexIdx = s.indexOf('!')) {
            -1 -> {
                out.add(s)
            }

            0 -> {
                out.add(s.substring(1))
            }

            else -> {
                var tmp = s
                tmp = tmp.removeRange(regexIdx - 1..<regexIdx + 1)
                out.add(tmp)
                tmp = s
                tmp = tmp.removeRange(regexIdx..<regexIdx + 1)
                out.add(tmp)
            }
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
        yaleSyllable = yaleSyllable.replace(yaleLightToneClusterRegex, $$"h$0")
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

    var syllables: MutableList<String>
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

    val yaleSyllables = mutableListOf<String>()
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

@Suppress("SpellCheckingInspection")
private fun convertIPACantoneseSyllable(jyutping: String): String {
    val match = cantoneseIPASyllableRegex.find(jyutping)
    if (match == null) {
        Log.e(LOG_TAG, "Invalid jyutping found in IPA conversion: $jyutping")
        return jyutping
    }

    val (matchedInitial, matchedNucleus, matchedCoda, matchedTone) = match.destructured

    var initial = ""
    if (matchedInitial.isNotEmpty()) {
        cantoneseIPAInitials[matchedInitial]?.let {
            initial = it
        } ?: run {
            initial = matchedInitial
        }
    }

    var nucleus = ""
    if (matchedNucleus.isNotEmpty()) {
        cantoneseIPANuclei[matchedNucleus]?.let {
            nucleus = it
        } ?: run {
            nucleus = matchedNucleus
        }
    }

    var coda = ""
    if (matchedCoda.isNotEmpty()) {
        cantoneseIPACodas[matchedCoda]?.let {
            coda = it
        } ?: run {
            coda = matchedCoda
        }
    }

    var tone = ""
    if (matchedTone.isNotEmpty()) {
        tone = jyutpingToIPATones[matchedTone.toInt() - 1]
    }

    return initial + nucleus + coda + tone
}

fun convertJyutpingToIPA(jyutping: String, useSpacesToSegment: Boolean = false): String {
    if (jyutping.isEmpty()) {
        return jyutping
    }

    var syllables: MutableList<String>
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

    val ipaSyllables = mutableListOf<String>()
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
        cantoneseIPASpecialSyllableRegex.find(ipaSyllable)?.let { specialMatch ->
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
    val syllables: MutableList<String> = mutableListOf()
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
                    && syllables.isNotEmpty()
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
    if (lastSyllable.isNotEmpty() && lastSyllable != "'") {
        syllables.add(lastSyllable)
        if (!jyutpingFinals.contains(lastSyllable)) {
            validJyutping = false
        }
    }

    return Pair(validJyutping, syllables)
}

@Suppress("SpellCheckingInspection")
fun jyutpingAutocorrect(text: String, unsafeSubstitutions: Boolean = false): String {
    var out = text

    out = out
        // This is for some romanizations like "shui" for 水
        // And needs to happen before the "sh" -> "s" conversion
        .replace("hui", "heoi")
        // The initial + nucleus "cu-" never appears in Jyutping, so the user
        // probably intended to make the IPA [kʰɐ] sound
        // Surround the k with capturing group to prevent replacement with (g|k)
        // if sound changes are enabled
        .replace("cu", "(k)u")
        // "x" never appears in Jyutping, the user might be more familiar
        // with Pinyin and assume that it's an "s" sound
        .replace("x", "s")
        .replace("ch", "c")
        .replace("sh", "s")
        .replace("zh", "z")
        // Change "eung", "erng", "eong" -> "oeng"
        .replace("eung", "oeng")
        .replace("erng", "oeng")
        .replace("eui", "eoi")
        .replace("euk", "oek")
        .replace("eun", "(eo|yu)n")
        .replace("eut", "(eo|yu)t")
        .replace("eu", "(e|y)u")
        .replace("ern", "eon")

    // Change "-oen" -> "-eon"
    var idx = out.indexOf("oen")
    while (idx != -1) {
        if (out[min(out.length - 1, idx + 4)] == 'g') {
            idx = out.indexOf("oen", min(out.length - 1, idx + 4))
            continue
        }
        out = out.replaceRange(idx..<idx + 3, "eon")
        idx = out.indexOf("oen", idx + 3)
    }

    out = out
        .replace("oei", "eoi")
        .replace("oet", "eot")

    out = out
        .replace("eong ", "oeng ")
        .replace("eong'", "oeng'")
    if (out.endsWith("eong")) {
        out = out.replaceRange(out.length - 4..<out.length, "oeng")
    }
    if (unsafeSubstitutions) {
        out = out.replace("eong", "oeng") // unsafe because of zeon6 gun2
    }

    out = out
        .replace("eok", "oek")

    out = out
        .replace("ao ", "au ")
        .replace("ao'", "au'")
    if (out.endsWith("ao")) {
        out = out.replaceRange(out.length - 2..<out.length, "au")
    }
    if (unsafeSubstitutions) {
        out = out.replace("ao", "au") // unsafe because of maa5 on1
    }

    out = out
        .replace("ar", "aa") // like in "char siu"
        .replace("ee", "i") // like in "lai see"
        .replace("ay", "ei") // like in "gong hay fat choy"
        .replace("oy", "oi") // like in "choy sum"
        .replace("oo", "(y!u)") // like in "soot goh"
        .replace("ong", "(o|u)ng")
        .replace("young", "jung") // like in "foo young"
        .replace("yue", "(j)yu") // like "yuet yue" (粵語)
        .replace("ue", "(yu)") // like "tsuen wan" (轉彎)
        .replace("tsz", "zi") // like "tsat tsz mui" (七姊妹)
        .replace("ck", "k") // like "back" (白)

    // The following changes may be unsafe because it is ambiguous whether
    // they are final + initial or a "misspelling" of a final
    // However, it is unambiguous if there is a separator at the end of the
    // syllable, or it is the end of the string

    // Check if the user intends to write an [-ɔː h-] or [-ou̯] cluster
    run {
        out = out
            .replace("oh ", "ou ")
            .replace("oh'", "ou'")
        if (out.endsWith("oh")) {
            out = out.replaceRange(out.length - 2..<out.length, "ou")
        }

        // Initials for which <initial> + "-ou" exist in Jyutping
        val closeBackVowelCluster = setOf('n', 'j')
        val ambiguousVowelCluster =
            setOf('b', 'p', 'm', 'f', 'd', 't', 'l', 'g', 'h', 'w', 'z', 'c', 's')

        var replacementIdx = out.indexOf("oh")
        while (replacementIdx != -1) {
            if (replacementIdx == 0) {
                out = out.replaceRange(0..<2, "ou")
            } else {
                var initialIdx = replacementIdx - 1
                if (replacementIdx > 1 && out[initialIdx] == ')') {
                    --initialIdx
                }

                if (closeBackVowelCluster.contains(out[initialIdx])) {
                    out = out.replaceRange(replacementIdx..<replacementIdx + 2, "ou")
                } else if (ambiguousVowelCluster.contains(out[initialIdx])) {
                    // The [h-] cluster can only occur if what follows is not an initial
                    var initialFound = false
                    for (initialLen in (1..2).reversed()) {
                        val offset = min(out.length - replacementIdx, 2 + initialLen)
                        val endIdx = replacementIdx + offset
                        val s = out.substring(replacementIdx + 2..<endIdx)
                        if (jyutpingInitials.contains(s) || s == "y") {
                            initialFound = true
                        }
                    }

                    out = if (initialFound) {
                        out.replaceRange(replacementIdx..<replacementIdx + 2, "ou")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 2, "o h")
                    }
                }
            }

            replacementIdx = out.indexOf("oh", replacementIdx + 2)
        }
    }

    // Check if the user intends to write an [-ɔː w-] or [-auː] cluster
    run {
        out = out
            .replace("ow ", "au ")
            .replace("ow'", "au'")
        if (out.endsWith("ow")) {
            out = out.replaceRange(out.length - 2..<out.length, "au")
        }

        // Initials for which <initial> + "-(a)au" exist in Jyutping
        val closeBackVowelCluster = setOf('b', 'm', 'k', 's')
        val ambiguousVowelCluster =
            setOf('p', 'm', 'f', 'd', 't', 'n', 'l', 'g', 'h', 'z', 'c', 's')

        var replacementIdx = out.indexOf("ow")
        while (replacementIdx != -1) {
            if (replacementIdx == 0) {
                out = out.replaceRange(0..<2, "au")
            } else {
                var initialIdx = replacementIdx - 1
                if (replacementIdx > 1 && out[initialIdx] == ')') {
                    --initialIdx
                }

                if (closeBackVowelCluster.contains(out[initialIdx])) {
                    out = out.replaceRange(replacementIdx..<replacementIdx + 2, "au")
                } else if (ambiguousVowelCluster.contains(out[initialIdx])) {
                    var initialFound = false
                    for (initialLen in (1..2).reversed()) {
                        val offset = min(out.length - replacementIdx, 2 + initialLen)
                        val endIdx = replacementIdx + offset
                        val s = out.substring(replacementIdx + 2..<endIdx)
                        if (jyutpingInitials.contains(s) || s == "y") {
                            initialFound = true
                        }
                    }

                    out = if (initialFound) {
                        out.replaceRange(replacementIdx..<replacementIdx + 2, "au")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 2, "o w")
                    }
                }
            }

            replacementIdx = out.indexOf("ow", replacementIdx + 2)
        }
    }

    // Check if the user intends to write an [ɐm] or [-uː  m-] cluster
    run {
        out = out
            .replace("um ", "am ")
            .replace("um'", "am'")
        if (out.endsWith("um")) {
            out = out.replaceRange(out.length - 2..<out.length, "am")
        }

        // Initials for which <initial> + "am" exist in Jyutping
        val openMidCentralVowelCluster =
            setOf('b', 'p', 'm', 'd', 't', 'n', 'l', 'k', 'h', 'z', 'c', 's', 'j')
        // Initials for which <initial> + "-u m-" exist in Jyutping
        val closeBackVowelCluster =
            setOf('f', 'w', 'a', 'e', 'i', 'o')

        var replacementIdx = out.indexOf("um")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<2, "am")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }
                    if (openMidCentralVowelCluster.contains(out[initialIdx])) {
                        out = out.replaceRange(replacementIdx..<replacementIdx + 2, "am")
                    } else if (closeBackVowelCluster.contains(out[initialIdx])) {
                        @Suppress("UNUSED_EXPRESSION") Unit
                    } else if (out[initialIdx] == 'g') {
                        // The [m-] cluster can only occur if what follows is not an initial
                        var initialFound = false
                        for (initialLen in (1..2).reversed()) {
                            val offset = min(out.length - replacementIdx, 2 + initialLen)
                            val endIdx = replacementIdx + offset
                            val s = out.substring(replacementIdx + 2..<endIdx)
                            if (jyutpingInitials.contains(s) || s == "y") {
                                initialFound = true
                            }
                        }

                        out = if (initialFound) {
                            out.replaceRange(replacementIdx..<replacementIdx + 2, "am")
                        } else {
                            out.replaceRange(replacementIdx..<replacementIdx + 2, "u m")
                        }
                    }
                }
            }

            replacementIdx = out.indexOf("um", replacementIdx + 2)
        }
    }

    // Check if the user intends to write an [-ɛː j-] or [-ei̯] cluster
    run {
        out = out
            .replace("ey ", "ei ")
            .replace("ey'", "ei'")
        if (out.endsWith("ey")) {
            out = out.replaceRange(out.length - 2..<out.length, "ei")
        }

        // Initials for which <initial> + "-ei" exist in Jyutping
        val closeFrontVowelCluster = setOf('p', 'f', 'd', 'n', 'l', 'h', 'w')
        // Initials for which <initial> + "-e j-" exist in Jyutping
        val openMidFrontVowelCluster = setOf('c', 'j', 'y')
        // Initials for which <initial> + "-u m-" exist in Jyutping
        val ambiguousVowelCluster = setOf('b', 'm', 'g', 'k', 'z', 's')

        var replacementIdx = out.indexOf("ey")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<2, "ei")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }

                    if (closeFrontVowelCluster.contains(out[initialIdx])) {
                        out = out.replaceRange(replacementIdx..<replacementIdx + 2, "ei")
                    } else if (openMidFrontVowelCluster.contains(out[initialIdx])) {
                        out = out.replaceRange(replacementIdx..<replacementIdx + 2, "e (j)")
                    } else if (ambiguousVowelCluster.contains(out[initialIdx])) {
                        // The [j-] cluster can only occur if what follows is not an initial
                        var initialFound = false
                        for (initialLen in (1..2).reversed()) {
                            val offset = min(out.length - replacementIdx, 2 + initialLen)
                            val endIdx = replacementIdx + offset
                            val s = out.substring(replacementIdx + 2..<endIdx)
                            if (jyutpingInitials.contains(s) || s == "y") {
                                initialFound = true
                            }
                        }

                        out = if (initialFound) {
                            out.replaceRange(replacementIdx..<replacementIdx + 2, "ei")
                        } else {
                            out.replaceRange(replacementIdx..<replacementIdx + 2, "e (j)")
                        }
                    }
                }
            }

            replacementIdx = out.indexOf("ey", replacementIdx + 2)
        }
    }

    // Check if the user intends to write an [-yː j-] or [-ɐm] cluster
    run {
        // Initials for which <initial> + "-yu m-" exist in Jyutping
        val closeFrontVowelCluster = setOf('z', 'c', 's', 'j')
        var replacementIdx = out.indexOf("yum")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<3, "(j)am")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }

                    out = if (closeFrontVowelCluster.contains(out[initialIdx])) {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "yu m")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "(j)am")
                    }
                }
            }

            replacementIdx = out.indexOf("yum", replacementIdx + 3)
        }
    }

    // Check if the user intends to write an [-yː p-] or [-ɐp] cluster
    run {
        // Initials for which <initial> + "-yu p-" exist in Jyutping
        val closeFrontVowelCluster = setOf('z', 's', 'j')
        var replacementIdx = out.indexOf("yup")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<3, "(j)ap")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }

                    out = if (closeFrontVowelCluster.contains(out[initialIdx])) {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "yu p")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "(j)ap")
                    }
                }
            }

            replacementIdx = out.indexOf("yup", replacementIdx + 3)
        }
    }

    // Check if the user intends to write an [-yː k-] or [jʊk] cluster
    run {
        // Initials for which <initial> + "-yu k-" exist in Jyutping
        val closeFrontVowelCluster = setOf('z', 's', 'c', 'j')
        var replacementIdx = out.indexOf("yuk")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<3, "(j)uk")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }

                    out = if (closeFrontVowelCluster.contains(out[initialIdx])) {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "yu k")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "(j)uk")
                    }
                }
            }

            replacementIdx = out.indexOf("yuk", replacementIdx + 3)
        }
    }

    // Check if the user intends to write an [-yn g-] or [jʊŋ] cluster
    run {
        // Initials for which <initial> + "-yun g-" exist in Jyutping
        val closeFrontVowelCluster = setOf('z', 's', 'c', 'j')
        var replacementIdx = out.indexOf("yung")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<4, "(j)ung")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }

                    out = if (closeFrontVowelCluster.contains(out[initialIdx])) {
                        out.replaceRange(replacementIdx..<replacementIdx + 4, "(yu)n g")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 4, "(j)ung")
                    }
                }
            }

            replacementIdx = out.indexOf("yung", replacementIdx + 3)
        }
    }

    // Check if the user intends to write an [-yn] or [jɐn], [jyn], [yn] cluster
    run {
        // Initials for which <initial> + "-yu n-" exist in Jyutping
        val closeFrontVowelCluster = setOf('z', 's', 'c', 'j')
        var replacementIdx = out.indexOf("yun")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<3, "(j)(a|yu)n")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }

                    out = if (closeFrontVowelCluster.contains(out[initialIdx])) {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "yun")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "(ja|jyu|yu)n")
                    }
                }
            }

            replacementIdx = out.indexOf("yun", replacementIdx + 3)
        }
    }

    // Check if the user intends to write an [-yt] or [jɐt], [jyt], [yt] cluster
    run {
        // Initials for which <initial> + "-yu t-" exist in Jyutping
        val closeFrontVowelCluster = setOf('z', 's', 'c', 'j')
        var replacementIdx = out.indexOf("yut")
        while (replacementIdx != -1) {
            when (replacementIdx) {
                0 -> {
                    out = out.replaceRange(0..<3, "(j)(a|yu)t")
                }

                else -> {
                    var initialIdx = replacementIdx - 1
                    if (replacementIdx > 1 && out[initialIdx] == ')') {
                        --initialIdx
                    }

                    out = if (closeFrontVowelCluster.contains(out[initialIdx])) {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "(yu)t")
                    } else {
                        out.replaceRange(replacementIdx..<replacementIdx + 3, "(ja|jyu|yu)t")
                    }
                }
            }

            replacementIdx = out.indexOf("yut", replacementIdx + 3)
        }
    }

    // Unsafe because it is ambiguous whether these are final + initial
    // or a "misspelling" of an initial
    // But unambiguous if they are at the start of a syllable
    if (out.startsWith("ts")) {
        out = out.replaceRange(0..<2, "c")
    }
    out = out.replace(" ts", "c")
    if (out.startsWith("kwu")) {
        out = out.replaceRange(0..<3, "(g|k)w!u")
    }
    out = out.replace(" kwu", " (g|k)w!u")
    if (unsafeSubstitutions) {
        out = out.replace("ts", "c")
        out = out.replace("kwu", "(g|k)w!u")
    }

    // Change any "y" that is not followed by a "u" to "j"
    // This needs to happen before the final replacements
    var yIdx = out.indexOf("y")
    while (yIdx != -1) {
        val substr = out.substring(yIdx)
        if (substr.startsWith("yu") || substr.startsWith("y!u") || substr.startsWith("y)u")) {
            yIdx = out.indexOf("y", yIdx + 1)
            continue
        }
        out = out.replaceRange(yIdx..<yIdx + 1, "(j)")
        yIdx = out.indexOf("y", yIdx + 1)
    }

    out = out
        .replace("ui", "(eo|u)i")
        .replace("un", "(y!u|a|eo)n")
        .replace("ut", "(a|y!u)t")

    out = out.replace("o ", "(ou!) ")
    out = out.replace("o'", "(ou!)'")
    if (out.endsWith("o")) {
        out = out.replaceRange(out.length - 1..<out.length, "(ou!)")
    }
    var oIdx = out.indexOf("o")
    while (oIdx != -1) {
        val letterAfter = out[oIdx + 1]
        if (!setOf('k', 't', 'e', 'i', 'u', 'n', ')', '|').contains(letterAfter)) {
            out = out.replaceRange(oIdx..<oIdx + 1, "(ou!)")
        }
        oIdx = out.indexOf("o", oIdx + 1)
    }

    return out
}

fun jyutpingSoundChanges(syllables: List<String>): List<String> {
    val changedSyllables: MutableList<String> = mutableListOf()

    for (syllable in syllables) {
        if (syllable.isEmpty()) {
            changedSyllables.add(syllable)
            continue
        }

        // Whole-syllable sound changes
        if (syllable == "ng" ||
            (syllable.length == 3 && syllable.dropLast(1) == "ng" &&
                    (syllable.last() == '?' || syllable.last().isDigit()))
        ) {
            val replacementIdx = syllable.lastIndexOf("ng")
            changedSyllables.add(
                syllable.replaceRange(
                    replacementIdx..<replacementIdx + 2,
                    "(ng|m)"
                )
            )
            continue
        } else if (syllable == "m" ||
            (syllable.length == 2 && syllable.dropLast(1) == "m" &&
                    (syllable.last() == '?' || syllable.last().isDigit()))
        ) {
            val replacementIdx = syllable.indexOf("m")
            changedSyllables.add(
                syllable.replaceRange(
                    replacementIdx..<replacementIdx + 1,
                    "(ng|m)"
                )
            )
            continue
        }

        // Initial sound changes
        // "Lazy" pronunciations
        var changedSyllable = syllable
        if (changedSyllable.length >= 3 && changedSyllable.startsWith("ng") && !changedSyllable[2].isDigit() && changedSyllable[2] != '?') {
            // loss of [ŋ] initial, replacement with null initial
            changedSyllable = changedSyllable.replaceRange(0..<2, "(ng)!")
        } else if (setOf('a', 'o', 'u').contains(changedSyllable[0])) {
            // merging of null initial with initial [ŋ] before [a, ɐ, ɔ, o]
            changedSyllable = "(ng)!$changedSyllable"
        } else if (setOf('n', 'l').contains(changedSyllable[0])) {
            // merge of [n] and [l] initials
            changedSyllable = changedSyllable.replaceRange(0..<1, "(n|l)")
        } else if (changedSyllable.startsWith("go")
            || changedSyllable.startsWith("ko")
            || changedSyllable.startsWith("g(o")
            || changedSyllable.startsWith("k(o")
        ) {
            // merging of [k]/[kʷ] and [kʰ]/[kʷʰ] initials before [ɔ]
            if (changedSyllable[0] == 'g') {
                changedSyllable = changedSyllable.replaceRange(0..<1, "gw!")
            } else if (changedSyllable[0] == 'k') {
                changedSyllable = changedSyllable.replaceRange(0..<1, "kw!")
            }
        }

        // Lack of distinction between aspirated and unaspirated initials
        if (setOf('d', 't').contains(changedSyllable[0])) {
            changedSyllable = changedSyllable.replaceRange(0..<1, "(d|t)")
        } else if (setOf('c', 'z').contains(changedSyllable[0])) {
            changedSyllable = changedSyllable.replaceRange(0..<1, "(c|z)")
        } else if (setOf('g', 'k').contains(changedSyllable[0])) {
            changedSyllable = changedSyllable.replaceRange(0..<1, "(g|k)")
        }

        // Palatization of alveolar affricate
        if (changedSyllable[0] == 'j') {
            changedSyllable = changedSyllable.replaceRange(0..<1, "(j|z)")
        }

        // Nucleus sound changes
        // Merge between [aː] and [ɐ]
        var idx = changedSyllable.indexOf("a")
        while (idx != -1) {
            val endIdx = min(changedSyllable.length, idx + 2)
            if (changedSyllable.substring(idx..<endIdx) == "aa") {
                changedSyllable = "${changedSyllable.substring(0..<endIdx)}!${
                    changedSyllable.substring(
                        endIdx..<changedSyllable.length
                    )
                }"
                idx = changedSyllable.indexOf("a", startIndex = endIdx + 1)
            } else {
                changedSyllable = "${changedSyllable.substring(0..<idx + 1)}a!${
                    changedSyllable.substring(
                        idx + 1..<changedSyllable.length
                    )
                }"
                idx = changedSyllable.indexOf("a", startIndex = idx + 2)
            }
        }

        // Final sound changes
        val choppedSyllable = changedSyllable.dropLast(1)
        if (changedSyllable.endsWith("ang")
            || changedSyllable.endsWith("a!ng")
            || changedSyllable.endsWith("ong")
            || choppedSyllable.endsWith("ang")
            || choppedSyllable.endsWith("a!ng")
            || choppedSyllable.endsWith("ong")
        ) {
            // alveolarization of final [ŋ]
            val replacementIdx = changedSyllable.lastIndexOf("ng")
            changedSyllable =
                changedSyllable.replaceRange(replacementIdx..<replacementIdx + 2, "ng!")
        } else if (changedSyllable.endsWith("an")
            || changedSyllable.endsWith("a!n")
            || changedSyllable.endsWith("on")
            || choppedSyllable.endsWith("an")
            || choppedSyllable.endsWith("a!n")
            || choppedSyllable.endsWith("on")
        ) {
            // velarization of final [n]
            val replacementIdx = changedSyllable.lastIndexOf("n")
            changedSyllable =
                changedSyllable.replaceRange(replacementIdx..<replacementIdx + 1, "ng!")
        } else if (
            (changedSyllable.endsWith("t")
                    && !changedSyllable.endsWith("it")
                    && !changedSyllable.endsWith("ut"))
            || (choppedSyllable.endsWith("t")
                    && !choppedSyllable.endsWith("it")
                    && !choppedSyllable.endsWith("ut"))
        ) {
            // velarization of final [t]
            val replacementIdx = changedSyllable.lastIndexOf("t")
            changedSyllable =
                changedSyllable.replaceRange(replacementIdx..<replacementIdx + 1, "(k|t)")
        } else if (
            (changedSyllable.endsWith("k")
                    && !changedSyllable.endsWith("ik")
                    && !changedSyllable.endsWith("uk"))
            || (choppedSyllable.endsWith("k")
                    && !choppedSyllable.endsWith("ik")
                    && !choppedSyllable.endsWith("uk"))
        ) {
            // velarization of final [k]
            val replacementIdx = changedSyllable.lastIndexOf("k")
            changedSyllable =
                changedSyllable.replaceRange(replacementIdx..<replacementIdx + 1, "(k|t)")
        }

        changedSyllables.add(changedSyllable)
    }

    return changedSyllables.toList()
}