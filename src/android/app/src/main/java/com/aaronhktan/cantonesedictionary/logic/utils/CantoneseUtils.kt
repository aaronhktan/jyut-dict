package com.aaronhktan.cantonesedictionary.logic.utils

import kotlin.math.min

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