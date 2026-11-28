package com.aaronhktan.cantonesedictionary.ui.components.search

import android.util.Log
import androidx.activity.compose.BackHandler
import androidx.compose.animation.AnimatedContent
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.expandHorizontally
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.animation.shrinkHorizontally
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.ime
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.ArrowBackIosNew
import androidx.compose.material.icons.filled.ChevronLeft
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.icons.filled.KeyboardArrowDown
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.filled.SearchOff
import androidx.compose.material3.DropdownMenuGroup
import androidx.compose.material3.DropdownMenuPopup
import androidx.compose.material3.ExpandedFullScreenContainedSearchBar
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
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
import androidx.compose.material3.TopAppBar
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
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.focus.onFocusChanged
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.platform.LocalWindowInfo
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.semantics.LiveRegionMode
import androidx.compose.ui.semantics.clearAndSetSemantics
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.liveRegion
import androidx.compose.ui.semantics.paneTitle
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.semantics.stateDescription
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.times
import androidx.compose.ui.window.PopupProperties
import com.aaronhktan.cantonesedictionary.R
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.utils.InputMethod
import com.aaronhktan.cantonesedictionary.ui.components.SearchScreenUiState
import com.aaronhktan.cantonesedictionary.ui.components.entry.EntryRow
import com.aaronhktan.cantonesedictionary.ui.theme.CantoneseDictionaryTheme
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.flow.drop
import kotlinx.coroutines.launch

private const val LOG_TAG = "SearchScreen"

private val inputMethodOptions: List<InputMethod> = listOf(
    InputMethod.AUTO_DETECT,
    InputMethod.TRADITIONAL,
    InputMethod.SIMPLIFIED,
    InputMethod.FUZZY_JYUTPING,
    InputMethod.JYUTPING,
    InputMethod.FUZZY_PINYIN,
    InputMethod.PINYIN,
    InputMethod.ENGLISH
)

@Composable
fun SearchScreen(
    searchScreenUiState: SearchScreenUiState,
    isSinglePane: Boolean,
    onClearText: () -> Unit,
    onInputMethodChanged: (InputMethod) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    val searchBarState = rememberSearchBarState()
    val listState = rememberLazyListState()
    var inputMethodSelectorExpanded by rememberSaveable { mutableStateOf(false) }

    Scaffold(
        topBar = {
            if (!isSinglePane) {
                SearchScreenContents(
                    searchScreenUiState = searchScreenUiState,
                    searchBarState = searchBarState,
                    listState = listState,
                    inputMethodSelectorExpanded = inputMethodSelectorExpanded,
                    isSinglePane = isSinglePane,
                    onClearText = onClearText,
                    onInputMethodSelectorToggled = { value ->
                        inputMethodSelectorExpanded = value
                    },
                    onInputMethodChanged = { method ->
                        onInputMethodChanged(method)
                        inputMethodSelectorExpanded = false
                    },
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
                    inputMethodSelectorExpanded = inputMethodSelectorExpanded,
                    isSinglePane = isSinglePane,
                    onClearText = onClearText,
                    onInputMethodSelectorToggled = { value ->
                        inputMethodSelectorExpanded = value
                    },
                    onInputMethodChanged = { method ->
                        onInputMethodChanged(method)
                        inputMethodSelectorExpanded = false
                    },
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
        Column(
            modifier = Modifier.padding(innerPadding),
        ) {
            WelcomeScreen()
        }
    }
}

@Composable
private fun SearchScreenContents(
    searchScreenUiState: SearchScreenUiState,
    searchBarState: SearchBarState,
    listState: LazyListState,
    inputMethodSelectorExpanded: Boolean,
    isSinglePane: Boolean,
    onClearText: () -> Unit,
    onInputMethodSelectorToggled: (Boolean) -> Unit,
    onInputMethodChanged: (InputMethod) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    var shouldReexpand by rememberSaveable { mutableStateOf(false) }
    val scope = rememberCoroutineScope()

    LaunchedEffect(Unit) {
        if (shouldReexpand && isSinglePane) {
            searchBarState.animateToExpanded()
            shouldReexpand = false
        }
    }

    if (isSinglePane) {
        val inputField: @Composable () -> Unit =
            {
                SearchInputField(
                    searchScreenUiState,
                    searchBarState,
                    onClearText = onClearText,
                    showBackButton = searchBarState.currentValue == SearchBarValue.Expanded
                )
            }
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
                languageSelectorExpanded = inputMethodSelectorExpanded,
                isSinglePane = isSinglePane,
                onInputMethodChanged = onInputMethodChanged,
                onEntrySelected = { result ->
                    shouldReexpand = true
                    scope.launch {
                        searchBarState.animateToCollapsed()
                    }
                    onEntrySelected(result)
                },
                onLanguageSelectorToggled = onInputMethodSelectorToggled,
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
            backButton = {
                IconButton(
                    onClick = {
                        scope.launch {
                            searchBarState.animateToCollapsed()
                        }
                    }
                ) {
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                        contentDescription = "Dismiss search",
                    )
                }
            },
            inputField = { focusRequester, modifier ->
                Surface(
                    modifier = Modifier.fillMaxWidth(),
                    shape = SearchBarDefaults.dockedShape,
                    color = SearchBarDefaults.colors().containerColor,
                    tonalElevation = SearchBarDefaults.TonalElevation,
                ) {
                    SearchInputField(
                        searchScreenUiState,
                        searchBarState,
                        showBackButton = false,
                        onClearText = onClearText,
                        modifier.focusRequester(focusRequester),
                    )
                }
            },
        ) {
            SearchResults(
                searchScreenUiState = searchScreenUiState,
                listState = listState,
                languageSelectorExpanded = inputMethodSelectorExpanded,
                isSinglePane = isSinglePane,
                onInputMethodChanged = onInputMethodChanged,
                onEntrySelected = { result ->
                    onEntrySelected(result)
                },
                onLanguageSelectorToggled = onInputMethodSelectorToggled,
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
private fun SearchInputField(
    searchScreenUiState: SearchScreenUiState,
    searchBarState: SearchBarState,
    showBackButton: Boolean,
    onClearText: () -> Unit,
    modifier: Modifier = Modifier
) {
    val scope = rememberCoroutineScope()
    var inputHasFocus by remember { mutableStateOf(false) }

    val keyboardController = LocalSoftwareKeyboardController.current
    val focusManager = LocalFocusManager.current

    Row(
        horizontalArrangement = Arrangement.spacedBy(0.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        SearchBarDefaults.InputField(
            textFieldState = searchScreenUiState.textFieldState,
            searchBarState = searchBarState,
            onSearch = {
                keyboardController?.hide()
                focusManager.clearFocus()
            },
            modifier = modifier
                .onFocusChanged {
                    inputHasFocus = it.isFocused
                },
            placeholder = {
                Text(modifier = Modifier.clearAndSetSemantics {}, text = "Search")
            },
            leadingIcon = {
                if (showBackButton) {
                    IconButton(
                        onClick = {
                            scope.launch {
                                searchBarState.animateToCollapsed()
                            }
                        },
                    ) {
                        Icon(
                            imageVector = Icons.Filled.ArrowBackIosNew,
                            contentDescription = "Dismiss search",
                        )
                    }
                } else {
                    Icon(
                        Icons.Default.Search,
                        contentDescription = "Search",
                    )
                }
            },
            trailingIcon = {
                if (searchScreenUiState.textFieldState.text.isNotEmpty()) {
                    IconButton(
                        onClick = onClearText,
                    ) {
                        Icon(
                            imageVector = Icons.Filled.Close,
                            contentDescription = "Clear search text",
                        )
                    }
                }
            }
        )
    }
}

@Composable
private fun SearchResults(
    searchScreenUiState: SearchScreenUiState,
    listState: LazyListState,
    languageSelectorExpanded: Boolean,
    isSinglePane: Boolean,
    onLanguageSelectorToggled: (Boolean) -> Unit,
    onInputMethodChanged: (InputMethod) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    val searchResults = searchScreenUiState.searchResults

    val detectedInputMethod = searchScreenUiState.detectedInputMethod
    var inputMethodText by remember { mutableStateOf(searchScreenUiState.selectedInputMethod.descriptionString()) }
    var buttonHeight by remember { mutableIntStateOf(0) }

    val density = LocalDensity.current

    LaunchedEffect(detectedInputMethod) {
        if (searchScreenUiState.selectedInputMethod == InputMethod.AUTO_DETECT) {
            inputMethodText = if (detectedInputMethod != null) {
                "Detected input: ${detectedInputMethod.descriptionString()}"
            } else {
                "Auto-detect language"
            }
        }
    }

    LaunchedEffect(searchScreenUiState.selectedInputMethod) {
        if (searchScreenUiState.selectedInputMethod == InputMethod.AUTO_DETECT) {
            inputMethodText = if (detectedInputMethod != null) {
                "Detected input: ${detectedInputMethod.descriptionString()}"
            } else {
                "Auto-detect language"
            }
        } else {
            inputMethodText = searchScreenUiState.selectedInputMethod.descriptionString()
        }
    }

    LaunchedEffect(searchResults) {
        if (!searchResults.isNullOrEmpty()) {
            listState.scrollToItem(0)

            if (!isSinglePane) {
                onEntrySelected(searchResults.first())
            }
        }
    }

    Box(
        modifier = modifier
            .fillMaxSize(),
    ) {
        if (searchResults == null) {
            Unit
        } else if (searchResults.isEmpty()) {
            NoResultsScreen(Modifier.fillMaxWidth())
        } else {
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
            LanguageSelector(
                searchScreenUiState = searchScreenUiState,
                inputMethodText = inputMethodText,
                languageSelectorExpanded = languageSelectorExpanded,
                onLanguageSelectorToggled = onLanguageSelectorToggled,
                onInputMethodChanged = onInputMethodChanged
            )
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
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun LanguageSelector(
    searchScreenUiState: SearchScreenUiState,
    inputMethodText: String,
    languageSelectorExpanded: Boolean,
    onLanguageSelectorToggled: (Boolean) -> Unit,
    onInputMethodChanged: (InputMethod) -> Unit,
    modifier: Modifier = Modifier
) {
    SplitButtonLayout(
        leadingButton = {
            SplitButtonDefaults.LeadingButton(
                onClick = { onLanguageSelectorToggled(true) }
            ) {
                AnimatedContent(
                    targetState = inputMethodText,
                    label = "Input method text",
                ) { text ->
                    Text(text, maxLines = 1)
                }
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
        modifier = modifier,
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
            inputMethodOptions.forEachIndexed { idx, method ->
                SelectableDropdownMenuItem(
                    selected = searchScreenUiState.selectedInputMethod == method,
                    text = { Text(method.descriptionString()) },
                    shapes = MenuDefaults.itemShape(idx, inputMethodOptions.size),
                    onClick = { onInputMethodChanged(method) },
                    colors = MenuDefaults.selectableItemVibrantColors(),
                )
            }
        }
    }
}


@Composable
fun WelcomeScreen(modifier: Modifier = Modifier) {
    Column(
        verticalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.content_padding)),
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = modifier
    ) {
        Spacer(Modifier.weight(1f))
        Image(
            painter = painterResource(R.drawable.icon),
            contentDescription = "App icon",
            modifier = Modifier.width(150.dp)
        )
        Text(
            text = "Welcome to Jyut Dictionary",
            textAlign = TextAlign.Center,
            style = MaterialTheme.typography.displaySmall,
        )
        Text(
            text = "A Project by Aaron Tan",
            color = MaterialTheme.colorScheme.secondary,
            textAlign = TextAlign.Center,
            style = MaterialTheme.typography.bodyLarge,
        )
        Spacer(Modifier.weight(1f))
    }
}

@Composable
fun NoResultsScreen(modifier: Modifier = Modifier) {
    Column(
        verticalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.content_padding)),
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = modifier
    ) {
        Spacer(Modifier.weight(1f))
        Icon(
            imageVector = Icons.Filled.SearchOff,
            contentDescription = "No search results icon",
            modifier = Modifier
                .size(with(LocalDensity.current) { MaterialTheme.typography.displayLarge.fontSize.toDp() }),
            tint = MaterialTheme.colorScheme.onPrimaryContainer,
        )
        Text(
            text = "No search results were found.",
            color = MaterialTheme.colorScheme.secondary,
            textAlign = TextAlign.Center,
            style = MaterialTheme.typography.bodyLarge,
        )
        Spacer(Modifier.weight(1f))
    }
}

@Preview(showBackground = true)
@Composable
fun SearchScreenPreviewSinglePane() {
    CantoneseDictionaryTheme {
        SearchScreen(
            searchScreenUiState = SearchScreenUiState(),
            isSinglePane = true,
            onClearText = {},
            onInputMethodChanged = {},
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
            onClearText = {},
            onInputMethodChanged = {},
            onEntrySelected = {})
    }
}

@Preview(showBackground = true)
@Composable
fun NoResultsPreview() {
    CantoneseDictionaryTheme {
        NoResultsScreen()
    }
}
