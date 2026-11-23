package com.aaronhktan.cantonesedictionary.ui.components

import androidx.compose.animation.core.Spring
import androidx.compose.animation.core.spring
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.animation.slideInHorizontally
import androidx.compose.animation.slideOutHorizontally
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.adaptive.ExperimentalMaterial3AdaptiveApi
import androidx.compose.material3.adaptive.layout.AnimatedPane
import androidx.compose.material3.adaptive.layout.ListDetailPaneScaffoldRole
import androidx.compose.material3.adaptive.navigation.NavigableListDetailPaneScaffold
import androidx.compose.material3.adaptive.navigation.rememberListDetailPaneScaffoldNavigator
import androidx.compose.runtime.Composable
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.dimensionResource
import com.aaronhktan.cantonesedictionary.R
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.ui.components.entry.EntryDetail
import com.aaronhktan.cantonesedictionary.ui.components.search.SearchScreen
import kotlinx.coroutines.launch

@OptIn(ExperimentalMaterial3AdaptiveApi::class)
@Composable
fun HomeScreen(
    dictionaryUiState: DictionaryUiState,
    onQueryChanged: (String) -> Unit,
    onEntrySelected: (Entry) -> Unit,
    modifier: Modifier = Modifier
) {
    val coroutineScope = rememberCoroutineScope()
    val scaffoldNavigator = rememberListDetailPaneScaffoldNavigator<Int>()

    NavigableListDetailPaneScaffold(
        navigator = scaffoldNavigator,
        listPane = {
            AnimatedPane(
                modifier = Modifier.preferredWidth(
                    dimensionResource(R.dimen.search_pane_default_width)
                )
            ) {
                SearchScreen(
                    dictionaryUiState = dictionaryUiState,
                    isSinglePane = scaffoldNavigator.scaffoldDirective.maxHorizontalPartitions == 1,
                    onQueryChanged = onQueryChanged,
                    onEntrySelected = { entry ->
                        onEntrySelected(entry)
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
                    dictionaryUiState = dictionaryUiState,
                    modifier = modifier
                        .padding(dimensionResource(R.dimen.content_padding))
                )
            }
        },
    )
}
