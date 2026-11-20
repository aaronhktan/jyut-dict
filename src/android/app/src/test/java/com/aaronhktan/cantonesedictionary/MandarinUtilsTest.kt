@file:Suppress("SpellCheckingInspection")

package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.utils.convertPinyinToIPA
import com.aaronhktan.cantonesedictionary.logic.utils.convertPinyinToZhuyin
import com.aaronhktan.cantonesedictionary.logic.utils.createNumberedPinyin
import com.aaronhktan.cantonesedictionary.logic.utils.createPinyinWithV
import com.aaronhktan.cantonesedictionary.logic.utils.createPrettyPinyin
import com.aaronhktan.cantonesedictionary.logic.utils.pinyinSoundChanges
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
    fun mandarinUtils_pinyinToIPA_simple() {
        assertEquals("pä˥˥ tä˧˥ tʰʊŋ˥˥", convertPinyinToIPA("ba1 da2 tong1"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_empty() {
        assertEquals("", convertPinyinToIPA(""))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_rejectNoTone() {
        assertEquals("ba", convertPinyinToIPA("ba"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_rejectSingleLetter() {
        assertEquals("a", convertPinyinToIPA("a"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_rejectSpecialCharacter() {
        assertEquals("-", convertPinyinToIPA("-"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_noSpaces() {
        assertEquals("pä˥˥ tä˧˥ tʰʊŋ˥˥", convertPinyinToIPA("ba1da2tong1"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_spacesToSegment() {
        assertEquals(
            "pä˥˥ tä˧˥ tʰʊŋ˥˥",
            convertPinyinToIPA("ba1 da2 tong1", useSpacesToSegment = true)
        )
    }

    @Test
    fun mandarinUtils_pinyinToIPA_specialCaseNg() {
        assertEquals("ŋ̍", convertPinyinToIPA("ng5"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_specialCaseRi() {
        assertEquals("ʐ̩˥˩", convertPinyinToIPA("ri4"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_specialCaseV() {
        assertEquals("ny˨˩˦", convertPinyinToIPA("nv3"))
        assertEquals("t͡ɕʰy˥˩", convertPinyinToIPA("qu4"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_voicelessInitial() {
        assertEquals("g̊ə", convertPinyinToIPA("ge5"))
        assertEquals("i˥˥ g̊ə˨", convertPinyinToIPA("yi1 ge5"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_invalidInitialAndFinal() {
        assertEquals("wz˥˥", convertPinyinToIPA("wz1"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_toneThree() {
        assertEquals("kʰɤ˨˩˦", convertPinyinToIPA("ke3"))
        assertEquals("kʰɤ˨˩˦꜔꜒ i˨˩˦꜕꜖(꜓)", convertPinyinToIPA("ke3 yi3"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_toneFour() {
        assertEquals("ɕjä˥˩꜒꜔ t͡ɕʰy˥˩", convertPinyinToIPA("xia4 qu4"))
        assertEquals("ɕjä˥˩", convertPinyinToIPA("xia4"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_otherTone() {
        assertEquals("mä˥˥", convertPinyinToIPA("ma1"))
        assertEquals("mä˧˥", convertPinyinToIPA("ma2"))
        assertEquals("mä", convertPinyinToIPA("ma5"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_onlyTone() {
        assertEquals("66", convertPinyinToIPA("66", useSpacesToSegment = true))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_erhua() {
        assertEquals("xu̯ɑɻ˥˥", convertPinyinToIPA("huar1"))
        assertEquals("t͡ɕʰɥɑɻ˥˥", convertPinyinToIPA("quanr1"))

        assertEquals("ʂʐ̩˧˥", convertPinyinToIPA("shi2"))
        assertEquals("ʂʐ̩ɻ˧˥", convertPinyinToIPA("shir2"))

        assertEquals("t͡sʰz̩˧˥", convertPinyinToIPA("ci2"))
        assertEquals("t͡sʰz̩ɻ˧˥", convertPinyinToIPA("cir2"))
    }

    @Test
    fun mandarinUtils_pinyinToIPA_glottal() {
        assertEquals("ˀɤ˧˥", convertPinyinToIPA("e2"))
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

    @Test
    fun mandarinUtils_pinyinSoundChanges_z() {
        assertEquals(listOf("z(h)!uang!"), pinyinSoundChanges(listOf("zuan")))
        assertEquals(listOf("z(h)!uang!3"), pinyinSoundChanges(listOf("zuan3")))
        assertEquals(listOf("z(h)!uang!?"), pinyinSoundChanges(listOf("zuan?")))

        assertEquals(listOf("z(h)!uang!"), pinyinSoundChanges(listOf("zhuan")))
        assertEquals(listOf("z(h)!uang!3"), pinyinSoundChanges(listOf("zhuan3")))
        assertEquals(listOf("z(h)!uang!?"), pinyinSoundChanges(listOf("zhuan?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_c() {
        assertEquals(listOf("c(h)!ong"), pinyinSoundChanges(listOf("cong")))
        assertEquals(listOf("c(h)!ong1"), pinyinSoundChanges(listOf("cong1")))
        assertEquals(listOf("c(h)!ong?"), pinyinSoundChanges(listOf("cong?")))

        assertEquals(listOf("c(h)!ong"), pinyinSoundChanges(listOf("chong")))
        assertEquals(listOf("c(h)!ong1"), pinyinSoundChanges(listOf("chong1")))
        assertEquals(listOf("c(h)!ong?"), pinyinSoundChanges(listOf("chong?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_s() {
        assertEquals(listOf("s(h)!e"), pinyinSoundChanges(listOf("se")))
        assertEquals(listOf("s(h)!e2"), pinyinSoundChanges(listOf("se2")))
        assertEquals(listOf("s(h)!e?"), pinyinSoundChanges(listOf("se?")))

        assertEquals(listOf("s(h)!e"), pinyinSoundChanges(listOf("she")))
        assertEquals(listOf("s(h)!e2"), pinyinSoundChanges(listOf("she2")))
        assertEquals(listOf("s(h)!e?"), pinyinSoundChanges(listOf("she?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_n() {
        assertEquals(listOf("(n|l)i"), pinyinSoundChanges(listOf("ni")))
        assertEquals(listOf("(n|l)i3"), pinyinSoundChanges(listOf("ni3")))
        assertEquals(listOf("(n|l)i?"), pinyinSoundChanges(listOf("ni?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_r() {
        assertEquals(listOf("(l|r)e"), pinyinSoundChanges(listOf("re")))
        assertEquals(listOf("(l|r)e4"), pinyinSoundChanges(listOf("re4")))
        assertEquals(listOf("(l|r)e?"), pinyinSoundChanges(listOf("re?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_ln() {
        assertEquals(listOf("(l|n)i"), pinyinSoundChanges(listOf("li")))
        assertEquals(listOf("(l|n)ie4"), pinyinSoundChanges(listOf("lie4")))
        assertEquals(listOf("(l|n)iao?"), pinyinSoundChanges(listOf("liao?")))
        assertEquals(listOf("(l|n)iu"), pinyinSoundChanges(listOf("liu")))
        assertEquals(listOf("(l|n)iang!2"), pinyinSoundChanges(listOf("lian2")))
        assertEquals(listOf("(l|n)ing!?"), pinyinSoundChanges(listOf("lin?")))
        assertEquals(listOf("(l|n)iang!"), pinyinSoundChanges(listOf("liang")))
        assertEquals(listOf("(l|n)ing!1"), pinyinSoundChanges(listOf("ling1")))
        assertEquals(listOf("(l|n)u:?"), pinyinSoundChanges(listOf("lu:?")))
        assertEquals(listOf("(l|n)u:e"), pinyinSoundChanges(listOf("lu:e")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_lnr() {
        assertEquals(listOf("(l|n|r)ang!"), pinyinSoundChanges(listOf("lang")))
        assertEquals(listOf("(l|n|r)ang!4"), pinyinSoundChanges(listOf("lang4")))
        assertEquals(listOf("(l|n|r)ang!?"), pinyinSoundChanges(listOf("lang?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_ang() {
        assertEquals(listOf("bang!"), pinyinSoundChanges(listOf("bang")))
        assertEquals(listOf("bang!1"), pinyinSoundChanges(listOf("bang1")))
        assertEquals(listOf("bang!?"), pinyinSoundChanges(listOf("bang?")))
        assertEquals(listOf("bang!"), pinyinSoundChanges(listOf("ban")))
        assertEquals(listOf("bang!1"), pinyinSoundChanges(listOf("ban1")))
        assertEquals(listOf("bang!?"), pinyinSoundChanges(listOf("ban?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_eng() {
        assertEquals(listOf("peng!"), pinyinSoundChanges(listOf("peng")))
        assertEquals(listOf("peng!4"), pinyinSoundChanges(listOf("peng4")))
        assertEquals(listOf("peng!?"), pinyinSoundChanges(listOf("peng?")))
        assertEquals(listOf("peng!"), pinyinSoundChanges(listOf("pen")))
        assertEquals(listOf("peng!1"), pinyinSoundChanges(listOf("pen1")))
        assertEquals(listOf("peng!?"), pinyinSoundChanges(listOf("pen?")))
    }

    @Test
    fun mandarinUtils_pinyinSoundChanges_ing() {
        assertEquals(listOf("bing!"), pinyinSoundChanges(listOf("bing")))
        assertEquals(listOf("bing!3"), pinyinSoundChanges(listOf("bing3")))
        assertEquals(listOf("bing!?"), pinyinSoundChanges(listOf("bing?")))
        assertEquals(listOf("ping!"), pinyinSoundChanges(listOf("pin")))
        assertEquals(listOf("ping!1"), pinyinSoundChanges(listOf("pin1")))
        assertEquals(listOf("ping!?"), pinyinSoundChanges(listOf("pin?")))
    }
}