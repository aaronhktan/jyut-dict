package com.aaronhktan.cantonesedictionary.logic.entry

import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.AnnotatedString
import com.aaronhktan.cantonesedictionary.logic.utils.applyColours
import com.aaronhktan.cantonesedictionary.logic.utils.compareStrings
import com.aaronhktan.cantonesedictionary.logic.utils.convertJyutpingToIPA
import com.aaronhktan.cantonesedictionary.logic.utils.convertJyutpingToYale
import com.aaronhktan.cantonesedictionary.logic.utils.convertPinyinToIPA
import com.aaronhktan.cantonesedictionary.logic.utils.convertPinyinToZhuyin
import com.aaronhktan.cantonesedictionary.logic.utils.createNumberedPinyin
import com.aaronhktan.cantonesedictionary.logic.utils.createPinyinWithV
import com.aaronhktan.cantonesedictionary.logic.utils.createPrettyPinyin
import java.util.EnumSet
import java.util.Objects

enum class EntryCharactersOptions {
    ONLY_TRADITIONAL, ONLY_SIMPLIFIED, PREFER_SIMPLIFIED, PREFER_TRADITIONAL
}

enum class EntryPhoneticOptions {
    ONLY_CANTONESE, ONLY_MANDARIN, PREFER_CANTONESE, PREFER_MANDARIN
}

enum class EntryColourPhoneticType {
    NONE, CANTONESE, MANDARIN
}

enum class CantoneseOptions {
    RAW_JYUTPING, PRETTY_YALE, CANTONESE_IPA
}

enum class MandarinOptions {
    RAW_PINYIN, PRETTY_PINYIN, NUMBERED_PINYIN, ZHUYIN, MANDARIN_IPA, RAW_PINYIN_WITH_V
}

class Entry {
    private var _traditional: String = ""
    private var _traditionalDifference: String = ""
    private var _preferTraditional: String = ""
    private var _simplified: String = ""
    private var _simplifiedDifference: String = ""
    private var _preferSimplified: String = ""

    private var _colouredTraditional: AnnotatedString? = null
    private var _colouredTraditionalDifference: AnnotatedString? = null
    private var _colouredPreferTraditional: AnnotatedString? = null
    private var _colouredSimplified: AnnotatedString? = null
    private var _colouredSimplifiedDifference: AnnotatedString? = null
    private var _colouredPreferSimplified: AnnotatedString? = null
    private var _cachedCharacterHash: Int? = null

    private var _jyutping: String = jyutping
    private var _yale: String? = null
    private var _cantoneseIPA: String? = null
    private var _jyutpingNumbers: MutableList<Int>? = null

    private var _pinyin: String = pinyin
    private var _prettyPinyin: String? = null
    private var _numberedPinyin: String? = null
    private var _zhuyin: String? = null
    private var _mandarinIPA: String? = null
    private var _pinyinWithV: String? = null
    private var _pinyinNumbers: MutableList<Int>? = null

    private var _definitions: List<DefinitionsSet>? = null
    private var _definitionSnippet: String? = null

    private var _rowId: Int = -1

    constructor(
        rowId: Int,
        traditional: String,
        simplified: String,
        jyutping: String,
        pinyin: String,
        definitions: List<DefinitionsSet>
    ) {
        _rowId = rowId
        _traditional = traditional
        _simplified = simplified
        _jyutping = jyutping.lowercase()
        _pinyin = pinyin.lowercase()
        _definitions = definitions

        _traditionalDifference = compareStrings(simplified, traditional)
        _simplifiedDifference = compareStrings(traditional, simplified)

        _preferTraditional = "$_traditional [$_simplifiedDifference]"
        _preferSimplified = "$_simplified [$_traditionalDifference]"
    }

    var id: Int = _rowId
        private set

    fun getCharacters(
        options: EntryCharactersOptions,
        useColours: Boolean,
        colourPhoneticType: EntryColourPhoneticType,
        jyutpingToneColours: List<Color>,
        pinyinToneColours: List<Color>
    ): AnnotatedString {
        var hash = Objects.hash(
            _traditional,
            _simplified,
            _jyutping,
            _pinyin,
            options,
            useColours,
            colourPhoneticType
        )
        val colours =
            if (colourPhoneticType == EntryColourPhoneticType.MANDARIN) pinyinToneColours else jyutpingToneColours
        for (c in colours) {
            hash = Objects.hash(hash, c)
        }

        if (_cachedCharacterHash == null || _cachedCharacterHash != hash) {
            refreshColours(colourPhoneticType, jyutpingToneColours, pinyinToneColours)
            _cachedCharacterHash = hash
        }

        when (options) {
            EntryCharactersOptions.ONLY_TRADITIONAL -> {
                return if (useColours) _colouredTraditional!! else AnnotatedString(_traditional)
            }

            EntryCharactersOptions.ONLY_SIMPLIFIED -> {
                return if (useColours) _colouredSimplified!! else AnnotatedString(_simplified)
            }

            EntryCharactersOptions.PREFER_TRADITIONAL -> {
                return if (useColours) _colouredPreferTraditional!! else AnnotatedString(
                    _preferTraditional
                )
            }

            EntryCharactersOptions.PREFER_SIMPLIFIED -> {
                return if (useColours) _colouredPreferSimplified!! else AnnotatedString(
                    _preferSimplified
                )
            }
        }
    }

    fun getCharactersNoSecondary(
        options: EntryCharactersOptions,
        useColours: Boolean,
        colourPhoneticType: EntryColourPhoneticType,
        jyutpingToneColours: List<Color>,
        pinyinToneColours: List<Color>
    ): AnnotatedString {
        var hash = Objects.hash(
            _traditional,
            _simplified,
            _jyutping,
            _pinyin,
            options,
            useColours,
            colourPhoneticType
        )
        val colours =
            if (colourPhoneticType == EntryColourPhoneticType.MANDARIN) pinyinToneColours else jyutpingToneColours
        for (c in colours) {
            hash = Objects.hash(hash, c)
        }

        if (_cachedCharacterHash == null || _cachedCharacterHash != hash) {
            refreshColours(colourPhoneticType, jyutpingToneColours, pinyinToneColours)
            _cachedCharacterHash = hash
        }

        return when (options) {
            EntryCharactersOptions.ONLY_TRADITIONAL, EntryCharactersOptions.PREFER_TRADITIONAL -> {
                if (useColours) _colouredTraditional!! else AnnotatedString(_traditional)
            }

            EntryCharactersOptions.ONLY_SIMPLIFIED, EntryCharactersOptions.PREFER_SIMPLIFIED -> {
                if (useColours) _colouredSimplified!! else AnnotatedString(_simplified)
            }
        }
    }

    var traditional: String
        get() = _traditional
        set(value) {
            _traditional = value

            _simplifiedDifference = compareStrings(_traditional, _simplified)
            _traditionalDifference = compareStrings(_simplified, _traditional)
            _preferTraditional = "$_traditional [$_simplifiedDifference]"
            _preferSimplified = "$_simplified [$_traditionalDifference]"
        }

    var simplified: String
        get() = _simplified
        set(value) {
            _simplified = value

            _simplifiedDifference = compareStrings(_traditional, _simplified)
            _traditionalDifference = compareStrings(_simplified, _traditional)
            _preferTraditional = "$_traditional [$_simplifiedDifference]"
            _preferSimplified = "$_simplified [$_traditionalDifference]"
        }

    fun getPhonetic(
        options: EntryPhoneticOptions,
        cantoneseOptions: CantoneseOptions,
        mandarinOptions: MandarinOptions
    ): String {
        when (options) {
            EntryPhoneticOptions.ONLY_CANTONESE -> {
                return getCantonesePhonetic(cantoneseOptions)
            }

            EntryPhoneticOptions.ONLY_MANDARIN -> {
                return getMandarinPhonetic(mandarinOptions)
            }

            EntryPhoneticOptions.PREFER_CANTONESE -> {
                val cantonese = getCantonesePhonetic(cantoneseOptions)
                val mandarin = getMandarinPhonetic(mandarinOptions)

                return if (cantonese.isEmpty()) {
                    if (mandarin.isEmpty()) "" else "($mandarin)"
                } else if (mandarin.isEmpty()) {
                    cantonese
                } else {
                    "$cantonese ($mandarin)"
                }
            }

            EntryPhoneticOptions.PREFER_MANDARIN -> {
                val cantonese = getCantonesePhonetic(cantoneseOptions)
                val mandarin = getMandarinPhonetic(mandarinOptions)

                return if (mandarin.isEmpty()) {
                    if (cantonese.isEmpty()) "" else "($cantonese)"
                } else if (cantonese.isEmpty()) {
                    mandarin
                } else {
                    "$mandarin ($cantonese)"
                }
            }
        }
    }

    fun getCantonesePhonetic(cantoneseOptions: CantoneseOptions): String {
        when (cantoneseOptions) {
            CantoneseOptions.PRETTY_YALE -> {
                if (_yale == null) {
                    generatePhonetic(cantoneseOptions = EnumSet.of(CantoneseOptions.PRETTY_YALE))
                }
                return _yale ?: "Yale not available"
            }

            CantoneseOptions.CANTONESE_IPA -> {
                if (_cantoneseIPA == null) {
                    generatePhonetic(cantoneseOptions = EnumSet.of(CantoneseOptions.CANTONESE_IPA))
                }
                return _cantoneseIPA ?: "Cantonese IPA not available"
            }

            CantoneseOptions.RAW_JYUTPING -> {
                return _jyutping
            }
        }
    }

    fun getMandarinPhonetic(mandarinOptions: MandarinOptions): String {
        when (mandarinOptions) {
            MandarinOptions.PRETTY_PINYIN -> {
                if (_prettyPinyin == null) {
                    generatePhonetic(mandarinOptions = EnumSet.of(MandarinOptions.PRETTY_PINYIN))
                }
                return _prettyPinyin ?: "Pretty pinyin not available"
            }

            MandarinOptions.NUMBERED_PINYIN -> {
                if (_numberedPinyin == null) {
                    generatePhonetic(mandarinOptions = EnumSet.of(MandarinOptions.NUMBERED_PINYIN))
                }
                return _numberedPinyin ?: "Numbered pinyin not available"
            }

            MandarinOptions.ZHUYIN -> {
                if (_zhuyin == null) {
                    generatePhonetic(mandarinOptions = EnumSet.of(MandarinOptions.ZHUYIN))
                }
                return _zhuyin ?: "Zhuyin not available"
            }

            MandarinOptions.MANDARIN_IPA -> {
                if (_mandarinIPA == null) {
                    generatePhonetic(mandarinOptions = EnumSet.of(MandarinOptions.MANDARIN_IPA))
                }
                return _mandarinIPA ?: "Mandarin IPA not available"
            }

            MandarinOptions.RAW_PINYIN_WITH_V -> {
                if (_pinyinWithV == null) {
                    generatePhonetic(mandarinOptions = EnumSet.of(MandarinOptions.RAW_PINYIN_WITH_V))
                }
                return _pinyinWithV ?: "Pinyin with V not available"
            }

            else -> {
                return _pinyin
            }
        }
    }

    var jyutping: String
        get() = _jyutping
        set(value) {
            _jyutping = value.lowercase()
            _yale = null
            _cantoneseIPA = null
            _jyutpingNumbers = null
        }

    fun getJyutpingNumbers(): List<Int> {
        if (_jyutpingNumbers == null) {
            _jyutpingNumbers = mutableListOf()
        }

        var toneIdx = _jyutping.indexOfAny(charArrayOf('0', '1', '2', '3', '4', '5', '6'))
        while (toneIdx != -1) {
            _jyutpingNumbers?.add(_jyutping[toneIdx].toString().toInt())
            toneIdx = _jyutping.indexOfAny(
                charArrayOf('0', '1', '2', '3', '4', '5', '6'),
                startIndex = toneIdx + 1
            )
        }

        return _jyutpingNumbers?.toList() ?: listOf()
    }

    var pinyin: String
        get() = _pinyin
        set(value) {
            _pinyin = value.lowercase()
            _prettyPinyin = null
            _numberedPinyin = null
            _zhuyin = null
            _mandarinIPA = null
            _pinyinWithV = null
            _pinyinNumbers = null
        }

    fun getPinyinNumbers(): List<Int> {
        if (_pinyinNumbers == null) {
            _pinyinNumbers = mutableListOf()
        }

        var toneIdx = _pinyin.indexOfAny(charArrayOf('0', '1', '2', '3', '4', '5'))
        while (toneIdx != -1) {
            _pinyinNumbers?.add(_pinyin[toneIdx].toString().toInt())
            toneIdx = _pinyin.indexOfAny(
                charArrayOf('0', '1', '2', '3', '4', '5'),
                startIndex = toneIdx + 1
            )
        }

        return _pinyinNumbers?.toList() ?: listOf()
    }

    fun getDefinitionsSets(): List<DefinitionsSet>? {
        return _definitions
    }

    fun getDefinitionSnippet(): String {
        if (_definitions == null || _definitions!!.isEmpty()) {
            return ""
        }

        if (_definitionSnippet == null) {
            for (d in _definitions) {
                if (d.isEmpty()) {
                    continue
                }

                _definitionSnippet = d.getDefinitionSnippet()
                break
            }
        }

        return _definitionSnippet ?: ""
    }

    private fun generatePhonetic(
        cantoneseOptions: EnumSet<CantoneseOptions> = EnumSet.noneOf(CantoneseOptions::class.java),
        mandarinOptions: EnumSet<MandarinOptions> = EnumSet.noneOf(MandarinOptions::class.java)
    ) {
        if (cantoneseOptions.contains(CantoneseOptions.PRETTY_YALE) && _yale == null) {
            _yale = convertJyutpingToYale(_jyutping)
        }
        if (cantoneseOptions.contains(CantoneseOptions.CANTONESE_IPA) && _cantoneseIPA == null) {
            _cantoneseIPA = convertJyutpingToIPA(_jyutping)
        }

        if (mandarinOptions.contains(MandarinOptions.PRETTY_PINYIN) && _prettyPinyin == null) {
            _prettyPinyin = createPrettyPinyin(_pinyin)
        }
        if (mandarinOptions.contains(MandarinOptions.NUMBERED_PINYIN) && _numberedPinyin == null) {
            _numberedPinyin = createNumberedPinyin(_pinyin)
        }
        if (mandarinOptions.contains(MandarinOptions.ZHUYIN) && _zhuyin == null) {
            _zhuyin = convertPinyinToZhuyin(_pinyin)
        }
        if (mandarinOptions.contains(MandarinOptions.MANDARIN_IPA) && _mandarinIPA == null) {
            _mandarinIPA = convertPinyinToIPA(_pinyin)
        }
        if (mandarinOptions.contains(MandarinOptions.RAW_PINYIN_WITH_V) && _pinyinWithV == null) {
            _pinyinWithV = createPinyinWithV(_pinyin)
        }
    }

    private fun refreshColours(
        type: EntryColourPhoneticType,
        jyutpingToneColours: List<Color>,
        pinyinToneColours: List<Color>
    ) {
        var tones: List<Int>
        when (type) {
            EntryColourPhoneticType.NONE -> {
                _colouredTraditional = AnnotatedString(_traditional)
                _colouredSimplified = AnnotatedString(_simplified)
                _colouredTraditionalDifference = AnnotatedString(_traditionalDifference)
                _colouredSimplifiedDifference = AnnotatedString(_simplifiedDifference)
                _colouredPreferTraditional =
                    _colouredSimplifiedDifference?.let {
                        _colouredTraditional?.plus(AnnotatedString("["))
                            ?.plus(it)
                    }?.plus(
                        AnnotatedString(
                            "]"
                        )
                    )
                _colouredPreferSimplified =
                    _colouredTraditionalDifference?.let {
                        _colouredSimplified?.plus(AnnotatedString("["))
                            ?.plus(it)
                    }?.plus(
                        AnnotatedString(
                            "]"
                        )
                    )
                return
            }

            EntryColourPhoneticType.CANTONESE -> {
                tones = getJyutpingNumbers()
            }

            EntryColourPhoneticType.MANDARIN -> {
                tones = getPinyinNumbers()
            }
        }

        _colouredTraditional = applyColours(
            text = _traditional,
            tones = tones,
            jyutpingToneColours = jyutpingToneColours,
            pinyinToneColours = pinyinToneColours,
            type = type
        )
        _colouredSimplified = applyColours(
            text = _simplified,
            tones = tones,
            jyutpingToneColours = jyutpingToneColours,
            pinyinToneColours = pinyinToneColours,
            type = type
        )
        _colouredTraditionalDifference = applyColours(
            text = _traditionalDifference,
            tones = tones,
            jyutpingToneColours = jyutpingToneColours,
            pinyinToneColours = pinyinToneColours,
            type = type
        )
        _colouredSimplifiedDifference = applyColours(
            text = _simplifiedDifference,
            tones = tones,
            jyutpingToneColours = jyutpingToneColours,
            pinyinToneColours = pinyinToneColours,
            type = type
        )
        _colouredPreferTraditional = _colouredSimplifiedDifference?.let {
            _colouredTraditional?.plus(AnnotatedString("["))
                ?.plus(it)?.plus(AnnotatedString("]")) ?: AnnotatedString("")
        }
        _colouredPreferSimplified = _colouredTraditionalDifference?.let {
            _colouredSimplified?.plus(AnnotatedString("["))
                ?.plus(it)?.plus(AnnotatedString("]")) ?: AnnotatedString("")
        }
    }
}