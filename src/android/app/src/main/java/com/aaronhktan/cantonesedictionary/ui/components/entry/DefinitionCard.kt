package com.aaronhktan.cantonesedictionary.ui.components.entry

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Grid
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeContentPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.aaronhktan.cantonesedictionary.R
import com.aaronhktan.cantonesedictionary.logic.entry.CantoneseOptions
import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.MandarinOptions
import com.aaronhktan.cantonesedictionary.logic.example.Example
import com.aaronhktan.cantonesedictionary.logic.example.Translation
import com.aaronhktan.cantonesedictionary.logic.example.TranslationSet
import com.aaronhktan.cantonesedictionary.ui.theme.CantoneseDictionaryTheme

@Composable
fun DefinitionCard(definitionsSet: DefinitionsSet, modifier: Modifier = Modifier) {
    Card(modifier = modifier) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
        ) {
            Text(
                text = definitionsSet.source,
                modifier = Modifier
                    .background(
                        color = MaterialTheme.colorScheme.surfaceContainerHigh,
                        shape = RoundedCornerShape(topEnd = 0.dp, bottomEnd = 0.dp)
                    )
                    .fillMaxWidth()
                    .padding(
                        dimensionResource(R.dimen.content_padding),
                    ),
                color = MaterialTheme.colorScheme.onSurface,
                style = MaterialTheme.typography.titleSmall,
            )

            Surface(
                color = MaterialTheme.colorScheme.surfaceContainer,
                modifier = Modifier
                    .background(
                        // This has no effect but must be passed in anyway to prevent a weird white box
                        color = MaterialTheme.colorScheme.surfaceContainer,
                        shape = RoundedCornerShape(topEnd = 0.dp, bottomEnd = 0.dp)
                    )
                    .fillMaxWidth()
                    .padding(
                        dimensionResource(R.dimen.content_padding),
                    ),
            ) {
                Column(
                    verticalArrangement = Arrangement.spacedBy(
                        dimensionResource(R.dimen.column_spacing)
                    ),
                ) {
                    definitionsSet.definitions.forEachIndexed { index, definition ->
                        Row {
                            Text(
                                text = (index + 1).toString(),
                                modifier = Modifier
                                    .width(dimensionResource(R.dimen.number_width)),
                                color = MaterialTheme.colorScheme.onSurface,
                                style = MaterialTheme.typography.bodyMedium,
                            )

                            DefinitionView(definition)
                        }
                    }
                }
            }

        }
    }
}

@Composable
fun DefinitionView(definition: Definition, modifier: Modifier = Modifier) {
    Column(modifier = modifier) {
        Text(
            text = definition.definitionContent,
            color = MaterialTheme.colorScheme.onSurface,
            style = MaterialTheme.typography.bodyMedium,
        )

        definition.examples.forEach { example ->
            ExampleView(example)
        }
    }
}

@Composable
fun ExampleView(example: Example, modifier: Modifier = Modifier) {
    Row(
        horizontalArrangement = Arrangement.spacedBy(dimensionResource(R.dimen.row_spacing)),
        modifier = modifier,
    ) {
        Text(
            text = "◦",
            modifier = Modifier
                .padding(start = dimensionResource(R.dimen.horizontal_padding))
                .width(dimensionResource(R.dimen.bullet_width)),
            color = MaterialTheme.colorScheme.secondary,
            style = MaterialTheme.typography.bodyMedium,
        )
        Column {
            Text(
                text = example.traditional,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = example.simplified,
                style = MaterialTheme.typography.bodyMedium,
            )
            Text(
                text = example.getCantonesePhonetic(CantoneseOptions.RAW_JYUTPING),
                color = MaterialTheme.colorScheme.secondary,
                style = MaterialTheme.typography.bodyMedium,
            )
//            Text(
//                text = example.getMandarinPhonetic(MandarinOptions.RAW_PINYIN),
//                style = MaterialTheme.typography.bodyMedium,
//            )

            example.getTranslationSets().forEach { translationSet ->
                val first = translationSet.translations.firstOrNull()
                if (first != null) {
                    Text(
                        text = first.content,
                        style = MaterialTheme.typography.bodyMedium,
                    )
                }
            }
        }
    }
}

@Preview(showBackground = true)
@Composable
fun DefinitionCardPreview() {
    val translation = Translation(
        content = "There have been days of heavy raining in Tsingyün (A city in Guangdong), and the situation of flooding is pressing.",
        language = "eng",
        directTarget = true
    )

    val translationSet = TranslationSet(
        source = "粵典—words.hk",
        translations = listOf(translation)
    )

    val example = Example(
        sourceLanguage = "yue",
        traditional = "清遠 呢幾日有連日嘅大雨，汛情緊急。",
        simplified = "清远 呢几日有连日嘅大雨，汛情紧急。",
        jyutping = "cing1 jyun5 nei1 gei2 jat6 jau5 lin4 jat6 ge3 daai6 jyu5, seon3 cing4 gan2 gap1",
        pinyin = "",
        translations = listOf(translationSet)
    )

    val definition = Definition(
        "Qingyuan, prefecture-level city in Guangdong",
        label = "",
        examples = listOf(example)
    )
    val definitionsSet = DefinitionsSet(
        "CC-CANTO", "CCY", listOf(
            definition
        )
    )

    CantoneseDictionaryTheme {
        DefinitionCard(definitionsSet = definitionsSet)
    }
}