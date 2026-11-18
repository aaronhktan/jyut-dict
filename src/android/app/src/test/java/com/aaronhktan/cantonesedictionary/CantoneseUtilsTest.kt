package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.utils.convertJyutpingToYale
import com.aaronhktan.cantonesedictionary.logic.utils.segmentJyutping
import org.junit.Test

import org.junit.Assert.*

class CantoneseUtilsUnitTest {
    @Test
    fun cantoneseUtils_jyutpingToYale_simple() {
        assertEquals("sī jí sāan", convertJyutpingToYale("si1 zi2 saan1"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_empty() {
        assertEquals("", convertJyutpingToYale(""))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_rejectNoTone() {
        assertEquals("joeng", convertJyutpingToYale("joeng"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_rejectSingleLetter() {
        assertEquals("a", convertJyutpingToYale("a"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_rejectSpecialCharacter() {
        assertEquals("x", convertJyutpingToYale("-"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_noSpaces() {
        assertEquals("sī jí sāan", convertJyutpingToYale("si1zi2saan1"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_spacesToSegment() {
        assertEquals(
            "sī jí sāan",
            convertJyutpingToYale("si1 zi2 saan1", useSpacesToSegment = true)
        )
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_specialFinal() {
        assertEquals("geuk yúhn", convertJyutpingToYale("goek3jyun5"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_invalidFinal() {
        assertEquals(
            "bt1", convertJyutpingToYale("bt1", useSpacesToSegment = true)
        )
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_lightTone() {
        assertEquals("lohk yúh", convertJyutpingToYale("lok6 jyu5"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_specialSyllable() {
        assertEquals("m̀h haih", convertJyutpingToYale("m4 hai6"))
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_tones() {
        assertEquals(
            "sāam gáu sei lìhng ńgh yih chāt baat luhk",
            convertJyutpingToYale("saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6")
        )
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_noTone() {
        assertEquals(
            "mit", convertJyutpingToYale("mit")
        )
    }

    @Test
    fun cantoneseUtils_jyutpingToYale_malformedTone() {
        assertEquals(
            "x", convertJyutpingToYale("mat 7")
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_simple() {
        val expected = Pair(true, listOf("m4", "goi1"))
        assertEquals(expected, segmentJyutping("m4 goi1"))
    }

    @Test
    fun cantoneseUtils_segmentJyutping_noDigits() {
        val expected = Pair(true, listOf("m", "goi"))
        assertEquals(expected, segmentJyutping("m goi"))
    }

    @Test
    fun cantoneseUtils_segmentJyutping_noSpaces() {
        val expected = Pair(true, listOf("m4", "goi1"))
        assertEquals(expected, segmentJyutping("m4goi1"))
    }

    @Test
    fun cantoneseUtils_segmentJyutping_noDigitsNoSpaces() {
        val expected = Pair(true, listOf("m", "goi"))
        assertEquals(expected, segmentJyutping("mgoi"))
    }

    @Test
    fun cantoneseUtils_segmentJyutping_noDigitsApostrophe() {
        val expected = Pair(true, listOf("m", "aam"))
        assertEquals(expected, segmentJyutping("m'aam"))
    }

    @Test
    fun cantoneseUtils_segmentJyutping_digitsApostrophe() {
        val expected = Pair(true, listOf("m4", "aam"))
        assertEquals(expected, segmentJyutping("m4'aam"))
    }

    @Test
    fun cantoneseUtils_segmentJyutping_removeSpecialCharacters() {
        val expected = Pair(true, listOf("m", "aam"))
        assertEquals(expected, segmentJyutping("m*'aam"))
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepGlobCharacters() {
        val expected = Pair(true, listOf("m", "* ", "goi"))
        assertEquals(
            expected, segmentJyutping(
                "m* goi", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepGlobCharactersInvalidPreviousInitial() {
        val expected = Pair(false, listOf("x", "*"))
        assertEquals(
            expected, segmentJyutping(
                "x*", removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepGlobCharactersNoWhitespace() {
        val expected = Pair(true, listOf("m", "*", "goi"))
        assertEquals(
            expected, segmentJyutping(
                "m*goi", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepMultipleGlobCharacters() {
        val expected = Pair(true, listOf("m", "?", "* ", "goi"))
        assertEquals(
            expected, segmentJyutping(
                "m?* goi", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepMultipleGlobCharactersNoLetters() {
        val expected = Pair(true, listOf("*", "*", "*", "*"))
        assertEquals(
            expected, segmentJyutping(
                "****", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepMultipleGlobCharactersWhitespace() {
        val expected = Pair(true, listOf("m", "? ", "* ", "goi"))
        assertEquals(
            expected, segmentJyutping(
                "m? * goi", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepMultipleGlobCharactersWhitespaceSurround() {
        val expected = Pair(true, listOf("m", " ? ", "* ", "goi"))
        assertEquals(
            expected, segmentJyutping(
                "m ? * goi", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepMultipleGlobCharactersTrimWhitespace() {
        var expected = Pair(true, listOf("m", " ? ", "* ", "goi"))
        assertEquals(
            expected, segmentJyutping(
                "m  ?            *      goi",
                removeSpecialCharacters = true,
                removeGlobCharacters = false
            )
        )

        expected = Pair(true, listOf("m", "?", "* ", "?", "?", "?", "?"))
        assertEquals(
            expected, segmentJyutping(
                "m?* ????", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )

        expected = Pair(true, listOf("m", " * ", "?", "?", "?", "?", "*"))
        assertEquals(
            expected, segmentJyutping(
                "m * ????*", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )

        expected = Pair(true, listOf("m", "? ", "goi", "*"))
        assertEquals(
            expected, segmentJyutping(
                "m? goi*", removeSpecialCharacters = true, removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_keepSpecialCharacters() {
        val expected = Pair(true, listOf("m", "？", "goi"))
        assertEquals(
            expected, segmentJyutping("m？ goi", removeSpecialCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_unfoldBasicRegex() {
        var expected = Pair(false, listOf("(m|ng)", "goi"))
        assertEquals(
            expected, segmentJyutping("(m|ng) goi", removeRegexCharacters = false)
        )

        expected = Pair(true, listOf("mang!"))
        assertEquals(
            expected, segmentJyutping("mang!", removeRegexCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_unfoldPreviousInitial() {
        var expected = Pair(true, listOf("ng4"))
        assertEquals(
            expected, segmentJyutping("ng4", removeRegexCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_unfoldRegexFail() {
        var expected = Pair(false, listOf("((m)|ng)", "goi"))
        assertEquals(
            expected, segmentJyutping("((m)|ng) goi", removeRegexCharacters = false)
        )

        expected = Pair(false, listOf("(m!!!)", "goi"))
        assertEquals(
            expected, segmentJyutping("(m!!!) goi", removeRegexCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_removeWhitespace() {
        val expected = Pair(true, listOf("m", "goi"))
        assertEquals(
            expected,
            segmentJyutping(
                "  m                           goi      ",
                removeSpecialCharacters = true,
                removeGlobCharacters = false
            )
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_lower() {
        val expected = Pair(true, listOf("m", "goi"))
        assertEquals(
            expected,
            segmentJyutping("mGoI")
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_lowerWithDigits() {
        val expected = Pair(true, listOf("m4", "goi1"))
        assertEquals(
            expected,
            segmentJyutping("m4GoI1")
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_multipleFinalVowelsOnly() {
        var expected = Pair(true, listOf("aa", "aa"))
        assertEquals(
            expected,
            segmentJyutping("aaaa", removeSpecialCharacters = true, removeGlobCharacters = false)
        )

        expected = Pair(true, listOf("aa", "aa"))
        assertEquals(
            expected,
            segmentJyutping("aaaa", removeRegexCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_multipleFinalConsonantsOnly() {
        var expected = Pair(true, listOf("ng", "ng"))
        assertEquals(
            expected,
            segmentJyutping("ngng", removeSpecialCharacters = true, removeGlobCharacters = false)
        )

        expected = Pair(true, listOf("ng", "ng"))
        assertEquals(
            expected,
            segmentJyutping("ngng", removeRegexCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_multipleFinals() {
        val expected = Pair(true, listOf("am", "am"))
        assertEquals(
            expected,
            segmentJyutping("amam", removeSpecialCharacters = true, removeGlobCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_invalidTone() {
        var expected = Pair(false, listOf("hang0"))
        assertEquals(
            expected,
            segmentJyutping("hang0", removeSpecialCharacters = true, removeGlobCharacters = false)
        )

        expected = Pair(false, listOf("hang7"))
        assertEquals(
            expected,
            segmentJyutping("hang7", removeSpecialCharacters = true, removeGlobCharacters = false)
        )

        expected = Pair(false, listOf("0"))
        assertEquals(
            expected,
            segmentJyutping("0")
        )

        expected = Pair(false, listOf("ng0"))
        assertEquals(
            expected,
            segmentJyutping("ng0")
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_garbage() {
        val expected = Pair(false, listOf("kljnxclkjvnl"))
        assertEquals(
            expected,
            segmentJyutping(
                "kljnxclkjvnl",
                removeSpecialCharacters = true,
                removeGlobCharacters = false
            )
        )
    }
}