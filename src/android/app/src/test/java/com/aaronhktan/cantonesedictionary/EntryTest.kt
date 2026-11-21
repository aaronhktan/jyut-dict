@file:Suppress("SpellCheckingInspection")

package com.aaronhktan.cantonesedictionary

import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.withStyle
import com.aaronhktan.cantonesedictionary.data.datastore.defaultJyutpingToneColours
import com.aaronhktan.cantonesedictionary.data.datastore.defaultPinyinToneColours
import com.aaronhktan.cantonesedictionary.logic.entry.CantoneseOptions
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.entry.EntryCharactersOptions
import com.aaronhktan.cantonesedictionary.logic.entry.EntryColourPhoneticType
import com.aaronhktan.cantonesedictionary.logic.entry.EntryPhoneticOptions
import com.aaronhktan.cantonesedictionary.logic.entry.MandarinOptions
import com.aaronhktan.cantonesedictionary.logic.utils.compareStrings
import org.junit.Assert.assertEquals
import org.junit.Test

class EntryUnitTest {
    @Test
    fun entry_getCharacters_simple() {
        val traditional = "清遠"
        val simplified = "清远"
        val entry = Entry(
            rowId = 1,
            traditional = traditional,
            simplified = simplified,
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(
            AnnotatedString(traditional), entry.getCharacters(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified), entry.getCharacters(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$traditional [${compareStrings(traditional, simplified)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$simplified [${compareStrings(simplified, traditional)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
    }

    @Test
    fun entry_traditional_getSet() {
        val traditional = "清遠"
        val simplified = "清远"
        val entry = Entry(
            rowId = 1,
            traditional = traditional,
            simplified = simplified,
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        val annotatedTraditional = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[1])) {
                append("清")
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[5])) {
                append("遠")
            }
        }

        assertEquals(traditional, entry.traditional)
        assertEquals(
            annotatedTraditional, entry.getCharacters(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )

        val newTraditional = "廣州"
        entry.traditional = newTraditional
        entry.jyutping = "gwong2 zau1"
        val annotatedNewTraditional = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[2])) {
                append("廣")
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[1])) {
                append("州")
            }
        }
        assertEquals(newTraditional, entry.traditional)
        assertEquals(
            annotatedNewTraditional, entry.getCharacters(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
    }

    @Test
    fun entry_simplified_getSet() {
        val traditional = "清遠"
        val simplified = "清远"
        val entry = Entry(
            rowId = 1,
            traditional = traditional,
            simplified = simplified,
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        val annotatedSimplified = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[1])) {
                append("清")
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[5])) {
                append("远")
            }
        }

        assertEquals(simplified, entry.simplified)
        assertEquals(
            annotatedSimplified, entry.getCharacters(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )

        val newSimplified = "广州"
        entry.simplified = newSimplified
        entry.jyutping = "gwong2 zau1"
        val annotatedNewSimplified = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[2])) {
                append("广")
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[1])) {
                append("州")
            }
        }
        assertEquals(newSimplified, entry.simplified)
        assertEquals(
            annotatedNewSimplified, entry.getCharacters(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
    }

    @Test
    fun entry_jyutping_getSet() {
        val jyutping = "cing1 jyun5"
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = jyutping,
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(jyutping, entry.jyutping)
        assertEquals(
            jyutping, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        val yale = "chīng yúhn"
        assertEquals(
            yale, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        val cantoneseIPA = "t͡sʰɪŋ˥ jyːn˩˧"
        assertEquals(
            cantoneseIPA, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )

        val newSimplified = "广州"
        entry.simplified = newSimplified
        val newJyutping = "gWoNg2 ZaU1"
        val normalizedJyutping = "gwong2 zau1"
        entry.jyutping = newJyutping
        assertEquals(normalizedJyutping, entry.jyutping)
        assertEquals(
            normalizedJyutping, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        val newYale = "gwóng jāu"
        assertEquals(
            newYale, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        val newCantoneseIPA = "kʷɔːŋ˧˥ t͡sɐu̯˥"
        assertEquals(
            newCantoneseIPA, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
    }

    @Test
    fun entry_pinyin_getSet() {
        val pinyin = "qing1 yuan3"
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = pinyin,
            definitions = listOf()
        )

        assertEquals(pinyin, entry.pinyin)
        assertEquals(
            pinyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        val prettyPinyin = "qīng yuǎn"
        assertEquals(
            prettyPinyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.PRETTY_PINYIN
            )
        )
        val numberedPinyin = "qing1 yuan3"
        assertEquals(
            numberedPinyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.NUMBERED_PINYIN
            )
        )
        val zhuyin = "ㄑㄧㄥ ㄩㄢˇ"
        assertEquals(
            zhuyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.ZHUYIN
            )
        )
        val mandarinIPA = "t͡ɕʰiŋ˥˥ ɥɛn˨˩˦꜕꜖(꜓)"
        assertEquals(
            mandarinIPA, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.MANDARIN_IPA
            )
        )
        val rawPinyinWithV = "qing1 yuan3"
        assertEquals(
            rawPinyinWithV, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN_WITH_V
            )
        )

        val newSimplified = "广州"
        entry.simplified = newSimplified
        val newPinyin = "gUaNg3 ZhOu1"
        val normalizedPinyin = "guang3 zhou1"
        entry.pinyin = newPinyin
        assertEquals(normalizedPinyin, entry.pinyin)
        assertEquals(
            normalizedPinyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        val newPrettyPinyin = "guǎng zhōu"
        assertEquals(
            newPrettyPinyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.PRETTY_PINYIN
            )
        )
        val newNumberedPinyin = "guang3 zhou1"
        assertEquals(
            newNumberedPinyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.NUMBERED_PINYIN
            )
        )
        val newZhuyin = "ㄍㄨㄤˇ ㄓㄡ"
        assertEquals(
            newZhuyin, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.ZHUYIN
            )
        )
        val newMandarinIPA = "kwɑŋ˨˩˦꜕꜖꜖ ʈ͡ʂoʊ̯˥˥"
        assertEquals(
            newMandarinIPA, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.MANDARIN_IPA
            )
        )
        val newRawPinyinWithV = "guang3 zhou1"
        assertEquals(
            newRawPinyinWithV, entry.getPhonetic(
                options = EntryPhoneticOptions.ONLY_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN_WITH_V
            )
        )
    }
}