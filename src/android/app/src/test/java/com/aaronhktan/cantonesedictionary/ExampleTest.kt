package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.entry.CantoneseOptions
import com.aaronhktan.cantonesedictionary.logic.entry.EntryCharactersOptions
import com.aaronhktan.cantonesedictionary.logic.entry.EntryPhoneticOptions
import com.aaronhktan.cantonesedictionary.logic.entry.MandarinOptions
import com.aaronhktan.cantonesedictionary.logic.example.Example
import com.aaronhktan.cantonesedictionary.logic.example.Translation
import com.aaronhktan.cantonesedictionary.logic.example.TranslationSet
import junit.framework.TestCase.assertEquals
import junit.framework.TestCase.assertFalse
import junit.framework.TestCase.assertTrue
import org.junit.Test

class ExampleUnitTest {
    @Test
    fun example_clone_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(translationSet)
        )

        val secondExample = example.clone()
        assertFalse(example === secondExample)
        assertTrue(example == secondExample)
        assertTrue(example.hashCode() == secondExample.hashCode())
    }

    @Test
    fun example_equals_simple() {
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        val secondExample = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertFalse(example === secondExample)
        assertTrue(example == secondExample)

        val thirdExample = example.clone()
        assertFalse(example === thirdExample)
        assertTrue(example == thirdExample)
    }

    @Test
    fun example_equals_false() {
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        val secondExample = Example(
            sourceLanguage = "cmn",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )
        assertFalse(example === secondExample)
        assertFalse(example == secondExample)

        val thirdExample = Example(
            sourceLanguage = "yue",
            traditional = "台山话",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )
        assertFalse(example === thirdExample)
        assertFalse(example == thirdExample)

        val fourthExample = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "臺山話",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )
        assertFalse(example === fourthExample)
        assertFalse(example == fourthExample)

        val fifthExample = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa6",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )
        assertFalse(example === fifthExample)
        assertFalse(example == fifthExample)

        val sixthExample = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua2",
            translations = listOf()
        )
        assertFalse(example === sixthExample)
        assertFalse(example == sixthExample)

        val seventhExample = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf(TranslationSet("", listOf()))
        )
        assertFalse(example === seventhExample)
        assertFalse(example == seventhExample)
    }

    @Test
    fun example_hashCode_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(translationSet)
        )

        val secondTranslation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val secondTranslationSet =
            TranslationSet("粵典—words.hk", translations = listOf(secondTranslation))
        val secondExample = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(secondTranslationSet)
        )

        assertFalse(example === secondExample)
        assertTrue(example.hashCode() == secondExample.hashCode())
    }

    @Test
    fun example_hashCode_notEqual() {
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf()
        )

        val secondExample = Example(
            sourceLanguage = "cmn",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf()
        )
        assertFalse(example === secondExample)
        assertFalse(example.hashCode() == secondExample.hashCode())

        val thirdExample = Example(
            sourceLanguage = "yue",
            traditional = "鸡蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf()
        )
        assertFalse(example === thirdExample)
        assertFalse(example.hashCode() == thirdExample.hashCode())

        val fourthExample = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "雞蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf()
        )
        assertFalse(example === fourthExample)
        assertFalse(example.hashCode() == fourthExample.hashCode())

        val fifthExample = Example(
            sourceLanguage = "yue",
            traditional = "鸡蛋仔",
            simplified = "雞蛋仔",
            jyutping = "gai1 daan6 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf()
        )
        assertFalse(example === fifthExample)
        assertFalse(example.hashCode() == fifthExample.hashCode())

        val sixthExample = Example(
            sourceLanguage = "yue",
            traditional = "鸡蛋仔",
            simplified = "雞蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai5",
            translations = listOf()
        )
        assertFalse(example === sixthExample)
        assertFalse(example.hashCode() == sixthExample.hashCode())

        val seventhExample = Example(
            sourceLanguage = "yue",
            traditional = "鸡蛋仔",
            simplified = "雞蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(TranslationSet("", listOf()))
        )
        assertFalse(example === seventhExample)
        assertFalse(example.hashCode() == seventhExample.hashCode())
    }

    @Test
    fun example_sourceLanguage_simple() {
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals("yue", example.sourceLanguage)

        example.sourceLanguage = "cmn"
        assertEquals("cmn", example.sourceLanguage)
    }

    @Test
    fun example_getCharacters_traditional() {
        val traditional = "臺山話"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(traditional, example.traditional)
        assertEquals(traditional, example.getCharacters(EntryCharactersOptions.ONLY_TRADITIONAL))
        assertEquals(traditional, example.getCharacters(EntryCharactersOptions.PREFER_TRADITIONAL))
    }

    @Test
    fun example_getCharacters_simplified() {
        val simplified = "台山话"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(simplified, example.simplified)
        assertEquals(simplified, example.getCharacters(EntryCharactersOptions.ONLY_SIMPLIFIED))
        assertEquals(simplified, example.getCharacters(EntryCharactersOptions.PREFER_SIMPLIFIED))
    }

    @Test
    fun example_traditional_getSet() {
        val traditional = "臺山話"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(traditional, example.traditional)

        val newTraditional = "四邑話"
        example.traditional = newTraditional
        assertEquals(newTraditional, example.traditional)
        assertEquals(newTraditional, example.getCharacters(EntryCharactersOptions.ONLY_TRADITIONAL))
        assertEquals(
            newTraditional,
            example.getCharacters(EntryCharactersOptions.PREFER_TRADITIONAL)
        )
    }

    @Test
    fun example_simplified_getSet() {
        val simplified = "台山话"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(simplified, example.simplified)

        val newSimplified = "四邑话"
        example.simplified = newSimplified
        assertEquals(newSimplified, example.simplified)
        assertEquals(newSimplified, example.getCharacters(EntryCharactersOptions.ONLY_SIMPLIFIED))
        assertEquals(newSimplified, example.getCharacters(EntryCharactersOptions.PREFER_SIMPLIFIED))
    }

    @Test
    fun example_jyutping_getSet() {
        val jyutping = "toi4 saan1 waa2"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(jyutping, example.jyutping)
        var phoneticString = "tòih sāan wá"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        phoneticString = "tʰɔːi̯˨˩ säːn˥ wäː˧˥"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )

        val newJyutping = "SeI3 jAp1 WaA2"
        val normalizedNewJyutping = "sei3 jap1 waa2"
        example.jyutping = newJyutping
        assertEquals(normalizedNewJyutping, example.jyutping)
        phoneticString = "sei yāp wá"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        phoneticString = "sei̯˧ jɐp̚˥ wäː˧˥"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
    }

    @Test
    fun example_pinyin_getSet() {
        val pinyin = "tai2 shan1 hua4"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(pinyin, example.pinyin)
        var phoneticString = "tái shān huà"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.PRETTY_PINYIN
            )
        )
        phoneticString = "tai2 shan1 hua4"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.NUMBERED_PINYIN
            )
        )
        phoneticString = "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.ZHUYIN
            )
        )
        phoneticString = "tʰaɪ̯˧˥ ʂän˥˥ xwä˥˩"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.MANDARIN_IPA
            )
        )
        phoneticString = "tai2 shan1 hua4"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN_WITH_V
            )
        )

        val newPinyin = "Si4 yi4 HuA4"
        val normalizedNewPinyin = "si4 yi4 hua4"
        example.pinyin = newPinyin
        assertEquals(normalizedNewPinyin, example.pinyin)
        phoneticString = "sì yì huà"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.PRETTY_PINYIN
            )
        )
        phoneticString = "si4 yi4 hua4"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.NUMBERED_PINYIN
            )
        )
        phoneticString = "ㄙˋ ㄧˋ ㄏㄨㄚˋ"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.ZHUYIN
            )
        )
        phoneticString = "sz̩˥˩꜒꜔ i˥˩꜒꜔ xwä˥˩"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.MANDARIN_IPA
            )
        )
        phoneticString = "si4 yi4 hua4"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN_WITH_V
            )
        )
    }

    @Test
    fun example_getPhonetic_cantonese() {
        val jyutping = "toi4 saan1 waa2"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(jyutping, example.jyutping)
        assertEquals(
            jyutping,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        var phoneticString = "tòih sāan wá"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        phoneticString = "tʰɔːi̯˨˩ säːn˥ wäː˧˥"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )

        example.pinyin = ""
        phoneticString = "tòih sāan wá"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        example.jyutping = ""
        example.pinyin = "tai2 shan1 hua4"
        phoneticString = ""
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
    }

    @Test
    fun example_getPhonetic_mandarin() {
        val pinyin = "tai2 shan1 hua4"
        val example = Example(
            sourceLanguage = "yue",
            traditional = "臺山話",
            simplified = "台山话",
            jyutping = "toi4 saan1 waa2",
            pinyin = "tai2 shan1 hua4",
            translations = listOf()
        )

        assertEquals(pinyin, example.pinyin)
        var phoneticString = "tái shān huà"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.PRETTY_PINYIN
            )
        )
        phoneticString = "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.ZHUYIN
            )
        )
        phoneticString = "tʰaɪ̯˧˥ ʂän˥˥ xwä˥˩"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.MANDARIN_IPA
            )
        )

        example.jyutping = ""
        phoneticString = "tai2 shan1 hua4"
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        example.pinyin = ""
        example.jyutping = "toi4 saan1 waa2"
        phoneticString = ""
        assertEquals(
            phoneticString,
            example.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.MANDARIN_IPA
            )
        )
    }

    @Test
    fun example_getTranslationSets_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(translationSet)
        )

        assertEquals(listOf(translationSet), example.getTranslationSets())
    }

    @Test
    fun example_modifyDefinitionsSets_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))
        val translationSetList = listOf(translationSet)
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = translationSetList
        )

        assertTrue(example.getTranslationSets() == translationSetList)
        assertFalse(example.getTranslationSets() === translationSetList)
    }

    @Test
    fun example_getTranslationSnippet_simple() {
        val translation = Translation(
            content = "egg waffle",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(translationSet)
        )

        assertEquals("egg waffle", example.getTranslationSnippet())
    }

    @Test
    fun example_getTranslationSnippet_emptyTranslations() {
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf()
        )

        assertEquals("", example.getTranslationSnippet())
    }

    @Test
    fun example_getTranslationSnippet_emptyTranslationSet() {
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf())
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(translationSet)
        )

        assertEquals("", example.getTranslationSnippet())
    }

    @Test
    fun example_getTranslationSnippet_emptyTranslationContent() {
        val translation = Translation(
            content = "",
            language = "eng",
            directTarget = true
        )
        val translationSet = TranslationSet("粵典—words.hk", translations = listOf(translation))
        val example = Example(
            sourceLanguage = "yue",
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            translations = listOf(translationSet)
        )

        assertEquals("", example.getTranslationSnippet())
    }
}