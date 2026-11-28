package com.aaronhktan.cantonesedictionary.ui.components

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.adaptive.ExperimentalMaterial3AdaptiveApi
import androidx.compose.material3.adaptive.layout.AnimatedPane
import androidx.compose.material3.adaptive.layout.ListDetailPaneScaffoldRole
import androidx.compose.material3.adaptive.layout.PaneAdaptedValue
import androidx.compose.material3.adaptive.navigation.NavigableListDetailPaneScaffold
import androidx.compose.material3.adaptive.navigation.rememberListDetailPaneScaffoldNavigator
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.painter.Painter
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import androidx.lifecycle.viewmodel.compose.viewModel
import com.aaronhktan.cantonesedictionary.MainApplication
import com.aaronhktan.cantonesedictionary.R
import com.aaronhktan.cantonesedictionary.data.datastore.DictionaryRepository
import com.aaronhktan.cantonesedictionary.ui.components.entry.EntryDetail
import com.aaronhktan.cantonesedictionary.ui.components.entry.EntryDetailViewModel
import com.aaronhktan.cantonesedictionary.ui.components.search.SearchScreen
import kotlinx.coroutines.launch

@OptIn(ExperimentalMaterial3AdaptiveApi::class)
@Composable
fun HomeScreen(
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    val application = context.applicationContext as MainApplication
    val repository: DictionaryRepository =
        application.container.dictionaryRepository

    val searchScreenViewModel: SearchScreenViewModel = viewModel()
    val searchScreenUiState = searchScreenViewModel.uiState.collectAsStateWithLifecycle().value

    val entryDetailViewModel: EntryDetailViewModel = viewModel()
    val entryDetailUiState = entryDetailViewModel.uiState.collectAsStateWithLifecycle().value

    val coroutineScope = rememberCoroutineScope()
    val scaffoldNavigator = rememberListDetailPaneScaffoldNavigator<Int>()

    LaunchedEffect(scaffoldNavigator.scaffoldValue) {
        val isDetailVisible =
            scaffoldNavigator.scaffoldValue[ListDetailPaneScaffoldRole.Detail] ==
                    PaneAdaptedValue.Expanded
        if (!isDetailVisible) {
            repository.selectEntry(null)
        }
    }

    NavigableListDetailPaneScaffold(
        navigator = scaffoldNavigator,
        listPane = {
            AnimatedPane(
                modifier = Modifier.preferredWidth(
                    dimensionResource(R.dimen.search_pane_default_width)
                )
            ) {
                SearchScreen(
                    searchScreenUiState = searchScreenUiState,
                    isSinglePane = scaffoldNavigator.scaffoldDirective.maxHorizontalPartitions == 1,
                    onClearText = {
                        searchScreenViewModel.clearSearchInputText()
                    },
                    onInputMethodChanged = { method ->
                        searchScreenViewModel.updateInputMethod(method)
                    },
                    onEntrySelected = { entry ->
                        searchScreenViewModel.updateSelectedEntry(entry)
                        coroutineScope.launch {
                            scaffoldNavigator.navigateTo(
                                pane = ListDetailPaneScaffoldRole.Detail,
                                contentKey = entry.id
                            )
                        }
                    },
                    modifier = modifier
                )
            }
        },
        detailPane = {
            AnimatedPane() {
                EntryDetail(
                    entryDetailUiState = entryDetailUiState,
                    showBackButton = scaffoldNavigator.scaffoldDirective.maxHorizontalPartitions == 1,
                    modifier = modifier,
                )
            }
        },
    )
}
