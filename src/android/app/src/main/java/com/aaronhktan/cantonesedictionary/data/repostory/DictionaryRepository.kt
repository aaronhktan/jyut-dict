package com.aaronhktan.cantonesedictionary.data.datastore

import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow

class DictionaryRepository {
    private val _selectedEntryID = MutableStateFlow<Int?>(null)
    val selectedEntryId = _selectedEntryID.asStateFlow()

    fun selectEntry(entry: Entry?) {
        _selectedEntryID.value = entry?.id
    }
}