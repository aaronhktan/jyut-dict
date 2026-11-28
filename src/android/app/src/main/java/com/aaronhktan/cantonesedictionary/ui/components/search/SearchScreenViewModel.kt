package com.aaronhktan.cantonesedictionary.ui.components

import android.app.Application
import android.util.Log
import androidx.compose.foundation.text.input.TextFieldState
import androidx.compose.foundation.text.input.clearText
import androidx.compose.runtime.snapshotFlow
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import com.aaronhktan.cantonesedictionary.MainApplication
import com.aaronhktan.cantonesedictionary.data.datastore.DictionaryRepository
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.search.SQLSearch
import com.aaronhktan.cantonesedictionary.logic.utils.InputMethod
import com.aaronhktan.cantonesedictionary.logic.utils.InputMethodDetector
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.async
import kotlinx.coroutines.currentCoroutineContext
import kotlinx.coroutines.delay
import kotlinx.coroutines.ensureActive
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.flow.combine
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.flow.flatMapLatest
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import kotlin.collections.listOf
import kotlin.time.Duration.Companion.milliseconds

private const val LOG_TAG = "SearchScreenViewModel"

class SearchScreenViewModel(application: Application) : AndroidViewModel(application) {
    private val _databaseManager: Deferred<DatabaseManager> =
        (application as MainApplication).container.databaseManager
    private val _repository: DictionaryRepository =
        (application as MainApplication).container.dictionaryRepository

    private val _textFieldState = TextFieldState()
    private val _uiState = MutableStateFlow(SearchScreenUiState(_textFieldState))
    val uiState: StateFlow<SearchScreenUiState> = _uiState

    private val _query = MutableStateFlow("")
    private val _inputMethod = MutableStateFlow(InputMethod.AUTO_DETECT)

    init {
        initializeUIState()
    }

    @OptIn(ExperimentalCoroutinesApi::class)
    private fun initializeUIState() {
        viewModelScope.launch {
            snapshotFlow { _textFieldState.text.toString() }
                .distinctUntilChanged()
                .collectLatest { updateSearchQuery(it) }
        }

        viewModelScope.launch {
            _query.combine(_inputMethod) { query, method ->
                query to method
            }
                .flatMapLatest { (query, method) ->
                    flow {
                        if (query.isEmpty()) {
                            _uiState.update {
                                it.copy(
                                    detectedInputMethod = null,
                                    searchResults = null,
                                )
                            }
                            return@flow
                        }

                        val result = triggerSearch(query, method)
                        if (result.isEmpty()) {
                            delay(300.milliseconds)
                            currentCoroutineContext().ensureActive()
                        }
                        emit(result)
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

    private suspend fun triggerSearch(query: String, method: InputMethod): List<Entry> {
        val searcher = SQLSearch(_databaseManager.await())

        return withContext(Dispatchers.Default) {
            when (method) {
                InputMethod.TRADITIONAL -> {
                    return@withContext searcher.searchTraditional(query)
                }

                InputMethod.SIMPLIFIED -> {
                    return@withContext searcher.searchSimplified(query)
                }

                InputMethod.JYUTPING -> {
                    return@withContext searcher.searchJyutping(
                        query,
                        useFuzzyJyutping = false,
                        unsafeFuzzyJyutping = false // TODO: Make this respect settings
                    )
                }

                InputMethod.FUZZY_JYUTPING -> {
                    return@withContext searcher.searchJyutping(
                        query,
                        useFuzzyJyutping = true,
                        unsafeFuzzyJyutping = false // TODO: Make this respect settings
                    )
                }

                InputMethod.PINYIN -> {
                    return@withContext searcher.searchPinyin(
                        query,
                        useFuzzyPinyin = false
                    )
                }

                InputMethod.FUZZY_PINYIN -> {
                    return@withContext searcher.searchPinyin(
                        query,
                        useFuzzyPinyin = true
                    )
                }

                InputMethod.ENGLISH -> {
                    return@withContext searcher.searchEnglish(searchTerm = query)
                }

                InputMethod.AUTO_DETECT -> {
                    val detector = InputMethodDetector(searcher)
                    if (detector.hasSimplified(query)) {
                        _uiState.update {
                            it.copy(
                                detectedInputMethod = InputMethod.SIMPLIFIED
                            )
                        }
                        return@withContext searcher.searchSimplified(query)
                    } else if (detector.hasTraditional(query) || detector.hasHanCharacters(query)) {
                        _uiState.update {
                            it.copy(
                                detectedInputMethod = InputMethod.TRADITIONAL
                            )
                        }
                        return@withContext searcher.searchTraditional(query)
                    }

                    val isJyutping = async {
                        detector.hasJyutping(
                            text = query,
                            unsafeFuzzyJyutping = false // TODO: update this to match settings
                        )
                    }
                    val isPinyin = async {
                        detector.hasPinyin(query)
                    }

                    when {
                        isJyutping.await() != null -> {
                            _uiState.update {
                                it.copy(
                                    detectedInputMethod = isJyutping.await()
                                )
                            }
                            return@withContext searcher.searchJyutping(
                                searchTerm = query,
                                useFuzzyJyutping = isJyutping.await() == InputMethod.FUZZY_JYUTPING,
                                unsafeFuzzyJyutping = false // TODO: update this to match settings
                            )
                        }

                        isPinyin.await() != null -> {
                            _uiState.update {
                                it.copy(
                                    detectedInputMethod = isPinyin.await()
                                )
                            }
                            return@withContext searcher.searchPinyin(
                                searchTerm = query,
                                useFuzzyPinyin = isPinyin.await() == InputMethod.FUZZY_PINYIN,
                            )
                        }
                    }

                    _uiState.update {
                        it.copy(
                            detectedInputMethod = InputMethod.ENGLISH
                        )
                    }
                    return@withContext searcher.searchEnglish(query)
                }

                InputMethod.NONE -> {
                    return@withContext searcher.searchEnglish(query)
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

    fun clearSearchInputText() {
        _textFieldState.clearText()
    }

    fun updateSelectedEntry(entry: Entry) {
        _repository.selectEntry(entry)
    }

    fun updateInputMethod(method: InputMethod) {
        Log.d(LOG_TAG, "Updating input method to ${method.name}")
        _inputMethod.update {
            method
        }
        _uiState.update {
            it.copy(
                selectedInputMethod = method
            )
        }
    }
}