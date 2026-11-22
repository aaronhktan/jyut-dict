@file:Suppress("SpellCheckingInspection")

package com.aaronhktan.cantonesedictionary

import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.withStyle
import com.aaronhktan.cantonesedictionary.data.datastore.defaultJyutpingToneColours
import com.aaronhktan.cantonesedictionary.data.datastore.defaultPinyinToneColours
import com.aaronhktan.cantonesedictionary.logic.entry.CantoneseOptions
import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.entry.EntryCharactersOptions
import com.aaronhktan.cantonesedictionary.logic.entry.EntryColourPhoneticType
import com.aaronhktan.cantonesedictionary.logic.entry.EntryPhoneticOptions
import com.aaronhktan.cantonesedictionary.logic.entry.MandarinOptions
import com.aaronhktan.cantonesedictionary.logic.utils.SAME_CHARACTER_STRING
import com.aaronhktan.cantonesedictionary.logic.utils.compareStrings
import org.junit.Assert.assertEquals
import org.junit.Test

class EntryUnitTest {
    @Test
    fun entry_getCharacters_simpleCantonese() {
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
    fun entry_getCharacters_simpleMandarin() {
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
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified), entry.getCharacters(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$traditional [${compareStrings(traditional, simplified)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$simplified [${compareStrings(simplified, traditional)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
    }

    @Test
    fun entry_getCharacters_colourCantonese() {
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
        assertEquals(
            annotatedTraditional, entry.getCharacters(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        val annotatedSimplified = buildAnnotatedString {
            withStyle(SpanStyle(color = defaultJyutpingToneColours[1])) {
                append("清")
            }
            withStyle(SpanStyle(color = defaultJyutpingToneColours[5])) {
                append("远")
            }
        }
        assertEquals(
            annotatedSimplified, entry.getCharacters(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        val annotatedPreferTraditional = buildAnnotatedString {
            append(annotatedTraditional)
            append(" [")
            append(SAME_CHARACTER_STRING)
            withStyle(SpanStyle(color = defaultJyutpingToneColours[5])) {
                append("远")
            }
            append("]")
        }
        assertEquals(
            annotatedPreferTraditional,
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        val annotatedPreferSimplified = buildAnnotatedString {
            append(annotatedSimplified)
            append(" [")
            append(SAME_CHARACTER_STRING)
            withStyle(SpanStyle(color = defaultJyutpingToneColours[5])) {
                append("遠")
            }
            append("]")
        }
        assertEquals(
            annotatedPreferSimplified,
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            annotatedTraditional, entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            annotatedTraditional, entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            annotatedSimplified,
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            annotatedSimplified,
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
    }

    @Test
    fun entry_getCharacters_colourMandarin() {
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
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified), entry.getCharacters(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$traditional [${compareStrings(traditional, simplified)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$simplified [${compareStrings(simplified, traditional)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = false,
                colourPhoneticType = EntryColourPhoneticType.MANDARIN,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
    }

    @Test
    fun entry_getCharacters_simpleNone() {
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
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified), entry.getCharacters(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$traditional [${compareStrings(traditional, simplified)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString("$simplified [${compareStrings(simplified, traditional)}]"),
            entry.getCharacters(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(traditional), entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.ONLY_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours
            )
        )
        assertEquals(
            AnnotatedString(simplified),
            entry.getCharactersNoSecondary(
                options = EntryCharactersOptions.PREFER_SIMPLIFIED,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.NONE,
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

    @Test
    fun entry_getPhonetic_cantonese() {
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
        var phoneticString = "chīng yúhn (qing1 yuan3)"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        phoneticString = "t͡sʰɪŋ˥ jyːn˩˧ (qing1 yuan3)"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )

        entry.pinyin = ""
        phoneticString = "chīng yúhn"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.PRETTY_YALE,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
        entry.jyutping = ""
        entry.pinyin = "qing1 yuan3"
        phoneticString = "(qing1 yuan3)"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.CANTONESE_IPA,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
    }

    @Test
    fun entry_getPhonetic_mandarin() {
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
        var phoneticString = "qīng yuǎn (cing1 jyun5)"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.PRETTY_PINYIN
            )
        )
        phoneticString = "ㄑㄧㄥ ㄩㄢˇ (cing1 jyun5)"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.ZHUYIN
            )
        )
        phoneticString = "t͡ɕʰiŋ˥˥ ɥɛn˨˩˦꜕꜖(꜓) (cing1 jyun5)"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.MANDARIN_IPA
            )
        )

        entry.jyutping = ""
        phoneticString = "qing1 yuan3"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.NUMBERED_PINYIN
            )
        )
        entry.pinyin = ""
        entry.jyutping = "cing1 jyun5"
        phoneticString = "(cing1 jyun5)"
        assertEquals(
            phoneticString, entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_MANDARIN,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.RAW_PINYIN
            )
        )
    }

    @Test
    fun entry_getJyutpingNumbers_simple() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(
            listOf(1, 5), entry.getJyutpingNumbers()
        )
    }

    @Test
    fun entry_getJyutpingNumbers_invalidTones() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 cing7 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(
            listOf(1, 5), entry.getJyutpingNumbers()
        )
    }

    @Test
    fun entry_getJyutpingNumbers_resetJyutping() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(
            listOf(1, 5), entry.getJyutpingNumbers()
        )

        entry.jyutping = "gwong2 zau1"
        assertEquals(
            listOf(2, 1), entry.getJyutpingNumbers()
        )
    }

    @Test
    fun entry_getPinyinNumbers_simple() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(
            listOf(1, 3), entry.getPinyinNumbers()
        )
    }

    @Test
    fun entry_getPinyinNumbers_invalidTones() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 qing6 yuan3",
            definitions = listOf()
        )

        assertEquals(
            listOf(1, 3), entry.getPinyinNumbers()
        )
    }

    @Test
    fun entry_getPinyinNumbers_resetPinyin() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(
            listOf(1, 3), entry.getPinyinNumbers()
        )

        entry.pinyin = "guang3 zhou1"
        assertEquals(
            listOf(3, 1), entry.getPinyinNumbers()
        )
    }

    @Test
    fun entry_getDefinitionsSets_simple() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )

        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf(
                definitionsSet
            )
        )

        assertEquals(listOf(definitionsSet), entry.getDefinitionsSets())
    }

    @Test
    fun entry_modifyDefinitionsSets_simple() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )

        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf(
                definitionsSet
            )
        )
        assertEquals(listOf(definitionsSet), entry.getDefinitionsSets())

        val oldDefinitionsSet = definitionsSet.clone()
        val secondDefinition = Definition(
            "(historical) Qingyuan (a former county of Guangdong, China)",
            label = "noun",
            examples = listOf()
        )
        definitionsSet.appendDefinition(secondDefinition)
        // DefinitionsSet should be copied into entry, so modifications outside should not affect it
        assertEquals(listOf(oldDefinitionsSet), entry.getDefinitionsSets())

        val set = entry.getDefinitionsSets()?.get(0)
        set?.appendDefinition(secondDefinition)
        // The sets returned by getDefinitionsSets should be a copy, not a reference to the internal sets
        assertEquals(listOf(oldDefinitionsSet), entry.getDefinitionsSets())
    }

    @Test
    fun entry_getDefinitionSnippet_simple() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )

        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf(
                definitionsSet
            )
        )

        assertEquals("Qingyuan, prefecture-level city in Guangdong", entry.getDefinitionSnippet())
    }

    @Test
    fun entry_getDefinitionSnippet_emptyDefinitionsSet() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals("", entry.getDefinitionSnippet())
    }

    @Test
    fun entry_getDefinitionSnippet_emptyDefinition() {
        val definition = Definition(
            "",
            label = "",
            examples = listOf()
        )

        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf(definitionsSet)
        )

        assertEquals("", entry.getDefinitionSnippet())
    }

    @Test
    fun entry_getId_simple() {
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf()
        )

        assertEquals(1, entry.id)
    }
}