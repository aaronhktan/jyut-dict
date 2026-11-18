package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.utils.segmentJyutping
import org.junit.Test

import org.junit.Assert.*

class CantoneseUtilsUnitTest {
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
        val expected = Pair(true, listOf("aa", "aa"))
        assertEquals(
            expected,
            segmentJyutping("aaaa", removeSpecialCharacters = true, removeGlobCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_multipleFinalConsonantsOnly() {
        val expected = Pair(true, listOf("ng", "ng"))
        assertEquals(
            expected,
            segmentJyutping("ngng", removeSpecialCharacters = true, removeGlobCharacters = false)
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
        val expected = Pair(false, listOf("hang0"))
        assertEquals(
            expected,
            segmentJyutping("hang0", removeSpecialCharacters = true, removeGlobCharacters = false)
        )
    }

    @Test
    fun cantoneseUtils_segmentJyutping_garbage() {
        val expected = Pair(false, listOf("kljnxclkjvnl"))
        assertEquals(
            expected,
            segmentJyutping("kljnxclkjvnl", removeSpecialCharacters = true, removeGlobCharacters = false)
        )
    }
}