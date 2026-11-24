package com.aaronhktan.cantonesedictionary.ui.components

import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel

@Composable
fun CantoneseDictionaryApp(
    modifier: Modifier = Modifier,
) {
    val viewModel: DictionaryViewModel = viewModel()
    val dictionaryUiState = viewModel.uiState.collectAsState().value

    HomeScreen(
        dictionaryUiState = dictionaryUiState,
        onQueryChanged = {
            viewModel.updateSearchQuery(it)
        },
        onEntrySelected = {
            viewModel.updateDetailsScreenStates(it)
        },
        modifier = modifier
    )
}
