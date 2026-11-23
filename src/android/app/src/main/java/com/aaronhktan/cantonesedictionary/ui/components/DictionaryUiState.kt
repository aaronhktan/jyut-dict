package com.aaronhktan.cantonesedictionary.ui.components

import androidx.compose.foundation.text.input.TextFieldState
import androidx.compose.material3.SearchBarState
import com.aaronhktan.cantonesedictionary.logic.entry.Entry

data class DictionaryUiState(
    // See https://developer.android.com/develop/ui/compose/text/migrate-state-based#state-based
    val textFieldState: TextFieldState = TextFieldState(),

    val searchResults: List<Entry>? = null,

    val currentSelectedEntry: Entry? = null,
)