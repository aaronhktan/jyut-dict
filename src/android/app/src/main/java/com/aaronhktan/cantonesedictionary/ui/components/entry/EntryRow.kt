package com.aaronhktan.cantonesedictionary.ui.components.entry

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.tooling.preview.Preview
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

@Composable
fun EntryRow(entry: Entry, modifier: Modifier = Modifier) {
    Column(
        modifier = modifier
            .padding(
                horizontal = dimensionResource(R.dimen.content_padding),
                vertical = dimensionResource(R.dimen.content_padding)
            )
    ) {
        Text(
            text = entry.getCharacters(
                options = EntryCharactersOptions.PREFER_TRADITIONAL,
                useColours = true,
                colourPhoneticType = EntryColourPhoneticType.CANTONESE,
                jyutpingToneColours = defaultJyutpingToneColours,
                pinyinToneColours = defaultPinyinToneColours,
            ),
            style = MaterialTheme.typography.headlineSmall,
            overflow = TextOverflow.Ellipsis,
            maxLines = 1,
        )
        Text(
            text = entry.getPhonetic(
                options = EntryPhoneticOptions.PREFER_CANTONESE,
                cantoneseOptions = CantoneseOptions.RAW_JYUTPING,
                mandarinOptions = MandarinOptions.PRETTY_PINYIN,
            ),
            color = MaterialTheme.colorScheme.secondary,
            style = MaterialTheme.typography.bodyMedium,
            overflow = TextOverflow.Ellipsis,
            maxLines = 1,
        )
        Text(
            text = entry.getDefinitionSnippet(),
            style = MaterialTheme.typography.bodyMedium,
            overflow = TextOverflow.Ellipsis,
            maxLines = 1,
        )
    }
}

@Preview(showBackground = true)
@Composable
fun EntryRowPreview() {
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

    CantoneseDictionaryTheme {
        EntryRow(entry)
    }
}