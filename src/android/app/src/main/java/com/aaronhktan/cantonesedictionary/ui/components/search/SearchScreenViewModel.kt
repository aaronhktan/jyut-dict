package com.aaronhktan.cantonesedictionary.ui.components

import android.app.Application
import android.util.Log
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aaronhktan.cantonesedictionary.MainApplication
import com.aaronhktan.cantonesedictionary.data.datastore.DictionaryRepository
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.search.SQLSearch
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.flatMapLatest
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import kotlin.collections.listOf

private const val LOG_TAG = "SearchScreenViewModel"

class SearchScreenViewModel(application: Application) : AndroidViewModel(application) {
    private val _databaseManager: Deferred<DatabaseManager> =
        (application as MainApplication).container.databaseManager
    private val _repository: DictionaryRepository =
        (application as MainApplication).container.dictionaryRepository

    private val _uiState = MutableStateFlow(SearchScreenUiState())
    val uiState: StateFlow<SearchScreenUiState> = _uiState

    private val _query = MutableStateFlow("")

    init {
        initializeUIState()
    }

    @OptIn(ExperimentalCoroutinesApi::class)
    private fun initializeUIState() {
        _uiState.value = SearchScreenUiState()

        viewModelScope.launch {
            _query
                .flatMapLatest { query ->
                    flow {
                        if (query.isEmpty()) {
                            emit(listOf())
                            return@flow
                        }

                        val searcher = SQLSearch(_databaseManager.await())
                        emit(
                            withContext(Dispatchers.Default) {
                                searcher.searchJyutping(
                                    query,
                                    useFuzzyJyutping = false,
                                    unsafeFuzzyJyutping = false
                                )
                            }
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


    }

    fun updateSearchQuery(query: String) {
        Log.d(LOG_TAG, "Updating query to $query, ${query.isEmpty()}")
        _query.update {
            query
        }
    }

    fun updateSelectedEntry(entry: Entry) {
        _repository.selectEntry(entry)
    }
}