package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.utils.convertJyutpingToIPA
import com.aaronhktan.cantonesedictionary.logic.utils.convertJyutpingToYale
import com.aaronhktan.cantonesedictionary.logic.utils.jyutpingAutocorrect
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
    fun cantoneseUtils_jyutpingToIPA_simple() {
        assertEquals("jœ̽ːŋ˨˩ sɪŋ˨˩", convertJyutpingToIPA("joeng4 sing4"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_empty() {
        assertEquals("", convertJyutpingToIPA(""))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_rejectNoTone() {
        assertEquals("joeng", convertJyutpingToIPA("joeng"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_rejectSingleLetter() {
        assertEquals("a", convertJyutpingToIPA("a"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_rejectSpecialCharacter() {
        assertEquals("x", convertJyutpingToIPA("-"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_noSpaces() {
        assertEquals("fäː˥ sɪŋ˨˩", convertJyutpingToIPA("faa1sing4"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_spacesToSegment() {
        assertEquals(
            "jœ̽ːŋ˨˩ sɪŋ˨˩",
            convertJyutpingToIPA("joeng4 sing4", useSpacesToSegment = true)
        )
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_preprocessInitial() {
        assertEquals("t͡ʃyː˧˥ sʊŋ˧", convertJyutpingToIPA("zyu2 sung3"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_specialSyllable() {
        assertEquals("m̩˨˩", convertJyutpingToIPA("m4"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_checkedTone() {
        assertEquals("sɪk̚˨ siː˧˥ ɔː˥ fäːn˨", convertJyutpingToIPA("sik6 si2 o1 faan6"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_invalidNucleus() {
        assertEquals(
            "p@o˥",
            convertJyutpingToIPA("bao1", useSpacesToSegment = true)
        )
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_specialFinal() {
        assertEquals("ʊk̚˥ kʰei̯˧˥ jɐn˨˩", convertJyutpingToIPA("uk1 kei2 jan4"))
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_tones() {
        assertEquals(
            "säːm˥ kɐu̯˧˥ sei̯˧ lɪŋ˨˩ ŋ̍˩˧ jiː˨ t͡sʰɐt̚˥ päːt̚˧ lʊk̚˨",
            convertJyutpingToIPA("saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6")
        )
    }

    @Test
    fun cantoneseUtils_jyutpingToIPA_noTone() {
        assertEquals("mok", convertJyutpingToIPA("mok"))
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

    @Test
    fun cantoneseUtils_autocorrectJyutping_hui() {
        assertEquals("heoi", jyutpingAutocorrect("hui"))
        assertEquals("heoi", jyutpingAutocorrect("hui", unsafeSubstitutions = true))
        assertEquals("seoi", jyutpingAutocorrect("shui"))
        assertEquals("seoi", jyutpingAutocorrect("shui", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_cu() {
        assertEquals("(k)(eo|u)i", jyutpingAutocorrect("cui"))
        assertEquals("(k)(eo|u)i", jyutpingAutocorrect("cui", unsafeSubstitutions = true))
        assertEquals("(k)am", jyutpingAutocorrect("cum"))
        assertEquals("(k)am", jyutpingAutocorrect("cum", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_x() {
        assertEquals("s(yu)t", jyutpingAutocorrect("xuet"))
        assertEquals("s(yu)t", jyutpingAutocorrect("xuet", unsafeSubstitutions = true))
        assertEquals("s(eo|u)i", jyutpingAutocorrect("xui"))
        assertEquals("s(eo|u)i", jyutpingAutocorrect("xui", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ch() {
        assertEquals("coeng", jyutpingAutocorrect("choeng"))
        assertEquals("coeng", jyutpingAutocorrect("choeng", unsafeSubstitutions = true))
        assertEquals("ci", jyutpingAutocorrect("chi"))
        assertEquals("ci", jyutpingAutocorrect("chi", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_sh() {
        assertEquals("soeng", jyutpingAutocorrect("shoeng"))
        assertEquals("soeng", jyutpingAutocorrect("shoeng", unsafeSubstitutions = true))
        assertEquals("si", jyutpingAutocorrect("shi"))
        assertEquals("si", jyutpingAutocorrect("shi", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_zh() {
        assertEquals("zoeng", jyutpingAutocorrect("zhoeng"))
        assertEquals("zoeng", jyutpingAutocorrect("zhoeng", unsafeSubstitutions = true))
        assertEquals("zi", jyutpingAutocorrect("zhi"))
        assertEquals("zi", jyutpingAutocorrect("zhi", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_eungErngEong() {
        assertEquals("zoeng", jyutpingAutocorrect("zeung"))
        assertEquals("zoeng", jyutpingAutocorrect("zeung", unsafeSubstitutions = true))
        assertEquals("zoeng", jyutpingAutocorrect("zerng"))
        assertEquals("zoeng", jyutpingAutocorrect("zerng", unsafeSubstitutions = true))
        assertEquals("zoeng", jyutpingAutocorrect("zeong"))
        assertEquals("zoeng", jyutpingAutocorrect("zeong", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_eui() {
        assertEquals("zeoi", jyutpingAutocorrect("zeui"))
        assertEquals("zeoi", jyutpingAutocorrect("zeui", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_euk() {
        assertEquals("soek", jyutpingAutocorrect("seuk"))
        assertEquals("soek", jyutpingAutocorrect("seuk", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_eun() {
        assertEquals("c(eo|yu)n", jyutpingAutocorrect("ceun"))
        assertEquals("c(eo|yu)n", jyutpingAutocorrect("ceun", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_eut() {
        assertEquals("s(eo|yu)t", jyutpingAutocorrect("seut"))
        assertEquals("s(eo|yu)t", jyutpingAutocorrect("seut", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_eu() {
        assertEquals("z(e|y)u", jyutpingAutocorrect("zeu"))
        assertEquals("z(e|y)u", jyutpingAutocorrect("zeu", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ern() {
        assertEquals("zeon", jyutpingAutocorrect("zern"))
        assertEquals("zeon", jyutpingAutocorrect("zern", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_oen() {
        assertEquals("zeon", jyutpingAutocorrect("zoen"))
        assertEquals("zeon", jyutpingAutocorrect("zoen", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ao() {
        assertEquals("gau", jyutpingAutocorrect("gao"))
        assertEquals("gau", jyutpingAutocorrect("gao", unsafeSubstitutions = true))
        assertEquals("ga(ou!)l(ja|jyu|yu)n", jyutpingAutocorrect("gaolyun"))
        assertEquals("gaul(ja|jyu|yu)n", jyutpingAutocorrect("gaolyun", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ar() {
        assertEquals("caa", jyutpingAutocorrect("char"))
        assertEquals("caa", jyutpingAutocorrect("char", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ee() {
        assertEquals("si", jyutpingAutocorrect("see"))
        assertEquals("si", jyutpingAutocorrect("see", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ay() {
        assertEquals("hei", jyutpingAutocorrect("hay"))
        assertEquals("hei", jyutpingAutocorrect("hay", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_oy() {
        assertEquals("coi", jyutpingAutocorrect("choy"))
        assertEquals("coi", jyutpingAutocorrect("choy", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_oo() {
        assertEquals("s(y!u)t", jyutpingAutocorrect("soot"))
        assertEquals("s(y!u)t", jyutpingAutocorrect("soot", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ong() {
        assertEquals("f(o|u)ng", jyutpingAutocorrect("fong"))
        assertEquals("f(o|u)ng", jyutpingAutocorrect("fong", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_young() {
        assertEquals("j(y!u|a|eo)ng", jyutpingAutocorrect("young"))
        assertEquals("j(y!u|a|eo)ng", jyutpingAutocorrect("young", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_yue() {
        assertEquals("(j)(yu)t", jyutpingAutocorrect("yuet"))
        assertEquals("(j)(yu)t", jyutpingAutocorrect("yuet", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ue() {
        assertEquals("c(yu)n", jyutpingAutocorrect("tsuen"))
        assertEquals("c(yu)n", jyutpingAutocorrect("tsuen", unsafeSubstitutions = true))
        assertEquals("wants(yu)n", jyutpingAutocorrect("wantsuen"))
        assertEquals("wanc(yu)n", jyutpingAutocorrect("wantsuen", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_tsz() {
        assertEquals("zi", jyutpingAutocorrect("tsz"))
        assertEquals("zi", jyutpingAutocorrect("tsz", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ck() {
        assertEquals("bak", jyutpingAutocorrect("back"))
        assertEquals("bak", jyutpingAutocorrect("back", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ey() {
        assertEquals("gei ", jyutpingAutocorrect("gey "))
        assertEquals("gei ", jyutpingAutocorrect("gey ", unsafeSubstitutions = true))
        assertEquals("gei'", jyutpingAutocorrect("gey'"))
        assertEquals("gei'", jyutpingAutocorrect("gey'", unsafeSubstitutions = true))
        assertEquals("gei", jyutpingAutocorrect("gey"))
        assertEquals("gei", jyutpingAutocorrect("gey", unsafeSubstitutions = true))
        assertEquals("geihei", jyutpingAutocorrect("geyhey"))
        assertEquals("geihei", jyutpingAutocorrect("geyhey", unsafeSubstitutions = true))

        // Test with an initial that is only valid for the -ei combo
        assertEquals("peilou", jyutpingAutocorrect("peylou"))
        assertEquals("peilou", jyutpingAutocorrect("peylou", unsafeSubstitutions = true))

        // Test with an initial that is only valid for the -e j- combo
        assertEquals("(j)e (j)e", jyutpingAutocorrect("yeye"))
        assertEquals("(j)e (j)e", jyutpingAutocorrect("yeye", unsafeSubstitutions = true))

        // Test with ambiguous initial + initial after the "ey"
        assertEquals("beicaam", jyutpingAutocorrect("beycaam"))
        assertEquals("beicaam", jyutpingAutocorrect("beycaam", unsafeSubstitutions = true))

        // Test with ambiguous initial + non-initial after the "ey"
        assertEquals("ge (j)e", jyutpingAutocorrect("geye"))
        assertEquals("ge (j)e", jyutpingAutocorrect("geye", unsafeSubstitutions = true))

        assertEquals(
            "ge (j)ege (j)e beicaam(j)e (j)e (j)e (j)e (j)e (j)e (j)epeilougeiheigeigei'gei",
            jyutpingAutocorrect("geyegeye beycaamyeyeyeyeyeyeyepeylougeyheygeygey'gey")
        )
        assertEquals(
            "ge (j)ege (j)e beicaam(j)e (j)e (j)e (j)e (j)e (j)e (j)epeilougeiheigeigei'gei",
            jyutpingAutocorrect(
                "geyegeye beycaamyeyeyeyeyeyeyepeylougeyheygeygey'gey",
                unsafeSubstitutions = true
            )
        )

        // Test at beginning of string
        assertEquals("eib", jyutpingAutocorrect("eyb"))
        assertEquals("eib", jyutpingAutocorrect("eyb", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_oh() {
        assertEquals("mou ", jyutpingAutocorrect("moh "))
        assertEquals("mou ", jyutpingAutocorrect("moh ", unsafeSubstitutions = true))
        assertEquals("mou'", jyutpingAutocorrect("moh'"))
        assertEquals("mou'", jyutpingAutocorrect("moh'", unsafeSubstitutions = true))
        assertEquals("mou", jyutpingAutocorrect("moh"))
        assertEquals("mou", jyutpingAutocorrect("moh", unsafeSubstitutions = true))
        assertEquals("ouou", jyutpingAutocorrect("ohoh"))
        assertEquals("ouou", jyutpingAutocorrect("ohoh", unsafeSubstitutions = true))

        // Test with an initial that is only valid for the -ou combo
        assertEquals("noudoi", jyutpingAutocorrect("nohdoi"))
        assertEquals("noudoi", jyutpingAutocorrect("nohdoi", unsafeSubstitutions = true))

        // Test with ambiguous initial + initial after the "oh"
        assertEquals("loujan", jyutpingAutocorrect("lohjan"))
        assertEquals("loujan", jyutpingAutocorrect("lohjan", unsafeSubstitutions = true))

        // Test with ambiguous initial + non-initial after the "oh"
        assertEquals("l(ou!) hon", jyutpingAutocorrect("lohon"))
        assertEquals("l(ou!) hon", jyutpingAutocorrect("lohon", unsafeSubstitutions = true))

        assertEquals(
            "moumoumoul(ou!) honl(ou!) honloujannoudoimou mou",
            jyutpingAutocorrect("mohmohmohlohonlohonlohjannohdoimoh moh")
        )
        assertEquals(
            "moumoumoul(ou!) honl(ou!) honloujannoudoimou mou",
            jyutpingAutocorrect(
                "mohmohmohlohonlohonlohjannohdoimoh moh",
                unsafeSubstitutions = true
            )
        )
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ow() {
        assertEquals("gau ", jyutpingAutocorrect("gow "))
        assertEquals("gau ", jyutpingAutocorrect("gow ", unsafeSubstitutions = true))
        assertEquals("gau'", jyutpingAutocorrect("gow'"))
        assertEquals("gau'", jyutpingAutocorrect("gow'", unsafeSubstitutions = true))
        assertEquals("gau", jyutpingAutocorrect("gow"))
        assertEquals("gau", jyutpingAutocorrect("gow", unsafeSubstitutions = true))

        // Test with an initial that is only valid for the -au combo
        assertEquals("mau", jyutpingAutocorrect("mow"))
        assertEquals("mau", jyutpingAutocorrect("mow", unsafeSubstitutions = true))

        // Test with ambiguous initial + initial after the "ow"
        assertEquals("taugai", jyutpingAutocorrect("towgai"))
        assertEquals("taugai", jyutpingAutocorrect("towgai", unsafeSubstitutions = true))
        assertEquals("gaucat", jyutpingAutocorrect("gowcat"))
        assertEquals("gaucat", jyutpingAutocorrect("gowcat", unsafeSubstitutions = true))

        // Test with ambiguous initial + non-initial after the "ow"
        assertEquals("h(ou!) wu", jyutpingAutocorrect("howu"))
        assertEquals("h(ou!) wu", jyutpingAutocorrect("ho wu", unsafeSubstitutions = true))

        assertEquals(
            "maumaumauh(ou!) wuh(ou!) wu taugai",
            jyutpingAutocorrect("mowmowmowhowuho wu towgai")
        )
        assertEquals(
            "maumaumauh(ou!) wuh(ou!) wu taugai",
            jyutpingAutocorrect("mowmowmowhowuho wu towgai", unsafeSubstitutions = true)
        )

        // Test at beginning of input
        assertEquals("augai", jyutpingAutocorrect("owgai"))
        assertEquals("augai", jyutpingAutocorrect("owgai", unsafeSubstitutions = true))

        // Test with ) after initial
        assertEquals("(y!u)(ou!)wg", jyutpingAutocorrect("ooowg"))
        assertEquals("(y!u)(ou!)wg", jyutpingAutocorrect("ooowg", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_um() {
        assertEquals("gam ", jyutpingAutocorrect("gum "))
        assertEquals("gam ", jyutpingAutocorrect("gum ", unsafeSubstitutions = true))
        assertEquals("gam'", jyutpingAutocorrect("gum'"))
        assertEquals("gam'", jyutpingAutocorrect("gum'", unsafeSubstitutions = true))
        assertEquals("gam", jyutpingAutocorrect("gum"))
        assertEquals("gam", jyutpingAutocorrect("gum", unsafeSubstitutions = true))

        // Test with an initial that is only valid for the -am combo
        assertEquals("bam", jyutpingAutocorrect("bum"))
        assertEquals("bam", jyutpingAutocorrect("bum", unsafeSubstitutions = true))
        assertEquals(
            "bambambambam",
            jyutpingAutocorrect("bumbumbumbum", unsafeSubstitutions = true)
        )

        // Test with an initial that is only valid for the -u m- combo
        assertEquals("wumit", jyutpingAutocorrect("wumit"))
        assertEquals("wumit", jyutpingAutocorrect("wumit", unsafeSubstitutions = true))

        // Test with ambiguous initial + initial after the "um"
        assertEquals("gamzau", jyutpingAutocorrect("gumzau"))
        assertEquals("gamzau", jyutpingAutocorrect("gumzau", unsafeSubstitutions = true))

        // Test with ambiguous initial + non-initial after the "um"
        assertEquals("gu man", jyutpingAutocorrect("guman"))
        assertEquals("gu man", jyutpingAutocorrect("guman", unsafeSubstitutions = true))

        assertEquals(
            "gu mangamzauwumitbambambambam",
            jyutpingAutocorrect("gumangumzauwumitbumbumbumbum")
        )
        assertEquals(
            "gu mangamzauwumitbambambambam",
            jyutpingAutocorrect("gumangumzauwumitbumbumbumbum", unsafeSubstitutions = true)
        )

        // Test at beginning of input
        assertEquals("amzau", jyutpingAutocorrect("umzau"))
        assertEquals("amzau", jyutpingAutocorrect("umzau", unsafeSubstitutions = true))

        // Test with ) after initial
        assertEquals("(k)aman", jyutpingAutocorrect("cuman"))
        assertEquals("(k)aman", jyutpingAutocorrect("cuman", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_yum() {
        assertEquals("(j)am ", jyutpingAutocorrect("yum "))
        assertEquals("(j)am ", jyutpingAutocorrect("yum ", unsafeSubstitutions = true))
        assertEquals("(j)am'", jyutpingAutocorrect("yum'"))
        assertEquals("(j)am'", jyutpingAutocorrect("yum'", unsafeSubstitutions = true))
        assertEquals("(j)am", jyutpingAutocorrect("yum"))
        assertEquals("(j)am", jyutpingAutocorrect("yum", unsafeSubstitutions = true))

        // Test with an initial that is valid for the -yu m- combo
        assertEquals("cyu mat", jyutpingAutocorrect("cyumat"))
        assertEquals("cyu mat", jyutpingAutocorrect("cyumat", unsafeSubstitutions = true))

        // Test with an initial that is only valid for the jam combo
        assertEquals("syu(j)am", jyutpingAutocorrect("syuyum"))
        assertEquals("syu(j)am", jyutpingAutocorrect("syuyam", unsafeSubstitutions = true))
        assertEquals(
            "cyu matcyu matcyu matsyu(j)am(j)am(j)am",
            jyutpingAutocorrect("cyumatcyumatcyumatsyuyumyumyum")
        )
        assertEquals(
            "cyu matcyu matcyu macyu(j)am(j)am(j)am",
            jyutpingAutocorrect("cyumatcyumatcyumatsyuyumyumyum", unsafeSubstitutions = true)
        )

        // Test with ) after initial
        assertEquals("(z)yu mb", jyutpingAutocorrect("(z)yumb"))
        assertEquals("(z)yu mb", jyutpingAutocorrect("(z)yumb", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_yup() {
        assertEquals("(j)ap ", jyutpingAutocorrect("yup "))
        assertEquals("(j)ap ", jyutpingAutocorrect("yup ", unsafeSubstitutions = true))
        assertEquals("(j)ap'", jyutpingAutocorrect("yup'"))
        assertEquals("(j)ap'", jyutpingAutocorrect("yup'", unsafeSubstitutions = true))
        assertEquals("(j)ap", jyutpingAutocorrect("yup"))
        assertEquals("(j)ap", jyutpingAutocorrect("yup", unsafeSubstitutions = true))

        // Test with an initial that is valid for the -yu p- combo
        assertEquals("syu pei", jyutpingAutocorrect("syupei"))
        assertEquals("syu pei", jyutpingAutocorrect("syupei", unsafeSubstitutions = true))

        // Test with an initial that is only valid for the jap combo
        assertEquals("zeon(j)ap", jyutpingAutocorrect("zeonyup"))
        assertEquals("zeon(j)ap", jyutpingAutocorrect("zeonyup", unsafeSubstitutions = true))
        assertEquals(
            "zeon(j)apzeon(j)apsyu peisyu pei(j)ap(j)apzeon(j)ap",
            jyutpingAutocorrect("zeonyupzeonyupsyupeisyupeiyupyupzeonyup")
        )
        assertEquals(
            "zeon(j)apzeon(j)apsyu peisyu pei(j)ap(j)apzeon(j)ap",
            jyutpingAutocorrect(
                "zeonyupzeonyupsyupeisyupeiyupyupzeonyup",
                unsafeSubstitutions = true
            )
        )

        // Test with ) after initial
        assertEquals("(s)yu p", jyutpingAutocorrect("(s)yup"))
        assertEquals("(s)yu p", jyutpingAutocorrect("(s)yup", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_yuk() {
        assertEquals("(j)uk ", jyutpingAutocorrect("yuk "))
        assertEquals("(j)uk ", jyutpingAutocorrect("yuk ", unsafeSubstitutions = true))
        assertEquals(" (j)uk", jyutpingAutocorrect(" yuk"))
        assertEquals(" (j)uk", jyutpingAutocorrect(" yuk", unsafeSubstitutions = true))
        assertEquals("(j)uk'", jyutpingAutocorrect("yuk'"))
        assertEquals("(j)uk'", jyutpingAutocorrect("yuk'", unsafeSubstitutions = true))
        assertEquals("(j)uk", jyutpingAutocorrect("yuk"))
        assertEquals("(j)uk", jyutpingAutocorrect("yuk", unsafeSubstitutions = true))

        assertEquals("gei(j)uk", jyutpingAutocorrect("geyyuk"))
        assertEquals("gei(j)uk", jyutpingAutocorrect("geyyuk", unsafeSubstitutions = true))

        assertEquals("gei (j)uk", jyutpingAutocorrect("gey yuk"))
        assertEquals("gei (j)uk", jyutpingAutocorrect("gey yuk", unsafeSubstitutions = true))

        assertEquals("jyu kap", jyutpingAutocorrect("jyukap"))
        assertEquals("jyu kap", jyutpingAutocorrect("jyukap", unsafeSubstitutions = true))

        assertEquals("jyu kap", jyutpingAutocorrect("jyu kap"))
        assertEquals("jyu kap", jyutpingAutocorrect("jyu kap", unsafeSubstitutions = true))

        assertEquals(
            "jyu kapgei (j)ukgei(j)uk(j)uk(j)uk(j)uk(j)uk (j)uk",
            jyutpingAutocorrect("jyu kapgey yukgeyyukyukyukyukyuk yuk")
        )
        assertEquals(
            "jyu kapgei (j)ukgei(j)uk(j)uk(j)uk(j)uk(j)uk (j)uk",
            jyutpingAutocorrect("jyu kapgey yukgeyyukyukyukyukyuk yuk", unsafeSubstitutions = true)
        )

        // Test with ) after initial
        assertEquals("(z)yu k", jyutpingAutocorrect("(z)yuk"))
        assertEquals("(z)yu k", jyutpingAutocorrect("(z)yuk", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_yung() {
        assertEquals("(j)(y!u|a|eo)ng ", jyutpingAutocorrect("yung "))
        assertEquals("(j)(y!u|a|eo)ng ", jyutpingAutocorrect("yung ", unsafeSubstitutions = true))
        assertEquals(" (j)(y!u|a|eo)ng", jyutpingAutocorrect(" yung"))
        assertEquals(" (j)(y!u|a|eo)ng", jyutpingAutocorrect(" yung", unsafeSubstitutions = true))
        assertEquals("(j)(y!u|a|eo)ng'", jyutpingAutocorrect("yung'"))
        assertEquals("(j)(y!u|a|eo)ng'", jyutpingAutocorrect("yung'", unsafeSubstitutions = true))
        assertEquals("(j)(y!u|a|eo)ng", jyutpingAutocorrect("yung"))
        assertEquals("(j)(y!u|a|eo)ng", jyutpingAutocorrect("yung", unsafeSubstitutions = true))

        assertEquals("gam(j)(y!u|a|eo)ng", jyutpingAutocorrect("gumyung"))
        assertEquals(
            "gam(j)(y!u|a|eo)ng",
            jyutpingAutocorrect("gumyung", unsafeSubstitutions = true)
        )

        assertEquals("z(yu)n gaa", jyutpingAutocorrect("zyungaa"))
        assertEquals("z(yu)n gaa", jyutpingAutocorrect("zyungaa", unsafeSubstitutions = true))

        assertEquals("j(yu)n gin", jyutpingAutocorrect("jyungin"))
        assertEquals("j(yu)n gin", jyutpingAutocorrect("jyungin", unsafeSubstitutions = true))

        assertEquals(
            "j(yu)n gingam(j)(y!u|a|eo)ng (j)(y!u|a|eo)ng(j)(y!u|a|eo)ng(j)(y!u|a|eo)ngz(yu)n gaa",
            jyutpingAutocorrect("jyungingumyung yungyungyungzyungaa")
        )
        assertEquals(
            "j(yu)n gingam(j)(y!u|a|eo)ng (j)(y!u|a|eo)ng(j)(y!u|a|eo)ng(j)(y!u|a|eo)ngz(yu)n gaa",
            jyutpingAutocorrect("jyungingumyung yungyungyungzyungaa", unsafeSubstitutions = true)
        )

        // Test ) after initial
        assertEquals("(j)(yu)n g", jyutpingAutocorrect("(j)yung"))
        assertEquals("(j)(yu)n g", jyutpingAutocorrect("(j)yung", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_yun() {
        assertEquals("(j)(a|yu)n ", jyutpingAutocorrect("yun "))
        assertEquals("(j)(a|yu)n ", jyutpingAutocorrect("yun ", unsafeSubstitutions = true))
        assertEquals(" (ja|jyu|yu)n", jyutpingAutocorrect(" yun"))
        assertEquals(" (ja|jyu|yu)n", jyutpingAutocorrect(" yun", unsafeSubstitutions = true))
        assertEquals("(j)(a|yu)n'", jyutpingAutocorrect("yun'"))
        assertEquals("(j)(a|yu)n'", jyutpingAutocorrect("yun'", unsafeSubstitutions = true))
        assertEquals("(j)(a|yu)n", jyutpingAutocorrect("yun"))
        assertEquals("(j)(a|yu)n", jyutpingAutocorrect("yun", unsafeSubstitutions = true))

        assertEquals("gam(ja|jyu|yu)n", jyutpingAutocorrect("gumyun"))
        assertEquals("gam(ja|jyu|yu)n", jyutpingAutocorrect("gumyun", unsafeSubstitutions = true))

        assertEquals("sy(y!u|a|eo)ntau", jyutpingAutocorrect("syuntau"))
        assertEquals("sy(y!u|a|eo)ntau", jyutpingAutocorrect("syuntau", unsafeSubstitutions = true))

        assertEquals(
            "sy(y!u|a|eo)ntausy(y!u|a|eo)ntaugam(ja|jyu|yu)n(ja|jyu|yu)n (ja|jyu|yu)n(ja|jyu|yu)n(ja|jyu|yu)n",
            jyutpingAutocorrect("syuntausyuntaugumyunyun yunyunyun")
        )
        assertEquals(
            "sy(y!u|a|eo)ntausy(y!u|a|eo)ntaugam(ja|jyu|yu)n(ja|jyu|yu)n (ja|jyu|yu)n(ja|jyu|yu)n(ja|jyu|yu)n",
            jyutpingAutocorrect("syuntausyuntaugumyunyun yunyunyun", unsafeSubstitutions = true)
        )
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_yut() {
        assertEquals("(j)(a|yu)t ", jyutpingAutocorrect("yut "))
        assertEquals("(j)(a|yu)t ", jyutpingAutocorrect("yut ", unsafeSubstitutions = true))
        assertEquals(" (ja|jyu|yu)t", jyutpingAutocorrect(" yut"))
        assertEquals(" (ja|jyu|yu)t", jyutpingAutocorrect(" yut", unsafeSubstitutions = true))
        assertEquals("(j)(a|yu)t'", jyutpingAutocorrect("yut'"))
        assertEquals("(j)(a|yu)t'", jyutpingAutocorrect("yut'", unsafeSubstitutions = true))
        assertEquals("(j)(a|yu)t", jyutpingAutocorrect("yut"))
        assertEquals("(j)(a|yu)t", jyutpingAutocorrect("yut", unsafeSubstitutions = true))

        assertEquals("gam(ja|jyu|yu)t", jyutpingAutocorrect("gamyut"))
        assertEquals("gam(ja|jyu|yu)t", jyutpingAutocorrect("gamyut", unsafeSubstitutions = true))

        assertEquals("j(yu)tjyu", jyutpingAutocorrect("jyutjyu"))
        assertEquals("j(yu)tjyu", jyutpingAutocorrect("jyutjyu", unsafeSubstitutions = true))

        assertEquals("z(yu)tai", jyutpingAutocorrect("zyutai"))
        assertEquals("z(yu)tai", jyutpingAutocorrect("zyutai", unsafeSubstitutions = true))

        assertEquals(
            "z(yu)taij(yu)tjyugam(ja|jyu|yu)t (ja|jyu|yu)t (ja|jyu|yu)t(ja|jyu|yu)t(ja|jyu|yu)t",
            jyutpingAutocorrect("zyutaijyutjyugamyut yut yutyutyut")
        )
        assertEquals(
            "z(yu)taij(yu)tjyugam(ja|jyu|yu)t (ja|jyu|yu)t (ja|jyu|yu)t(ja|jyu|yu)t(ja|jyu|yu)t",
            jyutpingAutocorrect("zyutaijyutjyugamyut yut yutyutyut", unsafeSubstitutions = true)
        )
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_unsafe() {
        assertEquals("(g|k)w!(y!u|a|eo)n", jyutpingAutocorrect("kwun"))
        assertEquals("(g|k)w!(y!u|a|eo)n", jyutpingAutocorrect("kwun", unsafeSubstitutions = true))

        assertEquals("baakw(y!u|a|eo)n", jyutpingAutocorrect("baakwun"))
        assertEquals("baa(g|k)w!(y!u|a|eo)n", jyutpingAutocorrect("baakwun", unsafeSubstitutions = true))

        assertEquals("c(yu)n", jyutpingAutocorrect("tsuen"))
        assertEquals("c(yu)n", jyutpingAutocorrect("tsuen", unsafeSubstitutions = true))

        assertEquals("wants(yu)n", jyutpingAutocorrect("wantsuen"))
        assertEquals("wanc(yu)n", jyutpingAutocorrect("wantsuen", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_y() {
        assertEquals("(j)aang", jyutpingAutocorrect("yaang"))
        assertEquals("(j)aang", jyutpingAutocorrect("yaang", unsafeSubstitutions = true))

        assertEquals("(j)eng", jyutpingAutocorrect("yeng"))
        assertEquals("(j)eng", jyutpingAutocorrect("yeng", unsafeSubstitutions = true))

        assertEquals("(j)y(y!u|a|eo)n", jyutpingAutocorrect("yuen"))
        assertEquals("(j)y(y!u|a|eo)n", jyutpingAutocorrect("yuen", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ui() {
        assertEquals("g(eo|u)i", jyutpingAutocorrect("gui"))
        assertEquals("g(eo|u)i", jyutpingAutocorrect("gui", unsafeSubstitutions = true))
        assertEquals(" g(eo|u)i", jyutpingAutocorrect(" gui"))
        assertEquals(" g(eo|u)i", jyutpingAutocorrect(" gui", unsafeSubstitutions = true))
        assertEquals("(j)ams(eo|u)i", jyutpingAutocorrect("yumsui"))
        assertEquals("(j)ams(eo|u)i", jyutpingAutocorrect("yumsui", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_un() {
        assertEquals("g(y!u|a|eo)n", jyutpingAutocorrect("gun"))
        assertEquals("g(y!u|a|eo)n", jyutpingAutocorrect("gun", unsafeSubstitutions = true))
        assertEquals(" g(y!u|a|eo)n", jyutpingAutocorrect(" gun"))
        assertEquals(" g(y!u|a|eo)n", jyutpingAutocorrect(" gun", unsafeSubstitutions = true))
        assertEquals("g(y!u|a|eo)nzoeng", jyutpingAutocorrect("gunzoeng"))
        assertEquals("g(y!u|a|eo)nzoeng", jyutpingAutocorrect("gunzoeng", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_ut() {
        assertEquals("g(a|y!u)t", jyutpingAutocorrect("gut"))
        assertEquals("g(a|y!u)t", jyutpingAutocorrect("gut", unsafeSubstitutions = true))
        assertEquals(" g(a|y!u)t", jyutpingAutocorrect(" gut"))
        assertEquals(" g(a|y!u)t", jyutpingAutocorrect(" gut", unsafeSubstitutions = true))
        assertEquals("gamg(a|y!u)t", jyutpingAutocorrect("gumgut"))
        assertEquals("gamg(a|y!u)t", jyutpingAutocorrect("gumgut", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_oh_afterRegexFinal() {
        assertEquals("(n)oua", jyutpingAutocorrect("(n)oha"))
        assertEquals("(n)oua", jyutpingAutocorrect("(n)oha", unsafeSubstitutions = true))
    }

    @Test
    fun cantoneseUtils_autocorrectJyutping_finalO() {
        assertEquals("l(ou!)", jyutpingAutocorrect("lo"))
        assertEquals("l(ou!)", jyutpingAutocorrect("lo", unsafeSubstitutions = true))
        assertEquals("(ou!)", jyutpingAutocorrect("o"))
        assertEquals("(ou!)", jyutpingAutocorrect("o", unsafeSubstitutions = true))
    }
}