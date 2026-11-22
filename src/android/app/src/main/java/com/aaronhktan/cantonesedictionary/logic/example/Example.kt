package com.aaronhktan.cantonesedictionary.logic.example

import com.aaronhktan.cantonesedictionary.logic.entry.CantoneseOptions
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.EntryCharactersOptions
import com.aaronhktan.cantonesedictionary.logic.entry.EntryPhoneticOptions
import com.aaronhktan.cantonesedictionary.logic.entry.MandarinOptions
import com.aaronhktan.cantonesedictionary.logic.example.TranslationSet
import com.aaronhktan.cantonesedictionary.logic.utils.convertJyutpingToIPA
import com.aaronhktan.cantonesedictionary.logic.utils.convertJyutpingToYale
import com.aaronhktan.cantonesedictionary.logic.utils.convertPinyinToIPA
import com.aaronhktan.cantonesedictionary.logic.utils.convertPinyinToZhuyin
import com.aaronhktan.cantonesedictionary.logic.utils.createNumberedPinyin
import com.aaronhktan.cantonesedictionary.logic.utils.createPinyinWithV
import com.aaronhktan.cantonesedictionary.logic.utils.createPrettyPinyin
import java.util.EnumSet
import kotlin.uuid.ExperimentalUuidApi
import kotlin.uuid.Uuid

class Example : Cloneable {
    @OptIn(ExperimentalUuidApi::class)
    private val id: Uuid = Uuid.random()
    private var _sourceLanguage: String = ""
    private var _traditional: String = ""
    private var _simplified: String = ""

    private var _jyutping: String = ""
    private var _yale: String? = null
    private var _cantoneseIPA: String? = null

    private var _pinyin: String = ""
    private var _prettyPinyin: String? = null
    private var _numberedPinyin: String? = null
    private var _zhuyin: String? = null
    private var _mandarinIPA: String? = null
    private var _pinyinWithV: String? = null

    private var _translations: List<TranslationSet> = listOf()

    constructor(
        sourceLanguage: String,
        traditional: String,
        simplified: String,
        jyutping: String,
        pinyin: String,
        translations: List<TranslationSet>
    ) {
        _sourceLanguage = sourceLanguage
        _traditional = traditional
        _simplified = simplified
        _jyutping = jyutping.lowercase()
        _pinyin = pinyin.lowercase()
        _translations = translations
    }

    public override fun clone() = Example(
        sourceLanguage = _sourceLanguage,
        traditional = _traditional,
        simplified = _simplified,
        jyutping = _jyutping,
        pinyin = _pinyin,
        translations = _translations.map { it.clone() }
    )

    var sourceLanguage: String
        get() = _sourceLanguage
        set(value) {
            _sourceLanguage = value
        }

    fun getCharacters(options: EntryCharactersOptions): String {
        return when (options) {
            EntryCharactersOptions.ONLY_TRADITIONAL, EntryCharactersOptions.PREFER_TRADITIONAL -> {
                _traditional
            }

            EntryCharactersOptions.ONLY_SIMPLIFIED, EntryCharactersOptions.PREFER_SIMPLIFIED -> {
                _simplified
            }
        }
    }

    var traditional: String
        get() = _traditional
        set(value) {
            _traditional = traditional
        }

    var simplified: String
        get() = _simplified
        set(value) {
            _simplified = simplified
        }

    fun getPhonetic(
        options: EntryPhoneticOptions,
        cantoneseOptions: CantoneseOptions,
        mandarinOptions: MandarinOptions
    ): String {
        return when (options) {
            EntryPhoneticOptions.ONLY_CANTONESE, EntryPhoneticOptions.PREFER_CANTONESE -> {
                getCantonesePhonetic(cantoneseOptions)
            }

            EntryPhoneticOptions.ONLY_MANDARIN, EntryPhoneticOptions.PREFER_MANDARIN -> {
                getMandarinPhonetic(mandarinOptions)
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
        }

    var pinyin: String
        get() = pinyin
        set(value) {
            _pinyin = value.lowercase()
            _prettyPinyin = null
            _numberedPinyin = null
            _zhuyin = null
            _mandarinIPA = null
            _pinyinWithV = null
        }

    var translationSets: List<TranslationSet>
        get() = _translations
        set(value) {
            _translations = value
        }

    fun getTranslationSnippet(): String {
        if (_translations.isEmpty()) {
            return ""
        }

        val translationSet: TranslationSet = _translations[0]
        val snippets: List<Translation> = translationSet.getTranslationSnippet()
        if (snippets.isEmpty()) {
            return ""
        }

        return snippets[0].content
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
}