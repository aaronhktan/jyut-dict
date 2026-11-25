package com.aaronhktan.cantonesedictionary.ui.components.search

import android.util.Log
import androidx.activity.compose.BackHandler
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.KeyboardArrowDown
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.outlined.Edit
import androidx.compose.material.icons.outlined.Email
import androidx.compose.material.icons.outlined.Settings
import androidx.compose.material3.DropdownMenuGroup
import androidx.compose.material3.DropdownMenuPopup
import androidx.compose.material3.ExpandedFullScreenContainedSearchBar
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MenuAnchorPosition
import androidx.compose.material3.MenuDefaults
import androidx.compose.material3.PlainTooltip
import androidx.compose.material3.Scaffold
import androidx.compose.material3.SearchBar
import androidx.compose.material3.SearchBarDefaults
import androidx.compose.material3.SearchBarState
import androidx.compose.material3.SearchBarValue
import androidx.compose.material3.SelectableDropdownMenuItem
import androidx.compose.material3.SplitButtonDefaults
import androidx.compose.material3.SplitButtonLayout
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TooltipAnchorPosition
import androidx.compose.material3.TooltipBox
import androidx.compose.material3.TooltipDefaults
import androidx.compose.material3.rememberSearchBarState
import androidx.compose.material3.rememberTooltipState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.runtime.snapshotFlow
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.focusProperties
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.semantics.LiveRegionMode
import androidx.compose.ui.semantics.clearAndSetSemantics
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.liveRegion
import androidx.compose.ui.semantics.paneTitle
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.semantics.stateDescription
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.times
import androidx.compose.ui.window.PopupProperties
import com.aaronhktan.cantonesedictionary.R
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.ui.components.SearchScreenUiState
import com.aaronhktan.cantonesedictionary.ui.components.entry.EntryRow
import com.aaronhktan.cantonesedictionary.ui.theme.CantoneseDictionaryTheme
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.launch

private const val LOG_TAG = "SearchScreen"

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
    var languageSelectorExpanded by rememberSaveable { mutableStateOf(false) }

    Scaffold(
        topBar = {
            if (!isSinglePane) {
                SearchScreenContents(
                    searchScreenUiState = searchScreenUiState,
                    searchBarState = searchBarState,
                    listState = listState,
                    languageSelectorExpanded = languageSelectorExpanded,
                    isSinglePane = isSinglePane,
                    onLanguageSelectorToggled = { value ->
                        Log.d(LOG_TAG, "Updating language selector toggle to $value")
                        languageSelectorExpanded = value
                    },
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
                    languageSelectorExpanded = languageSelectorExpanded,
                    isSinglePane = isSinglePane,
                    onLanguageSelectorToggled = { value ->
                        Log.d(LOG_TAG, "Updating language selector toggle to $value")
                        languageSelectorExpanded = value
                    },
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
    languageSelectorExpanded: Boolean,
    isSinglePane: Boolean,
    onLanguageSelectorToggled: (Boolean) -> Unit,
    onQueryChanged: (String) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    var shouldReexpand by rememberSaveable { mutableStateOf(false) }
    val scope = rememberCoroutineScope()

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

    if (isSinglePane) {
        val inputField: @Composable () -> Unit =
            { SearchInputField(searchScreenUiState, searchBarState) }
        SearchBar(
            state = searchBarState,
            inputField = inputField,
            modifier = modifier
                .focusProperties {
                    canFocus = searchBarState.currentValue == SearchBarValue.Expanded
                }
        )
        ExpandedFullScreenContainedSearchBar(state = searchBarState, inputField = inputField) {
            SearchResults(
                searchScreenUiState = searchScreenUiState,
                listState = listState,
                languageSelectorExpanded = languageSelectorExpanded,
                isSinglePane = isSinglePane,
                onEntrySelected = { result ->
                    shouldReexpand = true
                    scope.launch {
                        searchBarState.animateToCollapsed()
                    }
                    onEntrySelected(result)
                },
                onLanguageSelectorToggled = onLanguageSelectorToggled,
                modifier = Modifier
                    .fillMaxWidth(),
            )
        }
    } else {
        CustomDockedSearchPopup(
            expanded = searchBarState.currentValue == SearchBarValue.Expanded,
            onDismissRequest = {
                scope.launch {
                    searchBarState.animateToCollapsed()
                }
            },
            modifier = modifier
                .focusProperties {
                    canFocus = searchBarState.currentValue == SearchBarValue.Expanded
                },
            inputField = {
                Surface(
                    modifier = Modifier.fillMaxWidth(),
                    shape = SearchBarDefaults.dockedShape,
                    color = SearchBarDefaults.colors().containerColor,
                    tonalElevation = SearchBarDefaults.TonalElevation,
                ) {
                    SearchInputField(
                        searchScreenUiState,
                        searchBarState,
                        Modifier
                            .focusProperties {
                                canFocus = searchBarState.currentValue == SearchBarValue.Expanded
                            }
                    )
                }
            },
        ) {
            SearchResults(
                searchScreenUiState = searchScreenUiState,
                listState = listState,
                languageSelectorExpanded = languageSelectorExpanded,
                isSinglePane = isSinglePane,
                onEntrySelected = { result ->
                    onEntrySelected(result)
                },
                onLanguageSelectorToggled = onLanguageSelectorToggled,
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
fun SearchInputField(
    searchScreenUiState: SearchScreenUiState,
    searchBarState: SearchBarState,
    modifier: Modifier = Modifier
) {
    val keyboardController = LocalSoftwareKeyboardController.current
    val focusManager = LocalFocusManager.current

    SearchBarDefaults.InputField(
        textFieldState = searchScreenUiState.textFieldState,
        searchBarState = searchBarState,
        onSearch = {
            keyboardController?.hide()
            focusManager.clearFocus()
        },
        modifier = modifier,
        placeholder = {
            Text(modifier = Modifier.clearAndSetSemantics {}, text = "Search")
        },
        leadingIcon = { Icon(Icons.Default.Search, contentDescription = "Search") },
    )
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SearchResults(
    searchScreenUiState: SearchScreenUiState,
    listState: LazyListState,
    languageSelectorExpanded: Boolean,
    isSinglePane: Boolean,
    onLanguageSelectorToggled: (Boolean) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    val searchResults = searchScreenUiState.searchResults ?: listOf()
    var buttonHeight by remember { mutableIntStateOf(0) }
    val density = LocalDensity.current

    Box(
        modifier = modifier
            .fillMaxSize(),
    ) {
        LazyColumn(
            modifier = Modifier.fillMaxSize(),
            state = listState,
            contentPadding = PaddingValues(
                bottom = with(density) {
                    buttonHeight.toDp() + 2 * dimensionResource(R.dimen.content_padding)
                }
            )
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

        Box(
            modifier = Modifier
                .wrapContentSize()
                .align(Alignment.BottomCenter)
                .imePadding()
                .padding(bottom = dimensionResource(R.dimen.content_padding))
                .onSizeChanged {
                    buttonHeight = it.height
                }
                .focusProperties {
                    canFocus = false
                }
        ) {
            SplitButtonLayout(
                leadingButton = {
                    SplitButtonDefaults.LeadingButton(
                        onClick = { onLanguageSelectorToggled(true) }
                    ) {
                        Text("Fuzzy Jyutping")
                    }
                },
                trailingButton = {
                    val description = "Switch search language"
                    TooltipBox(
                        positionProvider =
                            TooltipDefaults.rememberTooltipPositionProvider(
                                TooltipAnchorPosition.Above
                            ),
                        tooltip = {
                            PlainTooltip(
                                modifier =
                                    Modifier.semantics {
                                        liveRegion = LiveRegionMode.Assertive
                                        paneTitle = description
                                    }
                            ) {
                                Text(description)
                            }
                        },
                        state = rememberTooltipState(),
                    ) {
                        SplitButtonDefaults.TrailingButton(
                            checked = languageSelectorExpanded,
                            onCheckedChange = { onLanguageSelectorToggled(it) },
                            modifier =
                                Modifier.semantics {
                                    stateDescription =
                                        if (languageSelectorExpanded) "Expanded" else "Collapsed"
                                    contentDescription = description
                                },
                        ) {
                            val rotation: Float by
                            animateFloatAsState(
                                targetValue = if (languageSelectorExpanded) 180f else 0f,
                                label = "Trailing Icon Rotation",
                            )
                            Icon(
                                Icons.Filled.KeyboardArrowDown,
                                modifier =
                                    Modifier
                                        .size(SplitButtonDefaults.TrailingIconSize)
                                        .graphicsLayer {
                                            this.rotationZ = rotation
                                        },
                                contentDescription = "Localized description",
                            )
                        }
                    }
                },
            )

            DropdownMenuPopup(
                expanded = languageSelectorExpanded,
                onDismissRequest = { onLanguageSelectorToggled(false) },
                popupPositionProvider = MenuDefaults.rememberDropdownMenuPopupPositionProvider(
                    dropdownMenuAnchorPosition = MenuAnchorPosition.Above
                ),
                properties = PopupProperties(
                    // This prevents the keyboard from being dismissed, since the button can't steal focus away
                    focusable = false,
                    // When focusable is false, the tap registers to "outside of the popup".
                    // So we must set this value to false to prevent the trailing button from
                    // firing twice: once from tapping the button, and once from tapping "outside" it
                    dismissOnClickOutside = false,
                ),
            ) {
                DropdownMenuGroup(
                    shapes = MenuDefaults.groupShape(0, 1),
                    containerColor = MenuDefaults.groupVibrantContainerColor,
                ) {
                    SelectableDropdownMenuItem(
                        selected = true,
                        text = { Text("Edit") },
                        shapes = MenuDefaults.itemShape(0, 3),
                        onClick = { /* Handle edit! */ },
                        leadingIcon = { Icon(Icons.Outlined.Edit, contentDescription = null) },
                        colors = MenuDefaults.selectableItemVibrantColors(),
                    )
                    SelectableDropdownMenuItem(
                        selected = false,
                        text = { Text("Settings") },
                        shapes = MenuDefaults.itemShape(1, 3),
                        onClick = { /* Handle settings! */ },
                        leadingIcon = { Icon(Icons.Outlined.Settings, contentDescription = null) },
                        colors = MenuDefaults.selectableItemVibrantColors(),
                    )
                    SelectableDropdownMenuItem(
                        selected = false,
                        text = { Text("Send Feedback") },
                        shapes = MenuDefaults.itemShape(2, 3),
                        onClick = { /* Handle send feedback! */ },
                        leadingIcon = { Icon(Icons.Outlined.Email, contentDescription = null) },
                        colors = MenuDefaults.selectableItemVibrantColors(),
                    )
                }
            }
        }

        // This allows "click outside of the popup to dismiss" behaviour that's disabled by `dismissOnClickOutside = false`
        if (languageSelectorExpanded) {
            Box(
                Modifier
                    .fillMaxSize()
                    .clickable(
                        interactionSource = remember { MutableInteractionSource() },
                        indication = null
                    ) { onLanguageSelectorToggled(false) }
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
