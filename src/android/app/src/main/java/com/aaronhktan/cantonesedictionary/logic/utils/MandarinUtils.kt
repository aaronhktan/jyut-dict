package com.aaronhktan.cantonesedictionary.logic.utils

import android.util.Log
import kotlin.math.min

private const val LOG_TAG = "MandarinUtils"

private val pinyinInitials: Set<String> = setOf(
    "b", "p", "m", "f", "d", "t",
    "n", "l", "g", "k", "h", "j",
    "q", "x", "zh", "ch", "sh", "r",
    "z", "c", "s", "y",
)

@Suppress("SpellCheckingInspection")
private val pinyinFinals: Set<String> = setOf(
    "a", "e", "ai", "ei", "ao", "ou", "an", "ang", "en",
    "eng", "ong", "er", "i", "ia", "ie", "iao", "iu",
    "ian", "in", "iang", "ing", "iong", "ua", "uo", "uai",
    "ui", "uan", "un", "uang", "u", "u:", "ue", "u:e", "o",
)

private val pinyinToneReplacements: Map<String, List<String>> = mapOf(
    "a" to listOf("ā", "á", "ǎ", "à", "a"),
    "e" to listOf("ē", "é", "ě", "è", "e"),
    "i" to listOf("ī", "í", "ǐ", "ì", "i"),
    "o" to listOf("ō", "ó", "ǒ", "ò", "o"),
    "u" to listOf("ū", "ú", "ǔ", "ù", "u"),
    "ü" to listOf("ǖ", "ǘ", "ǚ", "ǜ", "ü"),
)

private val pinyinVPrecederRegex = Regex("([jqx])u")

private val zhuyinInitials: Map<String, String> = mapOf(
    "b" to "ㄅ", "p" to "ㄆ", "m" to "ㄇ", "f" to "ㄈ", "d" to "ㄉ",
    "t" to "ㄊ", "n" to "ㄋ", "l" to "ㄌ", "g" to "ㄍ", "k" to "ㄎ",
    "h" to "ㄏ", "j" to "ㄐ", "q" to "ㄑ", "x" to "ㄒ", "z" to "ㄗ",
    "c" to "ㄘ", "s" to "ㄙ", "r" to "ㄖ", "zh" to "ㄓ", "ch" to "ㄔ",
    "sh" to "ㄕ",
)

@Suppress("SpellCheckingInspection")
private val zhuyinFinals: Map<String, String> = mapOf(
    "yuan" to "ㄩㄢ", "iang" to "ㄧㄤ", "yang" to "ㄧㄤ", "uang" to "ㄨㄤ",
    "wang" to "ㄨㄤ", "ying" to "ㄧㄥ", "weng" to "ㄨㄥ", "iong" to "ㄩㄥ",
    "yong" to "ㄩㄥ", "uai" to "ㄨㄞ", "wai" to "ㄨㄞ", "yai" to "ㄧㄞ",
    "iao" to "ㄧㄠ", "yao" to "ㄧㄠ", "ian" to "ㄧㄢ", "yan" to "ㄧㄢ",
    "uan" to "ㄨㄢ", "wan" to "ㄨㄢ", "van" to "ㄩㄢ", "ang" to "ㄤ",
    "yue" to "ㄩㄝ", "wei" to "ㄨㄟ", "you" to "ㄧㄡ", "yin" to "ㄧㄣ",
    "wen" to "ㄨㄣ", "yun" to "ㄩㄣ", "eng" to "ㄥ", "ing" to "ㄧㄥ",
    "ong" to "ㄨㄥ", "io" to "ㄧㄛ", "yo" to "ㄧㄛ", "ia" to "ㄧㄚ",
    "ya" to "ㄧㄚ", "ua" to "ㄨㄚ", "wa" to "ㄨㄚ", "ai" to "ㄞ",
    "ao" to "ㄠ", "an" to "ㄢ", "ie" to "ㄧㄝ", "ye" to "ㄧㄝ",
    "uo" to "ㄨㄛ", "wo" to "ㄨㄛ", "ue" to "ㄩㄝ", "ve" to "ㄩㄝ",
    "ei" to "ㄟ", "ui" to "ㄨㄟ", "ou" to "ㄡ", "iu" to "ㄧㄡ",
    "en" to "ㄣ", "in" to "ㄧㄣ", "un" to "ㄨㄣ", "vn" to "ㄩㄣ",
    "yi" to "ㄧ", "wu" to "ㄨ", "yu" to "ㄩ", "a" to "ㄚ",
    "e" to "ㄜ", "o" to "ㄛ", "i" to "ㄧ", "u" to "ㄨ",
    "v" to "ㄩ", "ê" to "ㄝ",
)
private val zhuyinTones: List<String> = listOf("", "", "ˊ", "ˇ", "ˋ", "˙")

private val zhuyinIPrecederRegex = Regex("([zcs]h?)i")

private val zhuyinRRegex = Regex("(r)i")
private val zhuyinNgSpecialCaseRegex = Regex("^ng([012345])$")
private val zhuyinHmSpecialCaseRegex = Regex("^hm([012345])$")
private val zhuyinHngSpecialCaseRegex = Regex("^hng([012345])$")
private val zhuyinErSpecialCaseRegex = Regex("^er([012345])$")
private val zhuyinInitialRegex = Regex("^([bpmfdtnlgkhjqxzcsr]?h?)")
private val zhuyinFinalRegex = Regex("([aeiouêvyw]?[aeioun]?[aeioung]?[ng]?)(r?)([012345])$")

private val mandarinIPAGlottal: Set<String> = setOf(
    "a", "o", "e", "ai", "ei", "ao", "ou", "an", "en", "er", "ang", "ong", "eng",
)

private val mandarinIPAInitials: Map<String, String> = mapOf(
    "b" to "p", "c" to "t͡sʰ", "ch" to "ʈ͡ʂʰ", "d" to "t", "f" to "f",
    "g" to "k", "h" to "x", "j" to "t͡ɕ", "k" to "kʰ", "l" to "l",
    "m" to "m", "n" to "n", "ng" to "ŋ", "p" to "pʰ", "q" to "t͡ɕʰ",
    "r" to "ʐ", "s" to "s", "sh" to "ʂ", "t" to "tʰ", "x" to "ɕ",
    "z" to "t͡s", "zh" to "ʈ͡ʂ",
)

@Suppress("SpellCheckingInspection")
private val mandarinIPAFinals: Map<String, String> = mapOf(
    "a" to "ä", "ai" to "aɪ̯", "air" to "ɑɻ", "an" to "än",
    "ang" to "ɑŋ", "angr" to "ɑ̃ɻ", "anr" to "ɑɻ", "ao" to "ɑʊ̯",
    "aor" to "aʊ̯ɻʷ", "ar" to "ɑɻ", "e" to "ɤ", "ei" to "eɪ̯",
    "eir" to "əɻ", "en" to "ən", "eng" to "ɤŋ", "engr" to "ɤ̃ɻ",
    "enr" to "əɻ", "er" to "ɤɻ", "i" to "i", "ia" to "jä",
    "ian" to "jɛn", "iang" to "jɑŋ", "iangr" to "jɑ̃ɻ", "ianr" to "jɑɻ",
    "iao" to "jɑʊ̯", "iaor" to "jaʊ̯ɻʷ", "iar" to "jɑɻ", "ie" to "jɛ",
    "ier" to "jɛɻ", "in" to "in", "ing" to "iŋ", "ingr" to "iɤ̯̃ɻ",
    "inr" to "iə̯ɻ", "io" to "jɔ", "iong" to "jʊŋ", "iongr" to "jʊ̃ɻ",
    "ir" to "iə̯ɻ", "iu" to "joʊ̯", "iur" to "jɤʊ̯ɻʷ", "m" to "m̩",
    "n" to "n̩", "ng" to "ŋ̍", "o" to "wɔ", "ong" to "ʊŋ",
    "ongr" to "ʊ̃ɻ", "or" to "wɔɻ", "ou" to "oʊ̯", "our" to "ɤʊ̯ɻʷ",
    "u" to "u", "ua" to "u̯ä", "uai" to "waɪ̯", "uair" to "wɑɻ",
    "uan" to "wän", "uang" to "wɑŋ", "uangr" to "wɑ̃ɻ", "uanr" to "wɑɻ",
    "uar" to "u̯ɑɻ", "ue" to "ɥɛ", "ui" to "weɪ̯", "uir" to "wəɻ",
    "un" to "wən", "unr" to "wəɻ", "uo" to "wɔ", "uor" to "wɔɻ",
    "ur" to "uɻʷ", "v" to "y", "van" to "ɥɛn", "vanr" to "ɥɑɻ",
    "ve" to "ɥɛ", "ver" to "ɥɛɻ", "vn" to "yn", "vnr" to "yə̯ɻ",
    "vr" to "yə̯ɻ", "wa" to "wä", "wai" to "waɪ̯", "wair" to "wɑɻ",
    "wan" to "wän", "wang" to "wɑŋ", "wangr" to "wɑ̃ɻ", "wanr" to "wɑɻ",
    "war" to "wɑɻ", "wei" to "weɪ̯", "weir" to "wəɻ", "wen" to "wən",
    "weng" to "wəŋ", "wengr" to "ʊ̃ɻ", "wenr" to "wəɻ", "wo" to "wɔ",
    "wor" to "wɔɻ", "wu" to "u", "wur" to "uɻʷ", "ya" to "jä",
    "yai" to "jaɪ̯", "yan" to "jɛn", "yang" to "jɑŋ", "yangr" to "jɑ̃ɻ",
    "yanr" to "jɑɻ", "yao" to "jɑʊ̯", "yaor" to "jaʊ̯ɻʷ", "yar" to "jɑɻ",
    "ye" to "jɛ", "yer" to "jɛɻ", "yi" to "i", "yin" to "in",
    "ying" to "iŋ", "yingr" to "iɤ̯̃ɻ", "yinr" to "iə̯ɻ", "yir" to "iə̯ɻ",
    "yo" to "jɔ", "yong" to "jʊŋ", "yongr" to "jʊ̃ɻ", "yor" to "jɔɻ",
    "you" to "joʊ̯", "your" to "jɤʊ̯ɻʷ", "yu" to "y", "yuan" to "ɥɛn",
    "yuanr" to "ɥɑɻ", "yue" to "ɥɛ", "yuer" to "ɥɛɻ", "yun" to "yn",
    "yunr" to "yə̯ɻ", "yur" to "yə̯ɻ",
)

@Suppress("SpellCheckingInspection")
private val mandarinIPAVoicelessInitials: Map<String, String> = mapOf(
    "k" to "g̊",
    "p" to "b̥",
    "t" to "d̥",
    "t͡s" to "d͡z̥",
    "t͡ɕ" to "d͡ʑ̥",
    "ʈ͡ʂ" to "ɖ͡ʐ̥",
)
private val mandarinIPANeutralTone: List<String> = listOf("˨", "˧", "˦", "˩", "˩")
private val mandarinIPAThirdTone: List<String> =
    listOf("˨˩˦꜕꜖꜖", "˨˩˦꜕꜖꜖", "˨˩˦꜔꜒", "˨˩˦꜕꜖꜖", "˨˩˦")
private val mandarinIPATones: List<String> = listOf("˥˥", "˧˥", "˨˩˦", "˥˩", "")
private val mandarinIPASyllableRegex = Regex("^([bcdfghjklmnpqrstxz]?h?)(.+)$")

fun createPrettyPinyin(pinyin: String): String {
    if (pinyin.isEmpty()) {
        return pinyin
    }

    val processedSyllables: MutableList<String> = mutableListOf()
    val (_, syllables) = segmentPinyin(pinyin)

    for (s in syllables) {
        var syllable = s

        var toneIdx = syllable.indexOfFirst { it in '0'..'5' }
        if (toneIdx == -1) {
            processedSyllables.add(syllable)
            continue
        }
        var tone = syllable[toneIdx].toString().toInt()
        if (tone !in 1..5) {
            tone = 5
        }

        syllable = syllable.replace("u:", "ü")

        var vowelIdx = syllable.indexOfFirst { setOf('a', 'e', 'o').contains(it) }
        if (vowelIdx == -1) {
            vowelIdx = syllable.indexOfLast { setOf('i', 'u', 'ü').contains(it) }
        }
        if (vowelIdx == -1) {
            processedSyllables.add(syllable)
            continue
        }

        syllable = syllable.replaceRange(
            vowelIdx..<vowelIdx + 1,
            pinyinToneReplacements[syllable[vowelIdx].toString()]?.get(tone - 1) ?: "x"
        )

        toneIdx = syllable.indexOfFirst { it in '0'..'6' }
        syllable = syllable.replaceRange(toneIdx..<toneIdx + 1, "")
        processedSyllables.add(syllable)
    }

    return processedSyllables.joinToString(" ")
}

fun createNumberedPinyin(pinyin: String): String {
    return pinyin.replace("u:", "ü")
}

fun createPinyinWithV(pinyin: String): String {
    return pinyin.replace("u:", "v")
}

@Suppress("SpellCheckingInspection")
fun convertPinyinToZhuyin(pinyin: String, useSpacesToSegment: Boolean = false): String {
    if (pinyin.isEmpty()) {
        return pinyin
    }

    var syllables: MutableList<String>
    var pinyinCopy = ""
    if (useSpacesToSegment) {
        // Insert a space before and after every special character, so that the
        // zhuyin conversion doesn't attempt to convert special characters.
        for (c in specialCharacters) {
            pinyinCopy = pinyin.split(c).joinToString(" $c ")
        }
        syllables = pinyinCopy.split(" ").toMutableList()
    } else {
        val (_, result) = segmentPinyin(
            pinyin,
            removeSpecialCharacters = false,
            removeGlobCharacters = false
        )
        syllables = result.toMutableList()
    }

    val zhuyinSyllables: MutableList<String> = mutableListOf()
    for (syllable in syllables) {
        if (syllable.length == 1 || specialCharacters.contains(syllable)) {
            zhuyinSyllables.add(syllable)
            continue
        }

        val toneIdx = syllable.indexOfFirst { it in '0'..'5' }
        if (toneIdx == -1) {
            zhuyinSyllables.add(syllable)
            continue
        }
        val tone = syllable[toneIdx].toString().toInt()

        var zhuyinSyllable = syllable
        zhuyinSyllable = zhuyinSyllable
            .replace("u:", "v")
            .replace(pinyinVPrecederRegex, "$1v")
            .replace(zhuyinIPrecederRegex, "$1")
            .replace(zhuyinRRegex, "$1")
            .replace(zhuyinNgSpecialCaseRegex, "ㄫ$1$1")
            .replace(zhuyinHmSpecialCaseRegex, "ㄏㄇ$1")
            .replace(zhuyinHngSpecialCaseRegex, "ㄏㄫ$1")
            .replace(zhuyinErSpecialCaseRegex, "ㄦ$1")


        zhuyinInitialRegex.find(zhuyinSyllable)?.let { initialMatch ->
            val initial = initialMatch.destructured.component1()
            if (initial.isNotEmpty() && zhuyinInitials[initial] != null) {
                val initialReplacement = zhuyinInitials[initial]!!
                zhuyinSyllable =
                    zhuyinSyllable.replaceRange(initialMatch.range, initialReplacement)
            }
        }

        zhuyinFinalRegex.find(zhuyinSyllable)?.let { finalMatch ->
            val final = finalMatch.destructured.component1()
            var finalReplacement: String? = null
            if (final.isNotEmpty()) {
                finalReplacement = zhuyinFinals[final]
                if (finalReplacement == null) {
                    Log.e(LOG_TAG, "Pinyin final had no valid zhuyin conversion: $final")
                    zhuyinSyllables.add(syllable)
                    continue
                }
            }
            var er = ""
            if (finalMatch.destructured.component2().isNotEmpty()) {
                er = "ㄦ"
            }
            zhuyinSyllable =
                zhuyinSyllable.replaceRange(finalMatch.range, (finalReplacement ?: "") + er)
        } ?: run {
            Log.e(LOG_TAG, "Invalid pinyin final found in zhuyin conversion: $pinyin")
            return pinyin
        }

        if (tone == 5) {
            zhuyinSyllable = zhuyinTones[tone] + zhuyinSyllable
        } else {
            val erIdx = zhuyinSyllable.indexOf("ㄦ")
            if (erIdx != -1 && zhuyinSyllable != "ㄦ") {
                zhuyinSyllable = "${zhuyinSyllable.substring(0..<erIdx)}${zhuyinTones[tone]}${
                    zhuyinSyllable.substring(
                        erIdx
                    )
                }"
            } else {
                zhuyinSyllable += zhuyinTones[tone]
            }
        }

        zhuyinSyllables.add(zhuyinSyllable)
    }

    return zhuyinSyllables.joinToString(" ")
}

private fun convertIPAMandarinSyllable(syllable: String): Pair<String, String> {
    var ipaInitial = ""
    var ipaFinal = ""

    var ipaInitialMatch = ""
    var ipaFinalMatch = ""
    if (syllable == "ng") {
        ipaFinal = mandarinIPAFinals["ng"] ?: ""
    } else {
        mandarinIPASyllableRegex.find(syllable)?.let { match ->
            ipaInitialMatch = match.destructured.component1()
            ipaFinalMatch = match.destructured.component2()
            ipaInitial = mandarinIPAInitials[ipaInitialMatch] ?: ""
            ipaFinal = mandarinIPAFinals[ipaFinalMatch] ?: ""

            if (ipaInitial.isEmpty() && ipaFinal.isEmpty()) {
                Log.e(LOG_TAG, "Could not find initial or final in $syllable")
                return Pair("", syllable)
            }
        }
    }

    // Replace close front unrounded vowel with syllabic retroflex sibilant
    // fricative (+ voiced retroflex approximant if erhua)
    // in Pinyin starting with ch, sh, zh, or r
    if (ipaInitial == "ʈ͡ʂʰ" || ipaInitial == "ʂ" || ipaInitial == "ʈ͡ʂ"
        || ipaInitial == "ʐ"
    ) {
        if (ipaFinalMatch == "ir") {
            ipaFinal = "ʐ̩ɻ"
        } else if (ipaFinalMatch == "i") {
            ipaFinal = "ʐ̩"
        }
    }

    // Replace close front unrounded vowel with syllabic alveolar sibilant
    // fricative (+ voiced retroflex approximant if erhua)
    // in Pinyin starting with c, s, or z
    if (ipaInitial == "t͡sʰ" || ipaInitial == "s" || ipaInitial == "t͡s") {
        if (ipaFinalMatch == "ir") {
            ipaFinal = "z̩ɻ"
        } else if (ipaFinalMatch == "i") {
            ipaFinal = "z̩"
        }
    }

    // Do some cleanup for Pinyin like "ri"
    if (ipaInitial == "ʐ" && ipaFinal == "ʐ̩") {
        ipaInitial = ""
    }

    return Pair(ipaInitial, ipaFinal)
}

fun convertPinyinToIPA(pinyin: String, useSpacesToSegment: Boolean = false): String {
    if (pinyin.isEmpty()) {
        return pinyin
    }

    val syllables: MutableList<String>
    var pinyinCopy = ""
    if (useSpacesToSegment) {
        // Insert a space before and after every special character, so that the
        // zhuyin conversion doesn't attempt to convert special characters.
        for (c in specialCharacters) {
            pinyinCopy = pinyin.split(c).joinToString(" $c ")
        }
        syllables = pinyinCopy.split(" ").toMutableList()
    } else {
        val (_, result) = segmentPinyin(
            pinyin,
            removeSpecialCharacters = false,
            removeGlobCharacters = false
        )
        syllables = result.toMutableList()
    }

    // Pre-compute list of tones corresponding to each syllable
    // This is used for tone sandhi reasons (3->3 sandhi, x->5 sandhi, etc.)
    val syllableTones: MutableList<Pair<Int, Int>> = mutableListOf()
    for (syllable in syllables) {
        val toneIdx = syllable.indexOfFirst { it in '1'..'5' }
        if (toneIdx == -1) {
            syllableTones.add(Pair(-1, -1))
        } else {
            val tone = syllable[toneIdx].toString().toInt()
            syllableTones.add(Pair(tone, toneIdx))
        }
    }

    val ipaSyllables: MutableList<String> = mutableListOf()
    for (i in syllables.indices) {
        val syllable = syllables[i]
        var ipaGlottal = ""
        var ipaTone: String

        if (syllable.length == 1 || specialCharacters.contains(syllable)) {
            ipaSyllables.add(syllable)
            continue
        }

        val (tone, toneIdx) = syllableTones[i]
        if (toneIdx == -1) {
            ipaSyllables.add(syllable)
            continue
        }
        var tonelessSyllable = syllable.substring(0..<toneIdx)

        // Figure out whether this syllable needs a glottal stop
        if (mandarinIPAGlottal.contains(tonelessSyllable)) {
            ipaGlottal = "ˀ"
        }

        tonelessSyllable = tonelessSyllable
            // Mark close front rounded vowel with v instead of "u" or "u:"
            .replace("u:", "v")
            .replace(pinyinVPrecederRegex, "$1v")

        // Convert initial and final
        var (ipaInitial, ipaFinal) = convertIPAMandarinSyllable(tonelessSyllable)

        // Convert tones
        val nextTone = if (i == syllables.size - 1) -1 else syllableTones[i + 1].component1()
        val previousTone = if (i == 0) -1 else syllableTones[i - 1].component1()

        when (tone) {
            5 -> {
                // When neutral tone, replace some initials with voiceless versions
                ipaInitial = mandarinIPAVoicelessInitials[ipaInitial] ?: ipaInitial
                ipaFinal = if (ipaFinal == "ɤ") "ə" else ipaFinal
                ipaTone = if (previousTone == -1) "" else mandarinIPANeutralTone[previousTone - 1]
            }

            3 -> {
                ipaTone = if (i == syllables.size - 1) {
                    if (i == 0) "˨˩˦" else "˨˩˦꜕꜖(꜓)"
                } else {
                    // If next syllable doesn't have tone, default to no tone sandhi
                    // (which is also what happens when the following tone is tone #5
                    if (nextTone == -1) mandarinIPAThirdTone[4] else mandarinIPAThirdTone[nextTone - 1]
                }
            }

            4 -> {
                ipaTone = if (nextTone == 4) "˥˩꜒꜔" else mandarinIPATones[tone - 1]
            }

            else -> {
                ipaTone = mandarinIPATones[tone - 1]
            }
        }

        ipaSyllables.add("$ipaGlottal$ipaInitial$ipaFinal$ipaTone")
    }

    return ipaSyllables.joinToString(" ")
}

fun segmentPinyin(
    text: String,
    removeSpecialCharacters: Boolean = true,
    removeGlobCharacters: Boolean = true
): Pair<Boolean, List<String>> {
    val processedText = text.lowercase()
    val syllables: MutableList<String> = mutableListOf()

    var validPinyin = true
    var startIdx = 0
    var endIdx = 0
    var initialFound = false

    while (endIdx < processedText.length) {
        var nextIteration = false
        var currentString = processedText[endIdx].toString()
        val isSpecialCharacter = specialCharacters.contains(currentString)
        val isGlobCharacter = currentString.trim() == "*" || currentString.trim() == "?"

        if (currentString == " " || currentString == "'" || isSpecialCharacter || isGlobCharacter) {
            if (initialFound) {
                val previousInitial = processedText.substring(startIdx, endIdx)
                syllables.add(previousInitial)

                if (!pinyinFinals.contains(previousInitial)) {
                    validPinyin = false
                }
                startIdx = endIdx
                initialFound = false
            }
            if (!removeGlobCharacters && isGlobCharacter) {
                if (endIdx > 0 && endIdx > startIdx) {
                    val previousInitial = processedText.substring(startIdx, endIdx)
                    syllables.add(previousInitial)
                    if (!pinyinFinals.contains(previousInitial)) {
                        validPinyin = false
                    }
                    initialFound = false
                }

                var globStartIdx = endIdx
                if (endIdx > 0
                    && processedText[endIdx - 1] == ' '
                    && syllables.isNotEmpty()
                    && syllables.last().last() != ' '
                ) {
                    --globStartIdx
                }
                if (endIdx + 1 < processedText.length
                    && processedText[endIdx + 1] == ' '
                ) {
                    ++endIdx
                }
                syllables.add(processedText.substring(globStartIdx..endIdx))
                startIdx = endIdx
            } else if (!removeSpecialCharacters && isSpecialCharacter) {
                syllables.add(currentString)
            }

            ++startIdx
            ++endIdx
            continue
        }

        // First, check for initials
        // If initial is valid, then extend the end_index for length of initial
        // cluster of consonants.
        for (initialLen in (1..2).reversed()) {
            val offset = min(processedText.length - endIdx, initialLen)
            currentString = processedText.substring(endIdx, endIdx + offset)
            if (pinyinInitials.contains(currentString)) {
                if (initialFound) {
                    validPinyin = false
                }
                endIdx += offset
                nextIteration = true
                initialFound = true
                break
            }
        }

        if (nextIteration) {
            continue
        }

        // Then, check for finals
        // If final is valid, then extend end_index for length of final.
        // Check for number at end of word as well (this represents tone number).
        //
        // Then add the substring from [start_index, end_index) to list
        // and reset start_index, so we can start searching after the end_index.
        for (finalLen in (1..4).reversed()) {
            val offset = min(processedText.length - endIdx, finalLen)
            currentString = processedText.substring(endIdx, endIdx + offset)
            if (pinyinFinals.contains(currentString)) {
                endIdx += offset
                if (endIdx < processedText.length && processedText[endIdx] == 'r') {
                    ++endIdx
                }
                if (endIdx < processedText.length && processedText[endIdx].isDigit()) {
                    val tone = processedText[endIdx].toString().toInt()
                    if (tone !in 1..5) {
                        validPinyin = false
                    }
                    ++endIdx
                }

                syllables.add(processedText.substring(startIdx, endIdx))
                startIdx = endIdx
                nextIteration = true
                initialFound = false
                break
            }
        }

        if (nextIteration) {
            continue
        }

        ++endIdx
    }

    // Then add whatever's left in the search term, minus whitespace.
    val lastSyllable = processedText.substring(startIdx, endIdx).trim()
    if (lastSyllable.isNotEmpty() && lastSyllable != "'") {
        syllables.add(lastSyllable)
        if (!pinyinFinals.contains(lastSyllable)) {
            validPinyin = false
        }
    }

    return Pair(validPinyin, syllables)
}

fun pinyinSoundChanges(text: List<String>): List<String> {
    var syllables: MutableList<String> = mutableListOf()

    for (s in text) {
        var syllable = s
        if (syllable.startsWith("zh")) {
            syllable = "z(h)!${syllable.drop(2)}"
        } else if (syllable.startsWith("z")) {
            syllable = "z(h)!${syllable.drop(1)}"
        }
        if (syllable.startsWith("ch")) {
            syllable = "c(h)!${syllable.drop(2)}"
        } else if (syllable.startsWith("c")) {
            syllable = "c(h)!${syllable.drop(1)}"
        }
        if (syllable.startsWith("sh")) {
            syllable = "s(h)!${syllable.drop(2)}"
        } else if (syllable.startsWith("s")) {
            syllable = "s(h)!${syllable.drop(1)}"
        }
        if (syllable.startsWith("n")) {
            syllable = "(n|l)${syllable.drop(1)}"
        } else if (syllable.startsWith("r")) {
            syllable = "(l|r)${syllable.drop(1)}"
        }
        if (syllable.startsWith("li") || syllable.startsWith("lu:")) {
            syllable = "(l|n)${syllable.drop(1)}"
        } else if (syllable.startsWith("l")) {
            syllable = "(l|n|r)${syllable.drop(1)}"
        }

        if (syllable.endsWith("ang") || syllable.dropLast(1).endsWith("ang")) {
            val idx = syllable.lastIndexOf("ng")
            syllable = syllable.replaceRange(idx..<idx+2, "ng!")
        } else if (syllable.endsWith("an") || syllable.dropLast(1).endsWith("an")) {
            val idx = syllable.lastIndexOf("n")
            syllable = syllable.replaceRange(idx..<idx+1, "ng!")
        }
        if (syllable.endsWith("eng") || syllable.dropLast(1).endsWith("eng")) {
            val idx = syllable.lastIndexOf("ng")
            syllable = syllable.replaceRange(idx..<idx+2, "ng!")
        } else if (syllable.endsWith("en") || syllable.dropLast(1).endsWith("en")) {
            val idx = syllable.lastIndexOf("n")
            syllable = syllable.replaceRange(idx..<idx + 1, "ng!")
        }
        if (syllable.endsWith("ing") || syllable.dropLast(1).endsWith("ing")) {
            val idx = syllable.lastIndexOf("ng")
            syllable = syllable.replaceRange(idx..<idx+2, "ng!")
        } else if (syllable.endsWith("in") || syllable.dropLast(1).endsWith("in")) {
            val idx = syllable.lastIndexOf("n")
            syllable = syllable.replaceRange(idx..<idx+1, "ng!")
        }

        syllables.add(syllable)
    }

    return syllables.toList()
}