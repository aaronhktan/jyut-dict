package com.aaronhktan.cantonesedictionary.ui.components

import androidx.compose.foundation.text.input.TextFieldState
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.utils.InputMethod

data class SearchScreenUiState(
    // See https://developer.android.com/develop/ui/compose/text/migrate-state-based#state-based
    val textFieldState: TextFieldState = TextFieldState(),
    val selectedInputMethod: InputMethod = InputMethod.AUTO_DETECT,
    val detectedInputMethod: InputMethod? = null,
    val searchResults: List<Entry>? = null,
)