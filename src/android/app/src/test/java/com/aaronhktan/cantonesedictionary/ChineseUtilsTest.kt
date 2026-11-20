@file:Suppress("SpellCheckingInspection")

package com.aaronhktan.cantonesedictionary

import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.withStyle
import com.aaronhktan.cantonesedictionary.data.datastore.defaultJyutpingToneColours
import com.aaronhktan.cantonesedictionary.data.datastore.defaultPinyinToneColours
import com.aaronhktan.cantonesedictionary.logic.entry.EntryColourPhoneticType
import com.aaronhktan.cantonesedictionary.logic.utils.SAME_CHARACTER_STRING
import com.aaronhktan.cantonesedictionary.logic.utils.applyColours
import com.aaronhktan.cantonesedictionary.logic.utils.compareStrings
import com.aaronhktan.cantonesedictionary.logic.utils.constructRomanisationQuery
import org.junit.Assert.assertEquals
import org.junit.Test

class ChineseUtilsUnitTest {
    @Test
    fun chineseUtils_applyColours_jyutping() {
        val expected = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[4])) {
                append("唔")
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[6])) {
                append("係")
            }
        }

        val text = "唔係"
        val tones = listOf(4, 6)
        assertEquals(
            expected, applyColours(
                text = text,
                tones = tones,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = listOf(),
                type = EntryColourPhoneticType.CANTONESE
            )
        )
    }

    @Test
    fun chineseUtils_applyColours_pinyin() {
        val expected = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultPinyinToneColours[2])) {
                append("不")
            }
            withStyle(SpanStyle(color = defaultPinyinToneColours[4])) {
                append("是")
            }
        }

        val text = "不是"
        val tones = listOf(2, 4)
        assertEquals(
            expected, applyColours(
                text = text,
                tones = tones,
                jyutpingToneColours = listOf(),
                pinyinToneColours = defaultPinyinToneColours,
                type = EntryColourPhoneticType.MANDARIN
            )
        )
    }

    @Test
    fun chineseUtils_applyColours_skipSameCharacters() {
        val expected = buildAnnotatedString {
            append(SAME_CHARACTER_STRING)
            append(SAME_CHARACTER_STRING)
        }

        val text = SAME_CHARACTER_STRING + SAME_CHARACTER_STRING
        val tones = listOf(4, 6)
        assertEquals(
            expected, applyColours(
                text = text,
                tones = tones,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = listOf(),
                type = EntryColourPhoneticType.CANTONESE
            )
        )
    }

    @Test
    fun chineseUtils_applyColours_variousCodePoints() {
        val expected = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[1])) {
                append(String(Character.toChars((0x3400))))
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[2])) {
                append(String(Character.toChars(0x20000)))
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[3])) {
                append(String(Character.toChars(0x2A700)))
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[4])) {
                append(String(Character.toChars(0x2B740)))
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[5])) {
                append(String(Character.toChars(0x2B820)))
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[6])) {
                append(String(Character.toChars(0x2CEB0)))
            }
            append(String(Character.toChars(0x200)))
        }

        val text = buildString {
            appendCodePoint(0x3400)
            appendCodePoint(0x20000)
            appendCodePoint(0x2A700)
            appendCodePoint(0x2B740)
            appendCodePoint(0x2B820)
            appendCodePoint(0x2CEB0)
            appendCodePoint(0x200)
        }
        val tones = listOf(1, 2, 3, 4, 5, 6, 6)
        assertEquals(
            expected, applyColours(
                text = text,
                tones = tones,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = listOf(),
                type = EntryColourPhoneticType.CANTONESE
            )
        )
    }

    @Test
    fun chineseUtils_applyColours_invalidCantoneseTones() {
        val expected = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[4])) {
                append("唔")
            }
            append("係")
        }

        val text = "唔係"
        val tones = listOf(4, 7)
        assertEquals(
            expected, applyColours(
                text = text,
                tones = tones,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = listOf(),
                type = EntryColourPhoneticType.CANTONESE
            )
        )
    }

    @Test
    fun chineseUtils_applyColours_invalidMandarinTones() {
        val expected = buildAnnotatedString {
            append("不")
            withStyle(SpanStyle(color = defaultPinyinToneColours[4])) {
                append("是")
            }
        }

        val text = "不是"
        val tones = listOf(7, 4)
        assertEquals(
            expected, applyColours(
                text = text,
                tones = tones,
                jyutpingToneColours = listOf(),
                pinyinToneColours = defaultPinyinToneColours,
                type = EntryColourPhoneticType.MANDARIN
            )
        )
    }

    @Test
    fun chineseUtils_applyColours_notEnoughTones() {
        val expected = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[4])) {
                append("唔")
            }
            append("係")
        }

        val text = "唔係"
        val tones = listOf(4)
        assertEquals(
            expected, applyColours(
                text = text,
                tones = tones,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = listOf(),
                type = EntryColourPhoneticType.CANTONESE
            )
        )
    }

    @Test
    fun chineseUtils_compareStrings_simple() {
        assertEquals("语" + SAME_CHARACTER_STRING + "艺术", compareStrings("語言藝術", "语言艺术"))
    }

    @Test
    fun chineseUtils_compareStrings_singleMultibyteGrapheme() {
        assertEquals("赗", compareStrings("賵", "赗"))
    }

    @Test
    fun chineseUtils_compareStrings_multipleMultibyteGraphemes() {
        assertEquals("\uD873\uDE88龁", compareStrings("齮齕", "\uD873\uDE88龁"))
    }

    @Test
    fun chineseUtils_compareStrings_multipleMultibyteGraphemesWithAlpha() {
        assertEquals(
            "𬺈" + SAME_CHARACTER_STRING
                    + SAME_CHARACTER_STRING + SAME_CHARACTER_STRING + SAME_CHARACTER_STRING + "龁",
            compareStrings("齮aaaa齕", "\uD873\uDE88aaaa龁")
        )
    }

    @Test
    fun chineseUtils_compareStrings_compatibilityVariantNormalization() {
        assertEquals(
            SAME_CHARACTER_STRING, compareStrings("響", "響")
        )
    }

    @Test
    fun chineseUtils_constructRomanisationQuery_empty() {
        assertEquals("", constructRomanisationQuery(listOf(), delimiter = "?"))
    }

    @Test
    fun chineseUtils_constructRomanisationQuery_singleSyllable() {
        assertEquals("se?", constructRomanisationQuery(listOf("se"), delimiter = "?"))
        assertEquals("se2", constructRomanisationQuery(listOf("se2"), delimiter = "?"))
        assertEquals("se*?", constructRomanisationQuery(listOf("se*"), delimiter = "?"))
        assertEquals("se??", constructRomanisationQuery(listOf("se?"), delimiter = "?"))
    }

    @Test
    fun chineseUtils_constructRomanisationQuery_multiSyllable() {
        assertEquals("se? dak?", constructRomanisationQuery(listOf("se", "dak"), delimiter = "?"))
        assertEquals("se2 dak1", constructRomanisationQuery(listOf("se2", "dak1"), delimiter = "?"))
        assertEquals(
            "se*? dak*?",
            constructRomanisationQuery(listOf("se*", "dak*"), delimiter = "?")
        )
        assertEquals(
            "se?? dak??",
            constructRomanisationQuery(listOf("se?", "dak?"), delimiter = "?")
        )
    }

    @Test
    fun chineseUtils_constructRomanisationQuery_globCharacters() {
        assertEquals("se? *", constructRomanisationQuery(listOf("se", " *"), delimiter = "?"))
        assertEquals(
            "se? ???",
            constructRomanisationQuery(listOf("se", " ?", "?", "?"), delimiter = "?")
        )
        assertEquals(
            "se? ??? dak?",
            constructRomanisationQuery(listOf("se", " ?", "?", "? ", "dak"), delimiter = "?")
        )
        assertEquals("se*", constructRomanisationQuery(listOf("se", "*"), delimiter = "?"))
    }

    @Test
    fun chineseUtils_constructRomanisationQuery_onlyGlobCharacters() {
        assertEquals(
            "????? ????",
            constructRomanisationQuery(
                listOf("?", "?", "?", "?", "? ", "?", "?", "?", "?"),
                delimiter = "?"
            )
        )
    }
}