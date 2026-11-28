package com.aaronhktan.cantonesedictionary.ui.components.entry

import android.content.ClipData
import androidx.activity.ComponentActivity
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.ExitTransition
import androidx.compose.animation.expandVertically
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.animation.shrinkVertically
import androidx.compose.animation.slideInVertically
import androidx.compose.animation.slideOutVertically
import androidx.compose.foundation.combinedClickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Grid
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.VolumeUp
import androidx.compose.material.icons.filled.ArrowBackIosNew
import androidx.compose.material.icons.filled.ContentCopy
import androidx.compose.material.icons.filled.Favorite
import androidx.compose.material3.FilledIconButton
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.IconButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.hapticfeedback.HapticFeedbackType
import androidx.compose.ui.platform.ClipEntry
import androidx.compose.ui.platform.LocalClipboard
import androidx.compose.ui.platform.LocalClipboardManager
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.platform.LocalHapticFeedback
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import com.aaronhktan.cantonesedictionary.R
import com.aaronhktan.cantonesedictionary.data.datastore.defaultJyutpingToneColours
import com.aaronhktan.cantonesedictionary.data.datastore.defaultPinyinToneColours
import com.aaronhktan.cantonesedictionary.logic.entry.CantoneseOptions
import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.entry.EntryCharactersOptions
import com.aaronhktan.cantonesedictionary.logic.entry.EntryColourPhoneticType
import com.aaronhktan.cantonesedictionary.logic.entry.EntryPhoneticOptions
import com.aaronhktan.cantonesedictionary.logic.entry.MandarinOptions
import com.aaronhktan.cantonesedictionary.ui.theme.CantoneseDictionaryTheme
import kotlinx.coroutines.launch

@Composable
fun EntryDetail(
    entryDetailUiState: EntryDetailUiState,
    showBackButton: Boolean,
    modifier: Modifier = Modifier,
) {
    val scope = rememberCoroutineScope()

    val entry = entryDetailUiState.currentEntry

    val context = LocalContext.current
    val activity = context as? ComponentActivity
    val clipboardManager = LocalClipboard.current
    val haptics = LocalHapticFeedback.current

    if (entry != null) {
        Scaffold(
            modifier = modifier,
            topBar = {
                TopAppBar(
                    title = {},
                    navigationIcon = {
                        if (showBackButton) {
                            IconButton(onClick = { activity?.onBackPressedDispatcher?.onBackPressed() }) {
                                Icon(
                                    imageVector = Icons.Filled.ArrowBackIosNew,
                                    contentDescription = "Return to search results",
                                )
                            }
                        }
                    },
                    actions = {
                        IconButton(
                            onClick = {},
                            modifier = Modifier.combinedClickable(
                                onClick = {
                                    scope.launch {
                                        clipboardManager.setClipEntry(
                                            ClipEntry(
                                                ClipData.newPlainText("", entry.traditional)
                                            )
                                        )
                                    }
                                },
                                onLongClick = {
                                    haptics.performHapticFeedback(HapticFeedbackType.LongPress)
                                    // TODO: Show menu for copying
                                }
                            )
                        ) {
                            Icon(
                                imageVector = Icons.Filled.ContentCopy,
                                contentDescription = "Save entry",
                            )
                        }
                        IconButton(onClick = {
                            // TODO: Implement favouriting an entry
                        }) {
                            Icon(
                                imageVector = Icons.Filled.Favorite,
                                contentDescription = "Save entry",
                            )
                        }
                    },
                    windowInsets = WindowInsets(0, 0, 0 , 0),
                )
            },
            contentWindowInsets = WindowInsets(0, 0, 0, 0)
        ) { padding ->
            LazyColumn(
                modifier = Modifier
                    .fillMaxSize(),
                contentPadding = PaddingValues(
                    start = dimensionResource(R.dimen.content_padding),
                    top = padding.calculateTopPadding(),
                    end = dimensionResource(R.dimen.content_padding),
                    bottom = padding.calculateBottomPadding() + dimensionResource(R.dimen.content_padding)
                ),
                verticalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.column_spacing)),
            ) {
                item {
                    Text(
                        text = entry.getCharacters(
                            options = EntryCharactersOptions.PREFER_TRADITIONAL,
                            useColours = true,
                            colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                            jyutpingToneColours = defaultJyutpingToneColours,
                            pinyinToneColours = defaultPinyinToneColours,
                        ),
                        style = MaterialTheme.typography.displayMedium,
                    )
                    Column(verticalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.column_spacing))) {
                        CantonesePronunciationView(entry = entry)
                        MandarinPronunciationView(entry = entry)
                    }
                }

                val definitionsSets = entry.getDefinitionsSets() ?: listOf()
                items(
                    definitionsSets,
                    key = { definitionSet -> definitionSet.hashCode() }) { definitionSet ->
                    DefinitionCard(definitionSet)
                }

                val examplesBySource = entryDetailUiState.examplesBySource.orEmpty()
                item {
                    val density = LocalDensity.current
                    AnimatedVisibility(
                        visible = examplesBySource.isNotEmpty(),
                        enter = slideInVertically {
                            with(density) { -40.dp.roundToPx() }
                        } + expandVertically(
                            expandFrom = Alignment.Top
                        ) + fadeIn(
                            initialAlpha = 0.3f
                        ),
                        exit = ExitTransition.None
                    ) {
                        Column(
                            verticalArrangement = Arrangement.spacedBy(
                                dimensionResource(R.dimen.column_spacing)
                            )
                        ) {
                            examplesBySource.forEach { (source, examples) ->
                                ExampleCard(source, examples)
                            }
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun CantonesePronunciationView(entry: Entry, modifier: Modifier = Modifier) {
    Grid(
        config = {
            rowGap(4.dp)
            columnGap(8.dp)
        },
        modifier = modifier,
    ) {
        FilledIconButton(
            onClick = {},
            shape = IconButtonDefaults.extraSmallSquareShape,
            modifier = Modifier
                .gridItem(row = 1, column = 1, rowSpan = 3, alignment = Alignment.TopStart)
                .size(IconButtonDefaults.extraSmallContainerSize())
                .fillMaxSize(),
        ) {
            Icon(Icons.AutoMirrored.Filled.VolumeUp, contentDescription = "Search")
        }
        Row(
            horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
            modifier = Modifier
                .gridItem(row = 1, column = 2, alignment = Alignment.CenterStart)
        ) {
            Text(
                text = "Jyutping",
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = entry.getPhonetic(
                    EntryPhoneticOptions.ONLY_CANTONESE,
                    CantoneseOptions.RAW_JYUTPING,
                    MandarinOptions.RAW_PINYIN
                ),
                style = MaterialTheme.typography.bodyMedium,
            )
        }
        Row(
            horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
            modifier = Modifier
                .gridItem(row = 2, column = 2, alignment = Alignment.CenterStart)
        ) {
            Text(
                text = "Yale",
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = entry.getPhonetic(
                    EntryPhoneticOptions.ONLY_CANTONESE,
                    CantoneseOptions.PRETTY_YALE,
                    MandarinOptions.RAW_PINYIN
                ),
                style = MaterialTheme.typography.bodyMedium,
            )
        }
        Row(
            horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
            modifier = Modifier
                .gridItem(row = 3, column = 2, alignment = Alignment.CenterStart)
        ) {
            Text(
                text = "Cantonese IPA",
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = entry.getPhonetic(
                    EntryPhoneticOptions.ONLY_CANTONESE,
                    CantoneseOptions.CANTONESE_IPA,
                    MandarinOptions.RAW_PINYIN
                ),
                style = MaterialTheme.typography.bodyMedium,
            )
        }
    }
}

@Composable
fun MandarinPronunciationView(entry: Entry, modifier: Modifier = Modifier) {
    Grid(
        config = {
            rowGap(4.dp)
            columnGap(8.dp)
        },
        modifier = modifier,
    ) {
        FilledIconButton(
            onClick = {},
            shape = IconButtonDefaults.extraSmallSquareShape,
            modifier = Modifier
                .gridItem(row = 1, column = 1, rowSpan = 3, alignment = Alignment.TopStart)
                .size(IconButtonDefaults.extraSmallContainerSize())
                .fillMaxSize(),
        ) {
            Icon(Icons.AutoMirrored.Filled.VolumeUp, contentDescription = "Search")
        }
        Row(
            horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
            modifier = Modifier
                .gridItem(row = 1, column = 2, alignment = Alignment.CenterStart)
        ) {
            Text(
                text = "Pinyin",
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = entry.getPhonetic(
                    EntryPhoneticOptions.ONLY_MANDARIN,
                    CantoneseOptions.RAW_JYUTPING,
                    MandarinOptions.PRETTY_PINYIN
                ),
                style = MaterialTheme.typography.bodyMedium,
            )
        }
        Row(
            horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
            modifier = Modifier
                .gridItem(row = 2, column = 2, alignment = Alignment.CenterStart)
        ) {
            Text(
                text = "Pinyin with digits",
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = entry.getPhonetic(
                    EntryPhoneticOptions.ONLY_MANDARIN,
                    CantoneseOptions.RAW_JYUTPING,
                    MandarinOptions.NUMBERED_PINYIN,
                ),
                style = MaterialTheme.typography.bodyMedium,
            )
        }
        Row(
            horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
            modifier = Modifier
                .gridItem(row = 3, column = 2, alignment = Alignment.CenterStart)
        ) {
            Text(
                text = "Zhuyin",
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = entry.getPhonetic(
                    EntryPhoneticOptions.ONLY_MANDARIN,
                    CantoneseOptions.RAW_JYUTPING,
                    MandarinOptions.ZHUYIN
                ),
                style = MaterialTheme.typography.bodyMedium,
            )
        }
        Row(
            horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
            modifier = Modifier
                .gridItem(row = 4, column = 2, alignment = Alignment.CenterStart)
        ) {
            Text(
                text = "Mandarin IPA",
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = entry.getPhonetic(
                    EntryPhoneticOptions.ONLY_MANDARIN,
                    CantoneseOptions.RAW_JYUTPING,
                    MandarinOptions.MANDARIN_IPA
                ),
                style = MaterialTheme.typography.bodyMedium,
            )
        }
    }
}

@Preview(showBackground = true)
@Composable
fun EntryDetailPreview() {
    val definition = Definition(
        "Qingyuan, prefecture-level city in Guangdong",
        label = "",
        examples = listOf()
    )
    val definitionsSet = DefinitionsSet(
        "CC-CANTO", "CCY", listOf(
            definition
        )
    )
    val entry = Entry(
        rowId = 1,
        traditional = "清遠",
        simplified = "清远",
        jyutping = "cing1 jyun5",
        pinyin = "qing1 yuan3",
        definitions = listOf(
            definitionsSet
        )
    )

    var entryDetailUiState = EntryDetailUiState(currentEntry = entry)
    CantoneseDictionaryTheme {
        EntryDetail(entryDetailUiState = entryDetailUiState, showBackButton = true)
    }
}