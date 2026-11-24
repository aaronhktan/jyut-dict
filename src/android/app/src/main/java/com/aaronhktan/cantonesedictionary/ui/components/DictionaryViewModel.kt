package com.aaronhktan.cantonesedictionary.ui.components

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.utils.prepareJyutpingBindValues
import com.aaronhktan.cantonesedictionary.logic.utils.segmentJyutping
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.flatMapLatest
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlin.time.Duration.Companion.milliseconds

class DictionaryViewModel : ViewModel() {
    private val _uiState = MutableStateFlow(DictionaryUiState())
    val uiState: StateFlow<DictionaryUiState> = _uiState

    private val _query = MutableStateFlow<String>("")

    init {
        initializeUIState()
    }

    @OptIn(ExperimentalCoroutinesApi::class)
    private fun initializeUIState() {
        _uiState.value = DictionaryUiState()

        viewModelScope.launch {
            _query
                .flatMapLatest { query ->
                    flow {
                        if (query.isEmpty()) {
                            emit("")
                            return@flow
                        }

                        val bindValue = prepareJyutpingBindValues(
                            jyutping = query,
                            useFuzzyJyutping = true,
                            unsafeFuzzyJyutping = false
                        )
                        Log.d("test", bindValue)
                        delay(1000.milliseconds)
                        emit(bindValue)
                    }
                }
                .collect { segmented ->
                    if (segmented.isEmpty()) {
                        _uiState.update {
                            it.copy(
                                searchResults = listOf()
                            )
                        }
                        return@collect
                    }

                    val entry = Entry(
                        rowId = 1337,
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
                            searchResults = List(200) { index ->
                                if (index % 2 == 1) {
                                    Entry(
                                        rowId = index,
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
                                } else {
                                    Entry(
                                        rowId = index,
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
                                }
                            }
                        )
                    }
                }
        }
    }

    fun updateSearchQuery(query: String) {
        Log.d("test", "Updating query to $query, ${query.isEmpty()}")
        if (query.isEmpty()) {
            _uiState.update {
                it.copy(
                    searchResults = listOf()
                )
            }
        } else {
            _query.update {
                query
            }
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