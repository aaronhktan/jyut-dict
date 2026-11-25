package com.aaronhktan.cantonesedictionary.ui.components.entry

import android.app.Application
import android.util.Log
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aaronhktan.cantonesedictionary.MainApplication
import com.aaronhktan.cantonesedictionary.data.datastore.DictionaryRepository
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.example.Example
import com.aaronhktan.cantonesedictionary.logic.search.SQLSearch
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.flatMapLatest
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.onEach
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlinx.coroutines.yield
import kotlin.collections.listOf
import kotlin.collections.mutableListOf
import kotlin.time.Duration.Companion.milliseconds

class EntryDetailViewModel(application: Application) : AndroidViewModel(application) {
    private val _databaseManager: Deferred<DatabaseManager> =
        (application as MainApplication).container.databaseManager
    private val _repository: DictionaryRepository =
        (application as MainApplication).container.dictionaryRepository

    private val _uiState = MutableStateFlow(EntryDetailUiState())
    val uiState: StateFlow<EntryDetailUiState> = _uiState

    init {
        initializeUIState()
    }

    @OptIn(ExperimentalCoroutinesApi::class)
    private fun initializeUIState() {
        viewModelScope.launch {
            val searcher = SQLSearch(_databaseManager.await())
            _repository.selectedEntryId
                .flatMapLatest { id ->
                    flow {
                        if (id == null) {
                            emit(null)
                            return@flow
                        }

                        _uiState.update{
                            it.copy(
                                allExamples = null,
                                examplesBySource = null,
                            )
                        }

                        emit(searcher.searchByRowId(id.toString()))
                    }
                }
                .onEach { entry ->
                    if (entry == null) {
                        _uiState.update {
                            it.copy(
                                currentEntry = null,
                                allExamples = null,
                                examplesBySource = null
                            )
                        }
                        return@onEach
                    }
                    _uiState.update {
                        it.copy(
                            currentEntry = entry
                        )
                    }

                    val examples = searcher.searchExamplesByTraditional(entry.traditional)
                    val examplesBySource: MutableMap<String, MutableList<Example>> = mutableMapOf()
                    for (e in examples) {
                        val source = e.getTranslationSets().firstOrNull()?.source
                        if (!source.isNullOrEmpty()) {
                            if (examplesBySource.getOrPut(source, { mutableListOf() }).size >= 2) {
                                continue
                            }
                            examplesBySource[source]?.add(e)
                        }
                    }

                    yield()

                    _uiState.update {
                        it.copy(
                            examplesBySource = examplesBySource,
                            allExamples = examples,
                        )
                    }
                }
                .collect {
                    Unit
                }
        }
    }
}
