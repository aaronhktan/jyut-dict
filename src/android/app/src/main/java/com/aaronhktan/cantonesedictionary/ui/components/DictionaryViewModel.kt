package com.aaronhktan.cantonesedictionary.ui.components

import android.app.Application
import android.util.Log
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.search.SQLSearch
import com.aaronhktan.cantonesedictionary.logic.utils.prepareJyutpingBindValues
import com.aaronhktan.cantonesedictionary.logic.utils.segmentJyutping
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.async
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.flatMapLatest
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlin.collections.listOf
import kotlin.time.Duration.Companion.milliseconds

class DictionaryViewModel(application: Application) : AndroidViewModel(application) {
    private val _uiState = MutableStateFlow(DictionaryUiState())
    val uiState: StateFlow<DictionaryUiState> = _uiState

    private val _query = MutableStateFlow("")
    private val _selectedEntryId = MutableStateFlow<Int?>(null)
    private val _databaseManager: Deferred<DatabaseManager> = viewModelScope.async(Dispatchers.IO) {
        DatabaseManager.getInstance(getApplication<Application>().applicationContext)
    }

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
                            emit(listOf<Entry>())
                            return@flow
                        }

                        val searcher = SQLSearch(_databaseManager.await())
                        emit(
                            searcher.searchJyutping(
                                query,
                                useFuzzyJyutping = true,
                                unsafeFuzzyJyutping = false
                            )
                        )
                    }
                }
                .collect { result ->
                    _uiState.update {
                        it.copy(
                            searchResults = result
                        )
                    }
                }
        }

        viewModelScope.launch {
            _selectedEntryId
                .flatMapLatest { id ->
                    flow {
                        if (id == null) {
                            emit(null)
                            return@flow
                        }

                        val searcher = SQLSearch(_databaseManager.await())
                        emit(searcher.searchByRowId(id.toString()))
                    }
                }
                .collect { result ->
                    if (result == null) {
                        return@collect
                    }
                    _uiState.update {
                        it.copy(
                            currentSelectedEntry = result
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
        _selectedEntryId.value = entry.id
    }
}