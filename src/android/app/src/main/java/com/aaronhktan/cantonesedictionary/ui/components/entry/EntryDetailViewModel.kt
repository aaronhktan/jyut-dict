package com.aaronhktan.cantonesedictionary.ui.components.entry

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aaronhktan.cantonesedictionary.MainApplication
import com.aaronhktan.cantonesedictionary.data.datastore.DictionaryRepository
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.search.SQLSearch
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.flatMapLatest
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

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
            _repository.selectedEntryId
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
                            currentEntry = result
                        )
                    }
                }
        }
    }
}
