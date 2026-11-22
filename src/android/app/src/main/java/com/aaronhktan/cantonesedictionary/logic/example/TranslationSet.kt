package com.aaronhktan.cantonesedictionary.logic.example

import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import java.util.Objects
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

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is TranslationSet) return false

        return (this._source == other._source
                && this._translations == other._translations)
    }

    override fun hashCode(): Int {
        var hash = Objects.hash(_source)
        for (t in _translations) {
            hash = Objects.hash(hash, t)
        }
        return hash
    }

    fun isEmpty(): Boolean {
        return (_translations.isEmpty()
                || _translations.all { it.content.isEmpty() && it.language.isEmpty() && !it.directTarget })
    }

    val source: String
        get() = _source

    fun getTranslationSnippet(): List<Translation> {
        _snippet?.let { return it }

        val snippets: MutableList<Translation> = mutableListOf()
        for (i in 0..4) {
            if (i < _translations.size) {
                snippets.add(_translations[i])
            }
        }
        _snippet = snippets.toList()

        return _snippet ?: listOf()
    }

    val translations: List<Translation>
        get() = _translations.map { it.clone() }
}