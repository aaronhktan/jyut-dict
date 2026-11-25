package com.aaronhktan.cantonesedictionary.ui.components.search

import androidx.activity.compose.BackHandler
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.ExpandedDockedSearchBarWithGap
import androidx.compose.material3.ExpandedFullScreenContainedSearchBar
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.SearchBar
import androidx.compose.material3.SearchBarDefaults
import androidx.compose.material3.SearchBarState
import androidx.compose.material3.SearchBarValue
import androidx.compose.material3.Text
import androidx.compose.material3.rememberSearchBarState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.runtime.snapshotFlow
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.focusProperties
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.semantics.clearAndSetSemantics
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.PopupProperties
import com.aaronhktan.cantonesedictionary.R
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.ui.components.SearchScreenUiState
import com.aaronhktan.cantonesedictionary.ui.components.entry.EntryRow
import com.aaronhktan.cantonesedictionary.ui.theme.CantoneseDictionaryTheme
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.launch

@Composable
fun SearchScreen(
    searchScreenUiState: SearchScreenUiState,
    isSinglePane: Boolean,
    onQueryChanged: (String) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    val searchBarState = rememberSearchBarState()
    val listState = rememberLazyListState()

    Scaffold(
        topBar = {
            if (!isSinglePane) {
                SearchScreenContents(
                    searchScreenUiState = searchScreenUiState,
                    searchBarState = searchBarState,
                    listState = listState,
                    isSinglePane = isSinglePane,
                    onQueryChanged = onQueryChanged,
                    onEntrySelected = onEntrySelected,
                    Modifier
                        .padding(
                            horizontal = dimensionResource(R.dimen.horizontal_padding),
                            vertical = dimensionResource(R.dimen.vertical_padding)
                        )
                )
            }
        },
        bottomBar = {
            if (isSinglePane) {
                SearchScreenContents(
                    searchScreenUiState = searchScreenUiState,
                    searchBarState = searchBarState,
                    listState = listState,
                    isSinglePane = isSinglePane,
                    onQueryChanged = onQueryChanged,
                    onEntrySelected = onEntrySelected,
                    Modifier
                        .fillMaxWidth()
                        .padding(
                            horizontal = dimensionResource(R.dimen.horizontal_padding),
                            vertical = dimensionResource(R.dimen.vertical_padding)
                        )
                )
            }
        },
        modifier = modifier
    ) { innerPadding ->
        Box {
            LazyColumn(contentPadding = innerPadding) {
                // TODO: Show welcome screen here
            }
        }
    }
}

@Composable
private fun SearchScreenContents(
    searchScreenUiState: SearchScreenUiState,
    searchBarState: SearchBarState,
    listState: LazyListState,
    isSinglePane: Boolean,
    onQueryChanged: (String) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    var shouldReexpand by rememberSaveable { mutableStateOf(false) }
    val scope = rememberCoroutineScope()
    val inputField =
        @Composable {
            val keyboardController = LocalSoftwareKeyboardController.current
            val focusManager = LocalFocusManager.current

            SearchBarDefaults.InputField(
                textFieldState = searchScreenUiState.textFieldState,
                searchBarState = searchBarState,
                onSearch = {
                    keyboardController?.hide()
                    focusManager.clearFocus()
                },
                placeholder = {
                    Text(modifier = Modifier.clearAndSetSemantics {}, text = "Search")
                },
                leadingIcon = { Icon(Icons.Default.Search, contentDescription = "Search") },
            )
        }

    LaunchedEffect(Unit) {
        if (shouldReexpand && isSinglePane) {
            shouldReexpand = false
            searchBarState.animateToExpanded()
        }
    }

    LaunchedEffect(searchScreenUiState.textFieldState) {
        snapshotFlow { searchScreenUiState.textFieldState.text.toString() }
            .distinctUntilChanged()
            .collectLatest { query ->
                onQueryChanged(query)
            }
    }

    SearchBar(
        state = searchBarState,
        inputField = inputField,
        modifier = modifier
            .focusProperties {
                canFocus = searchBarState.currentValue == SearchBarValue.Expanded
            }
    )
    if (isSinglePane) {
        ExpandedFullScreenContainedSearchBar(state = searchBarState, inputField = inputField) {
            SearchResults(
                searchScreenUiState = searchScreenUiState,
                listState = listState,
                isSinglePane = isSinglePane,
                onEntrySelected = { result ->
                    shouldReexpand = true
                    scope.launch {
                        searchBarState.animateToCollapsed()
                    }
                    onEntrySelected(result)
                },
                modifier = Modifier
                    .fillMaxWidth(),
            )
        }
    } else {
        ExpandedDockedSearchBarWithGap(
            state = searchBarState,
            inputField = inputField,
            dropdownScrimColor = Color.Unspecified,
            properties = PopupProperties(
                dismissOnClickOutside = false,
                focusable = false
            ),
        ) {
            SearchResults(
                searchScreenUiState = searchScreenUiState,
                listState = listState,
                isSinglePane = isSinglePane,
                onEntrySelected = { result ->
                    onEntrySelected(result)
                },
                modifier = Modifier
                    .fillMaxWidth()
                    .heightIn(min = dimensionResource(R.dimen.search_pane_min_height)),
            )

        }
    }

    BackHandler(enabled = searchBarState.currentValue == SearchBarValue.Expanded) {
        scope.launch { searchBarState.animateToCollapsed() }
    }
}

@Composable
fun SearchResults(
    searchScreenUiState: SearchScreenUiState,
    listState: LazyListState,
    isSinglePane: Boolean,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    val searchResults = searchScreenUiState.searchResults ?: listOf()
    LazyColumn(
        state = listState,
        modifier = modifier
    ) {
        items(searchResults, key = { entry -> entry.id }) { entry ->
            EntryRow(
                entry,
                modifier = Modifier
                    .fillMaxWidth()
                    .clickable {
                        onEntrySelected(entry)
                    }
            )
        }
    }

    if (!isSinglePane) {
        LaunchedEffect(searchResults) {
            if (searchResults.isNotEmpty()) {
                onEntrySelected(searchResults[0])
            }
        }
    }
}

@Preview(showBackground = true)
@Composable
fun SearchScreenPreviewSinglePane() {
    CantoneseDictionaryTheme {
        SearchScreen(
            searchScreenUiState = SearchScreenUiState(),
            isSinglePane = true,
            onQueryChanged = {},
            onEntrySelected = {})
    }
}

@Preview(showBackground = true)
@Composable
fun SearchScreenPreviewDoublePane() {
    CantoneseDictionaryTheme {
        SearchScreen(
            searchScreenUiState = SearchScreenUiState(),
            isSinglePane = false,
            onQueryChanged = {},
            onEntrySelected = {})
    }
}
