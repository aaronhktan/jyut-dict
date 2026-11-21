package com.aaronhktan.cantonesedictionary.logic.entry

import com.aaronhktan.cantonesedictionary.logic.example.Example
import kotlin.uuid.ExperimentalUuidApi
import kotlin.uuid.Uuid

data class Definition(
    val definitionContent: String,
    val label: String,
    var examples: List<Example>
)

class DefinitionsSet {
    @OptIn(ExperimentalUuidApi::class)
    private val id: Uuid = Uuid.random()
    private val _source: String = ""
    private val _sourceShortString: String = ""
    private var _snippet: String? = null
    private val _definitions: MutableList<Definition> = mutableListOf()

    constructor(
        source: String,
        sourceShortString: String,
        definitions: List<Definition>
    ) {
        _source = source
        _sourceShortString = sourceShortString
        _definitions = definitions.toMutableList()
    }

    fun isEmpty(): Boolean {
        return _definitions.isEmpty()
    }

    fun appendDefinition(definition: Definition) {
        _definitions.add(definition)
    }

    fun generatePhonetic(cantoneseOptions: CantoneseOptions, mandarinOptions: MandarinOptions) {
        for (d in _definitions) {
            for (e in d.examples) {
                e.generatePhonetic(cantoneseOptions, mandarinOptions)
            }
        }
    }

    var source: String = _source
        private set

    var sourceShortString = _sourceShortString
        private set

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

    var definitions: List<Definition> = _definitions
        private set
}