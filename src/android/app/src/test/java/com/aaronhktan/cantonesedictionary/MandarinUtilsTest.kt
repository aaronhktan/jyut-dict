@file:Suppress("SpellCheckingInspection")

package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.utils.convertPinyinToZhuyin
import com.aaronhktan.cantonesedictionary.logic.utils.createNumberedPinyin
import com.aaronhktan.cantonesedictionary.logic.utils.createPinyinWithV
import com.aaronhktan.cantonesedictionary.logic.utils.createPrettyPinyin
import com.aaronhktan.cantonesedictionary.logic.utils.segmentPinyin
import org.junit.Test

import org.junit.Assert.*

class MandarinUtilsUnitTest {
    @Test
    fun mandarinUtils_prettyPinyin_simple() {
        assertEquals("shuài gē", createPrettyPinyin("shuai4 ge1"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_empty() {
        assertEquals("", createPrettyPinyin(""))
    }

    @Test
    fun mandarinUtils_prettyPinyin_rejectNoTone() {
        assertEquals("ba", createPrettyPinyin("ba"))
        assertEquals("nu", createPrettyPinyin("nu"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_rejectSingleLetter() {
        assertEquals("a", createPrettyPinyin("a"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_rejectSpecialCharacter() {
        assertEquals("-", createPrettyPinyin("-"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_secondaryVowel() {
        assertEquals("huì tú", createPrettyPinyin("hui4 tu2"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_umlaut() {
        assertEquals("nǚ hái", createPrettyPinyin("nu:3 hai2"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_noVowel() {
        assertEquals("mmm", createPrettyPinyin("mmm"))
        assertEquals("hng5", createPrettyPinyin("hng5"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_tones() {
        assertEquals("mā má mǎ mà ma", createPrettyPinyin("ma1 ma2 ma3 ma4 ma5"))
    }

    @Test
    fun mandarinUtils_prettyPinyin_invalidTone() {
        assertEquals("ma", createPrettyPinyin("ma0"))
        assertEquals("ma6", createPrettyPinyin("ma6"))
    }

    @Test
    fun mandarinUtils_numberedPinyin_simple() {
        assertEquals("nü3 hai2", createNumberedPinyin("nu:3 hai2"))
    }

    @Test
    fun mandarinUtils_pinyinWithV_simple() {
        assertEquals("nv3 hai2", createPinyinWithV("nu:3 hai2"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_simple() {
        assertEquals("ㄅㄚ ㄉㄚˊ ㄊㄨㄥ", convertPinyinToZhuyin("ba1 da2 tong1"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_empty() {
        assertEquals("", convertPinyinToZhuyin(""))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_rejectNoTone() {
        assertEquals("ba", convertPinyinToZhuyin("ba"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_rejectSingleLetter() {
        assertEquals("a", convertPinyinToZhuyin("a"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_rejectSpecialCharacter() {
        assertEquals("-", convertPinyinToZhuyin("-"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_noSpaces() {
        assertEquals("ㄅㄚ ㄉㄚˊ ㄊㄨㄥ", convertPinyinToZhuyin("ba1da2tong1"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_spacesToSegment() {
        assertEquals(
            "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ",
            convertPinyinToZhuyin("ba1 da2 tong1", useSpacesToSegment = true)
        )
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_specialInitial() {
        assertEquals("ㄑㄩˋ", convertPinyinToZhuyin("qu4"))
        assertEquals("ㄔ", convertPinyinToZhuyin("chi1"))
        assertEquals("ㄖˋ", convertPinyinToZhuyin("ri4"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_specialFinals() {
        assertEquals("˙ㄏㄇ", convertPinyinToZhuyin("hm5"))
        assertEquals("˙ㄏㄫ", convertPinyinToZhuyin("hng5"))
        assertEquals("ㄦˊ", convertPinyinToZhuyin("er2"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_erHua() {
        assertEquals("ㄑㄩㄢㄦ", convertPinyinToZhuyin("quanr1"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_invalidFinal() {
        assertEquals("ma1n", convertPinyinToZhuyin("ma1n", useSpacesToSegment = true))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_invalidTone() {
        assertEquals("ㄅㄚ", convertPinyinToZhuyin("ba0"))
        assertEquals("ba6", convertPinyinToZhuyin("ba6"))
    }

    @Test
    fun mandarinUtils_pinyinToZhuyin_malformed() {
        assertEquals("chzng2 ㄑㄩㄢㄦ", convertPinyinToZhuyin("chzng2 quanr1"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_simple() {
        assertEquals(Pair(true, listOf("guang3", "dong1")), segmentPinyin("guang3 dong1"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_noDigits() {
        assertEquals(Pair(true, listOf("guang", "dong")), segmentPinyin("guang dong"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_noSpaces() {
        assertEquals(Pair(true, listOf("guang3", "dong1")), segmentPinyin("guang3dong1"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_noDigitsnoSpaces() {
        assertEquals(Pair(true, listOf("guang", "dong")), segmentPinyin("guangdong"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_noDigitsApostrophe() {
        assertEquals(Pair(true, listOf("xi", "an")), segmentPinyin("xi'an"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_digitsApostrophe() {
        assertEquals(Pair(true, listOf("xi1", "an")), segmentPinyin("xi1'an"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_danglingInitialBeforeWhitespace() {
        assertEquals(
            Pair(false, listOf("guang3", "b", "dong1")),
            segmentPinyin("guang3 b dong1")
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_removeSpecialCharacters() {
        assertEquals(Pair(true, listOf("guang", "dong")), segmentPinyin("guang。dong？"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_keepGlobCharacters() {
        assertEquals(
            Pair(true, listOf("guang", "* ", "dong", "?")),
            segmentPinyin("guang* dong?", removeGlobCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_keepGlobCharactersNoWhitespace() {
        assertEquals(
            Pair(true, listOf("guang", "* ", "dong", "?")),
            segmentPinyin("guang* dong?", removeGlobCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_keepMultipleGlobCharactersNoWhitespace() {
        assertEquals(
            Pair(true, listOf("guang", "?", "* ", "dong", "?")),
            segmentPinyin("guang?* dong?", removeGlobCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_keepMultipleGlobCharactersWhitespace() {
        assertEquals(
            Pair(true, listOf("guang", "? ", "* ", "dong")),
            segmentPinyin("guang? * dong", removeGlobCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_keepMultipleGlobCharactersWhitespaceSurround() {
        assertEquals(
            Pair(true, listOf("guang", " ? ", "* ", "dong")),
            segmentPinyin("guang ? * dong", removeGlobCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_globCharactersTrimWhitespace() {
        assertEquals(
            Pair(true, listOf("guang", " ? ", "* ", "dong")),
            segmentPinyin("guang  ?            *      dong", removeGlobCharacters = false)
        )

        assertEquals(
            Pair(true, listOf("guang", "?", "* ", "?", "?", "?", "?", "?")),
            segmentPinyin("guang?* ?????", removeGlobCharacters = false)
        )

        assertEquals(
            Pair(true, listOf("guang", " * ", "?", "?", "?", "?", "*")),
            segmentPinyin("guang * ????*", removeGlobCharacters = false)
        )

        assertEquals(
            Pair(true, listOf("guang", "? ", "dong", "*")),
            segmentPinyin("guang? dong*", removeGlobCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_keepGlobCharactersAfterInvalidCharacter() {
        assertEquals(
            Pair(false, listOf("9", "*")),
            segmentPinyin("9*", removeGlobCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_keepSpecialCharacters() {
        assertEquals(
            Pair(true, listOf("guang", "？", "dong1")),
            segmentPinyin("guang？ dong1", removeSpecialCharacters = false)
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_removeWhitespace() {
        assertEquals(
            Pair(true, listOf("guang", "dong")),
            segmentPinyin("  guang                           dong      ")
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_lower() {
        assertEquals(
            Pair(true, listOf("guang", "dong")), segmentPinyin("gUanGdOnG")
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_lowerWithDigits() {
        assertEquals(
            Pair(true, listOf("guang3", "dong1")), segmentPinyin("gUanG3dOnG1")
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_multipleFinalsVowelsOnly() {
        assertEquals(
            Pair(true, listOf("e", "e")), segmentPinyin("ee")
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_multipleFinals() {
        assertEquals(
            Pair(true, listOf("ang", "ang")), segmentPinyin("angang")
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_erhua() {
        assertEquals(Pair(true, listOf("huar")), segmentPinyin("huar"))

        assertEquals(Pair(true, listOf("huar1")), segmentPinyin("huar1"))

        assertEquals(Pair(true, listOf("huar4", "dong1")), segmentPinyin("huar4 dong1"))
    }

    @Test
    fun mandarinUtils_segmentPinyin_invalidTone() {
        assertEquals(
            Pair(false, listOf("heng0")), segmentPinyin("heng0")
        )

        assertEquals(
            Pair(false, listOf("heng6")), segmentPinyin("heng6")
        )
    }

    @Test
    fun mandarinUtils_segmentPinyin_garbage() {
        assertEquals(
            Pair(false, listOf("kljnxclkjvnl")), segmentPinyin("kljnxclkjvnl")
        )
    }
}