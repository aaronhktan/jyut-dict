package com.aaronhktan.cantonesedictionary

import android.app.Application
import android.content.Context
import androidx.lifecycle.viewModelScope
import com.aaronhktan.cantonesedictionary.data.datastore.DictionaryRepository
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.async

class AppContainer(context: Context) {
    private val applicationScope = CoroutineScope(SupervisorJob() + Dispatchers.IO)

    val databaseManager: Deferred<DatabaseManager> =
        applicationScope.async(Dispatchers.IO) {
            DatabaseManager.getInstance(context)
        }

    val dictionaryRepository by lazy {
        DictionaryRepository()
    }
}

class MainApplication : Application() {
    lateinit var container: AppContainer
        private set

    override fun onCreate() {
        super.onCreate()
        container = AppContainer(this)
    }
}