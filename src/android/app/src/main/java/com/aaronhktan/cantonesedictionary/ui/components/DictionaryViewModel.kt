package com.aaronhktan.cantonesedictionary.ui.components

import android.util.Log
import androidx.lifecycle.ViewModel
import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.example.Example
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.update

class DictionaryViewModel : ViewModel() {
    private val _uiState = MutableStateFlow(DictionaryUiState())
    val uiState: StateFlow<DictionaryUiState> = _uiState

    init {
        initializeUIState()
    }

    private fun initializeUIState() {
        _uiState.value = DictionaryUiState()
    }

    fun updateSearchResults(query: String) {
        Log.d("test", "$query, ${query.isEmpty()}")
        if (query.isEmpty()) {
            _uiState.update {
                it.copy(
                    searchResults = listOf()
                )
            }
            return
        }

        // TODO: Remove this test data
        val entry = Entry(
            rowId = 1,
            traditional = "清遠",
            simplified = "清远",
            jyutping = "cing1 jyun5",
            pinyin = "qing1 yuan3",
            definitions = listOf(
                DefinitionsSet(
                    "CC-CANTO", "CCY", listOf(
                        Definition(
                            "Qingyuan, prefecture-level city in Guangdong",
                            label = "",
                            examples = listOf()
                        )
                    )
                )
            )
        )
        val entry2 = Entry(
            rowId = 2,
            traditional = "雞蛋仔",
            simplified = "鸡蛋仔",
            jyutping = "gai1 daan2 zai2",
            pinyin = "ji1 dan4 zai3",
            definitions = listOf(
                DefinitionsSet(
                    "粵典—words.hk",
                    "WHK",
                    listOf(
                        Definition(
                            definitionContent = "egg waffle",
                            label = "noun",
                            examples = listOf()
                        )
                    )
                )
            )
        )

        _uiState.update {
            it.copy(
                searchResults = listOf(entry, entry2)
            )
        }
    }

    fun updateDetailsScreenStates(entry: Entry) {
        _uiState.update {
            it.copy(
                currentSelectedEntry = entry
            )
        }
    }
}