package com.aaronhktan.cantonesedictionary.ui.components.entry

import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.example.Example

data class EntryDetailUiState(
    val currentEntry: Entry? = null,
    val allExamples: List<Example>? = null,
    val examplesBySource: Map<String, List<Example>>? = null
)