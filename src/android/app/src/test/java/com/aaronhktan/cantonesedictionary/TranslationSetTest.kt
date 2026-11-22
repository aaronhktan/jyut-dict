package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.example.Translation
import com.aaronhktan.cantonesedictionary.logic.example.TranslationSet
import junit.framework.TestCase.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class TranslationSetUnitTest {
    @Test
    fun translationSet_isEmpty_simple() {
        val ts = TranslationSet("CC-CANTO", listOf())
        assertEquals(true, ts.isEmpty())
    }

    @Test
    fun translationSet_isEmpty_false() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val ts = TranslationSet("CC-CANTO", listOf(translation))
        assertEquals(false, ts.isEmpty())
    }

    @Test
    fun translationSet_clone_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))

        val secondTranslationSet = translationSet.clone()

        assertFalse(translationSet === secondTranslationSet)
        assertTrue(translationSet == secondTranslationSet)
        assertTrue(translationSet.hashCode() == secondTranslationSet.hashCode())
    }

    @Test
    fun translationSet_equals_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))

        val secondTranslation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val secondTranslationSet =
            TranslationSet("粵典—words.hk", translations = listOf(secondTranslation))

        assertFalse(translationSet === secondTranslationSet)
        assertTrue(translationSet == secondTranslationSet)

        val thirdTranslationSet = translationSet.clone()
        assertFalse(translationSet === thirdTranslationSet)
        assertTrue(translationSet == thirdTranslationSet)
    }

    @Test
    fun translationSet_equals_false() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))

        val secondTranslation = Translation(
            content = "small egg",
            language = "eng",
            directTarget = true
        )
        val secondTranslationSet =
            TranslationSet("粵典—words.hk", translations = listOf(secondTranslation))

        assertFalse(translationSet === secondTranslationSet)
        assertFalse(translationSet == secondTranslationSet)

        val thirdTranslation = Translation(
            content = "egg waffle",
            language = "fra",
            directTarget = true
        )
        val thirdTranslationSet = TranslationSet("粵典—words.hk", translations = listOf(thirdTranslation))
        assertFalse(translationSet === thirdTranslationSet)
        assertFalse(translationSet == thirdTranslationSet)
    }

    @Test
    fun translationSet_hashCode_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))

        val secondTranslation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val secondTranslationSet =
            TranslationSet("粵典—words.hk", translations = listOf(secondTranslation))

        assertFalse(translationSet === secondTranslationSet)
        assertTrue(translationSet.hashCode() == secondTranslationSet.hashCode())
    }

    @Test
    fun translationSet_hashCode_notEqual() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))

        val secondTranslation = Translation(
            content = "small egg",
            language = "eng",
            directTarget = true
        )
        val secondTranslationSet =
            TranslationSet("粵典—words.hk", translations = listOf(secondTranslation))

        assertFalse(translationSet === secondTranslationSet)
        assertFalse(translationSet.hashCode() == secondTranslationSet.hashCode())

        val thirdTranslation = Translation(
            content = "egg waffle",
            language = "fra",
            directTarget = true
        )
        val thirdTranslationSet = TranslationSet("粵典—words.hk", translations = listOf(thirdTranslation))
        assertFalse(translationSet === thirdTranslationSet)
        assertFalse(translationSet.hashCode() == thirdTranslationSet.hashCode())
    }

    @Test
    fun translationSet_getTranslationSnippet_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))

        assertEquals(listOf(translation), translationSet.getTranslationSnippet())
    }

    @Test
    fun translationSet_getTranslationSnippet_empty() {
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf())
        assertEquals(listOf<Translation>(), translationSet.getTranslationSnippet())
    }

    @Test
    fun translationSet_translation_copy() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationList = listOf(translation)
        val translationSet = TranslationSet("粵典—words.hk", translations = translationList)

        assertTrue(translationList == translationSet.translations)
        assertFalse(translationList === translationSet.translations)
        assertFalse(translationSet.translations === translationSet.translations)
    }

    @Test
    fun translationSet_source_simple() {
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf())
        assertEquals("粵典—words.hk", translationSet.source)
    }
}