package com.aaronhktan.cantonesedictionary.logic.entry

import com.aaronhktan.cantonesedictionary.logic.example.Example
import java.util.Objects
import kotlin.uuid.ExperimentalUuidApi
import kotlin.uuid.Uuid

data class Definition(
    val definitionContent: String,
    val label: String,
    var examples: List<Example>
) : Cloneable {
    public override fun clone() = Definition(
        definitionContent = definitionContent,
        label = label,
        examples = examples.map { it.clone() }
    )
}

class DefinitionsSet : Cloneable {
    @OptIn(ExperimentalUuidApi::class)
    private val id: Uuid = Uuid.random()
    private var _source: String = ""
    private var _sourceShortString: String = ""
    private var _snippet: String? = null
    private var _definitions: MutableList<Definition> = mutableListOf()

    constructor(
        source: String,
        sourceShortString: String,
        definitions: List<Definition>
    ) {
        _source = source
        _sourceShortString = sourceShortString
        _definitions = definitions.map { it.clone() }.toMutableList()
    }

    public override fun clone() = DefinitionsSet(
        source = _source,
        sourceShortString = _sourceShortString,
        definitions = _definitions.map { it.clone() }
    )

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is DefinitionsSet) return false

        return (this._source == other._source
                && this._sourceShortString == other._sourceShortString
                && this._definitions == other._definitions)
    }

    override fun hashCode(): Int {
        var hash = Objects.hash(
            _source, _sourceShortString
        )
        for (d in _definitions) {
            hash = Objects.hash(hash, d)
        }
        return hash
    }

    fun isEmpty(): Boolean {
        return _definitions.isEmpty()
    }

    fun appendDefinition(definition: Definition) {
        _definitions.add(definition)
    }

    val source: String
        get() = _source

    val sourceShortString: String
        get() = _sourceShortString

    fun getDefinitionSnippet(): String {
        if (isEmpty()) {
            return ""
        }

        _snippet?.let { return it }

        val snippets: MutableList<String> = mutableListOf()
        for ((definitionContent) in _definitions) {
            var firstLineBreak =
                definitionContent.indexOfFirst { setOf('\r', '\n').contains(it) }
            if (firstLineBreak == -1) {
                firstLineBreak = definitionContent.length
            }
            snippets.add(definitionContent.substring(0..<firstLineBreak))
        }
        _snippet = snippets.joinToString("; ")

        return _snippet ?: ""
    }

    val definitions: List<Definition>
        get() = _definitions.map { it.clone() }
}