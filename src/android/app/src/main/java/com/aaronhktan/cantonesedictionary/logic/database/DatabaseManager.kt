package com.aaronhktan.cantonesedictionary.logic.database

import android.content.Context
import android.util.Log
import androidx.sqlite.SQLiteConnection
import androidx.sqlite.driver.bundled.BundledSQLiteDriver
import androidx.sqlite.execSQL
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.async
import kotlinx.coroutines.awaitAll
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import kotlinx.coroutines.withContext
import java.io.File
import java.io.IOException

private const val LOG_TAG = "DatabaseManager"

class DatabaseManager {
    private val _dbFileName = "dict.db"
    private lateinit var _localFilePath: String
    private val _userFileName = "user.db"
    private lateinit var _localUserFilePath: String

    private lateinit var _writer: SQLiteConnection
    private val mutex = Mutex()
    private lateinit var _readers: List<SQLiteConnection>
    private lateinit var _readerChannel: Channel<SQLiteConnection>

    private constructor() {}

    private suspend fun copyDatabaseFromBundle(context: Context) = withContext(Dispatchers.IO) {
        val localFile = File(context.filesDir, "dictionaries/$_dbFileName")
        _localFilePath = localFile.absolutePath
        if (localFile.exists()) {
            return@withContext
        }

        localFile.parentFile?.mkdirs()
        val tmp = File(localFile.parentFile, "$_dbFileName.tmp")

        try {
            context.assets.open(_dbFileName).use { inputStream ->
                tmp.outputStream().use { tmpOutputStream ->
                    inputStream.copyTo(tmpOutputStream)
                }
            }
            if (!tmp.renameTo(localFile)) {
                throw IOException("Failed to rename $tmp to $_dbFileName!")
            }
        } finally {
            tmp.delete()
        }
    }

    private suspend fun copyUserDatabaseFromBundle(context: Context) = withContext(Dispatchers.IO) {
        val localFile = File(context.filesDir, "dictionaries/$_userFileName")
        _localUserFilePath = localFile.absolutePath
        if (localFile.exists()) {
            return@withContext
        }

        localFile.parentFile?.mkdirs()
        val tmp = File(localFile.parentFile, "$_userFileName.tmp")

        try {
            context.assets.open(_userFileName).use { inputStream ->
                tmp.outputStream().use { tmpOutputStream ->
                    inputStream.copyTo(tmpOutputStream)
                }
            }
            if (!tmp.renameTo(localFile)) {
                throw IOException("Failed to rename $tmp to $_userFileName!")
            }
        } finally {
            tmp.delete()
        }
    }

    private fun setupDatabase() {
        val driver = BundledSQLiteDriver()

        _readerChannel = Channel(Runtime.getRuntime().availableProcessors())

        _writer = driver.open(_localFilePath).apply {
            prepare("ATTACH DATABASE ? AS user").use { statement ->
                statement.bindText(1, _localUserFilePath)
                statement.step()
            }
            execSQL("PRAGMA cache_size = -64000")
            execSQL("PRAGMA mmap_size = 268435456")
            execSQL("PRAGMA temp_store = MEMORY")
            execSQL("PRAGMA synchronous = NORMAL")
            execSQL("PRAGMA journal_mode = WAL")
            execSQL("PRAGMA user.journal_mode = WAL")
        }

        _readers = List(Runtime.getRuntime().availableProcessors()) {
            driver.open(_localFilePath).apply {
                prepare("ATTACH DATABASE ? AS user").use { statement ->
                    statement.bindText(1, _localUserFilePath)
                    statement.step()
                }
                execSQL("PRAGMA cache_size = -64000")
                execSQL("PRAGMA mmap_size = 268435456")
                execSQL("PRAGMA temp_store = MEMORY")
                execSQL("PRAGMA synchronous = NORMAL")
                execSQL("PRAGMA query_only = ON")
            }
        }
        _readers.forEach { _readerChannel.trySend(it) }
    }

    suspend fun <T> read(lambda: suspend (SQLiteConnection) -> T): T {
        Log.i(LOG_TAG, "Starting read...")
        val reader = _readerChannel.receive()
        Log.i(LOG_TAG, "Got reader..")
        try {
            return withContext(Dispatchers.IO) {
                reader.execSQL("BEGIN")
                try {
                    lambda(reader)
                } finally {
                    runCatching {
                        reader.execSQL("ROLLBACK")
                    }
                }
            }
        } finally {
            _readerChannel.trySend(reader)
        }
    }

    suspend fun <T> write(lambda: suspend (SQLiteConnection) -> T): T = mutex.withLock {
        withContext(Dispatchers.IO) {
            _writer.execSQL("BEGIN")
            try {
                lambda(_writer).also { _writer.execSQL("COMMIT") }
            } catch (t: Throwable) {
                runCatching {
                    _writer.execSQL("ROLLBACK")
                }
                throw t
            }
        }
    }

    fun close() {
        _readerChannel.close()
        for (r in _readers) {
            runCatching {
                r.close()
            }
        }
        runCatching { _writer.close() }
    }

    companion object {
        @Volatile
        private var _instance: DatabaseManager? = null
        private val _mutex = Mutex()

        suspend fun getInstance(context: Context): DatabaseManager {
            _instance?.let { return it }
            return _mutex.withLock {
                _instance ?: create(context.applicationContext).also { _instance = it }
            }
        }

        private suspend fun create(context: Context): DatabaseManager =
            withContext(Dispatchers.IO) {
                val db = DatabaseManager()

                awaitAll(
                    async { db.copyDatabaseFromBundle(context) },
                    async { db.copyUserDatabaseFromBundle(context) }
                )

                db.setupDatabase()

                return@withContext db
            }
    }
}