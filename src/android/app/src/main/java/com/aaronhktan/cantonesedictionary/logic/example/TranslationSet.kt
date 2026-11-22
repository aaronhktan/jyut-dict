package com.aaronhktan.cantonesedictionary.logic.example

import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import kotlin.uuid.ExperimentalUuidApi
import kotlin.uuid.Uuid

data class Translation(
    val content: String,
    val language: String,
    val directTarget: Boolean
) : Cloneable {
    public override fun clone() = Translation(
        content = content,
        language = language,
        directTarget = directTarget,
    )
}

class TranslationSet : Cloneable {
    @OptIn(ExperimentalUuidApi::class)
    private val id: Uuid = Uuid.random()
    private var _source: String = ""
    private var _snippet: List<Translation>? = null
    private var _translations: List<Translation> = listOf()

    constructor(source: String, translations: List<Translation>) {
        _source = source
        _translations = translations
    }

    public override fun clone() = TranslationSet(
        source = _source,
        translations = _translations.map { it.clone() }
    )

    fun isEmpty(): Boolean {
        return (_translations.isEmpty()
                || _translations.all { it.content.isEmpty() && it.language.isEmpty() && !it.directTarget })
    }

    var source: String = _source
        private set

    fun getTranslationSnippet(): List<Translation> {
        _snippet?.let { return it }

        val snippets: MutableList<Translation> = mutableListOf()
        for (t in _translations) {
            snippets.add(t)
        }
        _snippet = snippets.toList()

        return _snippet ?: listOf()
    }

    var translations: List<Translation> = _translations
        private set
}