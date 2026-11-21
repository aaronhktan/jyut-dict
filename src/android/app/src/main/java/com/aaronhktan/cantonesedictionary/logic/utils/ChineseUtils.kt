package com.aaronhktan.cantonesedictionary.logic.utils

import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import com.aaronhktan.cantonesedictionary.logic.entry.EntryColourPhoneticType
import java.text.Normalizer

val specialCharacters: Set<String> = setOf(
".", "。", ",", "，", "！", "？", "%", "－", "…", "⋯",
"·", "\"", "“", "”", "$", "｜", "：", "１", "２", "３",
"４", "５", "６", "７", "８", "９", "０",
)

val regexCharacters: Set<String> = setOf("!", "(", ")", "|")

private val unicodePunctuationCategories: Set<Byte> = setOf(
    Character.CONNECTOR_PUNCTUATION,
    Character.DASH_PUNCTUATION,
    Character.START_PUNCTUATION,
    Character.END_PUNCTUATION,
    Character.INITIAL_QUOTE_PUNCTUATION,
    Character.FINAL_QUOTE_PUNCTUATION,
    Character.OTHER_PUNCTUATION,
)

private val unicodeNumberCategories: Set<Byte> = setOf(
    Character.DECIMAL_DIGIT_NUMBER,
    Character.LETTER_NUMBER,
    Character.OTHER_NUMBER,
)

private val unicodeWhitespaceCategories: Set<Byte> = setOf(
    Character.SPACE_SEPARATOR,
    Character.LINE_SEPARATOR,
    Character.PARAGRAPH_SEPARATOR,
)

fun applyColours(
    text: String,
    tones: List<Int>,
    jyutpingToneColours: List<Color>,
    pinyinToneColours: List<Color>,
    type: EntryColourPhoneticType
): AnnotatedString {
    return buildAnnotatedString {
        var offset = 0
        val nfc = Normalizer.normalize(text, Normalizer.Form.NFC)
        var toneIdx = 0
        while (offset < text.length) {
            val codePoint = nfc.codePointAt(offset)
            val currentString = Character.toString(codePoint)

            try {
                if (currentString == SAME_CHARACTER_STRING) {
                    append(currentString)
                    ++toneIdx
                    continue
                }

                val isSpecialCharacter = specialCharacters.contains(currentString)
                val isIdeograph = (codePoint in 0x4E00..0x9FFF)  // CJK Unified Ideographs
                        || (codePoint in 0x3400..0x4DBF)  // CJK Unified Ideographs Extension A
                        || (codePoint in 0x20000..0x2A6DF)  // CJK Unified Ideographs Extension B
                        || (codePoint in 0x2A700..0x2B73F)  // CJK Unified Ideographs Extension C
                        || (codePoint in 0x2B740..0x2B81F)  // CJK Unified Ideographs Extension D
                        || (codePoint in 0x2B820..0x2CEAF)  // CJK Unified Ideographs Extension E
                        || (codePoint in 0x2CEB0..0x2EBEF)  // CJK Unified Ideographs Extension F
                if (isSpecialCharacter || !isIdeograph) {
                    append(currentString)
                    continue
                }

                if (toneIdx >= tones.size) {
                    append(currentString)
                    continue
                }
                val tone = tones[toneIdx]

                var colour: Color?
                when (type) {
                    EntryColourPhoneticType.CANTONESE -> {
                        if (tone >= jyutpingToneColours.size) {
                            append(currentString)
                            ++toneIdx
                            continue
                        }
                        colour = jyutpingToneColours[tone]
                    }

                    EntryColourPhoneticType.MANDARIN -> {
                        if (tone >= pinyinToneColours.size) {
                            append(currentString)
                            ++toneIdx
                            continue
                        }
                        colour = pinyinToneColours[tone]
                    }

                    else -> {
                        break
                    }
                }

                pushStyle(SpanStyle(color = colour))
                append(currentString)
                pop()
                ++toneIdx
            } finally {
                offset += Character.charCount(codePoint)
            }
        }

        toAnnotatedString()
    }
}

fun compareStrings(original: String, comparison: String): String {
    var result = ""

    val originalNfc = Normalizer.normalize(original, Normalizer.Form.NFC)
    val comparisonNfc = Normalizer.normalize(comparison, Normalizer.Form.NFC)

    var originalOffset = 0
    var comparisonOffset = 0

    while (originalOffset < originalNfc.length && comparisonOffset < comparisonNfc.length) {
        val originalCodePoint = originalNfc.codePointAt(originalOffset)
        val originalSubString = Character.toString(originalCodePoint)

        val comparisonCodePoint = comparisonNfc.codePointAt(comparisonOffset)
        val comparisonSubString = Character.toString(comparisonCodePoint)

        try {
            if (specialCharacters.contains(comparisonSubString)
                || Character.getType(comparisonCodePoint).toByte() in unicodePunctuationCategories
                || Character.getType(comparisonCodePoint).toByte() in unicodeNumberCategories
                || Character.getType(comparisonCodePoint).toByte() in unicodeWhitespaceCategories
                || originalSubString != comparisonSubString
            ) {
                result += comparisonSubString
                continue
            }

            result += SAME_CHARACTER_STRING
        } finally {
            originalOffset += Character.charCount(originalCodePoint)
            comparisonOffset += Character.charCount(comparisonCodePoint)
        }
    }

    return result
}

fun constructRomanisationQuery(
    syllables: List<String>,
    delimiter: String
): String {
    if (syllables.isEmpty()) {
        return ""
    }

    var addedDelimiter = false
    var space = ""

    var result = ""
    for (s in syllables) {
        val trimmedSyllable = s.trim()
        if (trimmedSyllable.isEmpty()) {
            continue
        }
        if (trimmedSyllable.last().isDigit()) {
            result += space + trimmedSyllable
            space = " "
            addedDelimiter = false
        } else if (trimmedSyllable in setOf("*", "?")) {
            if (s in setOf("*", "?", "* ", "? ") && addedDelimiter) {
                // Replace delimiter with GLOB wildcard if GLOB wildcard
                // was attached to end of previous word (i.e. wildcard did not
                // start with a space)
                result = result.dropLast(delimiter.length)
            }
            result += s
            // GLOB characters handle their own spacing, so don't insert any
            // extra spaces.
            space = ""
            addedDelimiter = false
        } else {
            result += space + s + delimiter
            space = " "
            addedDelimiter = true
        }
    }

    return result
}