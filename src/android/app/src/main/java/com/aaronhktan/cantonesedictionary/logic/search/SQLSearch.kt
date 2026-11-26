package com.aaronhktan.cantonesedictionary.logic.search

import android.util.Log
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import com.aaronhktan.cantonesedictionary.logic.entry.Entry
import com.aaronhktan.cantonesedictionary.logic.example.Example
import com.aaronhktan.cantonesedictionary.logic.utils.columnIndices
import com.aaronhktan.cantonesedictionary.logic.utils.parseExistenceRecords
import com.aaronhktan.cantonesedictionary.logic.utils.parseReturnedEntryRecords
import com.aaronhktan.cantonesedictionary.logic.utils.parseReturnedExampleRecords
import com.aaronhktan.cantonesedictionary.logic.utils.prepareCharacterBindValues
import com.aaronhktan.cantonesedictionary.logic.utils.prepareEnglishBindValues
import com.aaronhktan.cantonesedictionary.logic.utils.prepareJyutpingBindValues
import com.aaronhktan.cantonesedictionary.logic.utils.preparePinyinBindValues
import com.aaronhktan.cantonesedictionary.logic.utils.unfoldBasicRegex
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.currentCoroutineContext
import kotlinx.coroutines.ensureActive
import kotlinx.coroutines.withContext
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import java.util.regex.PatternSyntaxException
import kotlin.math.min
import kotlin.text.Regex

private const val LOG_TAG = "SQLSearch"

class SQLSearch {
    private var _manager: DatabaseManager

    private var _jyutpingCache: Pair<Int, List<String>>? = null
    private var _pinyinCache: Pair<Int, List<String>>? = null

    constructor(manager: DatabaseManager) {
        _manager = manager
    }

    private suspend fun populateJyutpingCache(): List<String> {
        val results: MutableList<String> = mutableListOf()
        _manager.read { conn ->
            conn.prepare(fetchAllJyutpingQuery).use { statement ->
                val jyutpingIdx = statement.columnIndices()["jyutping"]!!
                while (statement.step()) {
                    currentCoroutineContext().ensureActive()
                    results.add(statement.getText(jyutpingIdx))
                }
            }
        }
        currentCoroutineContext().ensureActive()
        return results.toList()
    }

    private suspend fun populatePinyinCache(): List<String> {
        val results: MutableList<String> = mutableListOf()
        _manager.read { conn ->
            conn.prepare(fetchAllPinyinQuery).use { statement ->
                val pinyinIdx = statement.columnIndices()["pinyin"]!!
                while (statement.step()) {
                    currentCoroutineContext().ensureActive()
                    results.add(statement.getText(pinyinIdx))
                }
            }
        }
        currentCoroutineContext().ensureActive()
        return results.toList()
    }

    private suspend fun jyutpingFilteredBy(prefix: String): List<String> {
        val results: MutableList<String> = mutableListOf()
        _manager.read { conn ->
            conn.prepare(fetchJyutpingWithPrefixQuery).use { statement ->
                statement.bindText(1, "$prefix*")
                val jyutpingIdx = statement.columnIndices()["jyutping"]!!
                while (statement.step()) {
                    currentCoroutineContext().ensureActive()
                    results.add(statement.getText(jyutpingIdx))
                }
            }
        }
        currentCoroutineContext().ensureActive()
        return results.toList()
    }

    private suspend fun pinyinFilteredBy(prefix: String): List<String> {
        val results: MutableList<String> = mutableListOf()
        _manager.read { conn ->
            conn.prepare(fetchPinyinWithPrefixQuery).use { statement ->
                statement.bindText(1, "$prefix*")
                val pinyinIdx = statement.columnIndices()["pinyin"]!!
                while (statement.step()) {
                    currentCoroutineContext().ensureActive()
                    results.add(statement.getText(pinyinIdx))
                }
            }
        }
        currentCoroutineContext().ensureActive()
        return results.toList()
    }

    private suspend fun allJyutping(): List<String> {
        // TODO: Calculate a hash for all sources and only update Jyutping cache if sources changes
        _jyutpingCache = Pair(0, populateJyutpingCache())
        return _jyutpingCache?.component2() ?: listOf()
    }

    private suspend fun allPinyin(): List<String> {
        // TODO: Calculate a hash for all sources and only update Pinyin cache if sources changes
        _pinyinCache = Pair(0, populatePinyinCache())
        return _pinyinCache?.component2() ?: listOf()
    }

    private suspend fun allRowIds(): List<Int> {
        val results: MutableList<Int> = mutableListOf()
        _manager.read { conn ->
            conn.prepare(fetchAllRowId).use { statement ->
                val entryIdIdx = statement.columnIndices()["entry_id"]!!
                while (statement.step()) {
                    currentCoroutineContext().ensureActive()
                    results.add(statement.getInt(entryIdIdx))
                }
            }
        }
        currentCoroutineContext().ensureActive()
        return results.toList()
    }

    private suspend fun rowIdFilteredByJyutping(prefix: String): List<Int> {
        val results: MutableList<Int> = mutableListOf()
        _manager.read { conn ->
            conn.prepare(fetchRowIdWithJyutpingPrefixQuery).use { statement ->
                statement.bindText(1, "$prefix*")
                val entryIdIdx = statement.columnIndices()["entry_id"]!!
                while (statement.step()) {
                    currentCoroutineContext().ensureActive()
                    results.add(statement.getInt(entryIdIdx))
                }
            }
        }
        currentCoroutineContext().ensureActive()
        return results.toList()
    }

    private suspend fun rowIdFilteredByPinyin(prefix: String): List<Int> {
        val results: MutableList<Int> = mutableListOf()
        _manager.read { conn ->
            conn.prepare(fetchRowIdWithPinyinPrefixQuery).use { statement ->
                statement.bindText(1, "$prefix*")
                val entryIdIdx = statement.columnIndices()["entry_id"]!!
                while (statement.step()) {
                    currentCoroutineContext().ensureActive()
                    results.add(statement.getInt(entryIdIdx))
                }
            }
        }
        currentCoroutineContext().ensureActive()
        return results.toList()
    }

    private fun literalPrefixesOf(pattern: String): List<String> {
        // Truncate string to right before a second "?" or "(", whichever occurs
        // earlier. (The limitation of unfoldBasicRegex is that we only support
        // one set of parentheses and one question mark.)
        var endIdx = pattern.length
        val firstOpen = pattern.indexOf("(")
        if (firstOpen != -1) {
            val secondOpen = pattern.indexOf("(", firstOpen + 1)
            if (secondOpen != -1) {
                endIdx = secondOpen
            }
        }

        val firstQuestion = pattern.indexOf("?")
        if (firstQuestion != -1) {
            val secondQuestion = pattern.indexOf("?", firstQuestion + 1)
            if (secondQuestion != -1) {
                endIdx = min(endIdx, secondQuestion - 1)
            }
        }

        val possibilities = unfoldBasicRegex(pattern = pattern.substring(0..<endIdx))

        val prefixes: MutableSet<String> = mutableSetOf()
        for (p in possibilities) {
            var prefix = ""
            for (c in p) {
                if (!c.isLetterOrDigit()) {
                    break
                }
                prefix += c
            }

            val next = p.drop(prefix.length).firstOrNull()
            if (next == null) {
                if (prefix.isNotEmpty()) {
                    prefixes.add(prefix)
                }
                continue
            }

            when (next) {
                '?', '*' -> {
                    prefix = prefix.dropLast(1)
                }

                else -> {
                    Unit
                }
            }

            if (prefix.isNotEmpty()) {
                prefixes.add(prefix)
            }
        }

        return prefixes.toList()
    }

    suspend fun searchByRowId(rowId: String): Entry? {
        var result: Entry? = null

        _manager.read { conn ->
            conn.prepare(getEntryByRowId).use { statement ->
                statement.bindText(1, rowId)
                withContext(Dispatchers.Default) {
                    result = parseReturnedEntryRecords(statement)[0]
                }
            }
        }

        return result
    }

    suspend fun searchTraditional(searchTerm: String): List<Entry> {
        val pattern = prepareCharacterBindValues(searchTerm)
        Log.i(LOG_TAG, "searchTraditional pattern: '$pattern'")

        var result: List<Entry> = listOf()
        _manager.read { conn ->
            conn.prepare(searchTraditionalQuery).use { statement ->
                statement.bindText(1, pattern)
                withContext(Dispatchers.Default) {
                    result = parseReturnedEntryRecords(statement)
                }
            }
        }

        return result
    }

    suspend fun searchSimplified(searchTerm: String): List<Entry> {
        val pattern = prepareCharacterBindValues(searchTerm)
        Log.i(LOG_TAG, "searchSimplified pattern: '$pattern'")

        var result: List<Entry> = listOf()
        _manager.read { conn ->
            conn.prepare(searchSimplifiedQuery).use { statement ->
                statement.bindText(1, pattern)
                withContext(Dispatchers.Default) {
                    result = parseReturnedEntryRecords(statement)
                }
            }
        }

        return result
    }

    suspend fun searchJyutpingExistence(
        searchTerm: String,
        useFuzzyJyutping: Boolean,
        unsafeFuzzyJyutping: Boolean
    ): Boolean {
        val pattern =
            prepareJyutpingBindValues(jyutping = searchTerm, useFuzzyJyutping, unsafeFuzzyJyutping)
        Log.i(
            LOG_TAG,
            "searchJyutpingExistence pattern: '$pattern', useFuzzyJyutping: $useFuzzyJyutping"
        )

        if (!useFuzzyJyutping) {
            var result = false
            _manager.read { conn ->
                conn.prepare(jyutpingExistenceQuery).use { statement ->
                    statement.bindText(1, pattern)
                    result = parseExistenceRecords(statement)
                }
            }
            return result
        }

        try {
            val regex = Regex(pattern)
            val prefixes = literalPrefixesOf(pattern)
            Log.i(LOG_TAG, "Jyutping prefixes for pattern $pattern: $prefixes")
            val candidates: MutableSet<String> = mutableSetOf()
            if (prefixes.isEmpty()) {
                candidates.addAll(allJyutping())
            } else {
                for (p in prefixes) {
                    candidates.addAll(jyutpingFilteredBy(p))
                }
            }
            currentCoroutineContext().ensureActive()
            candidates.forEachIndexed { index, text ->
                if (index % 10000 == 0) {
                    currentCoroutineContext().ensureActive()
                }
                if (regex.matchAt(text, 0) != null) {
                    Log.i(LOG_TAG, "Found fuzzy Jyutping match for pattern $pattern: $text")
                    return true
                }
            }
        } catch (e: PatternSyntaxException) {
            Log.e(
                LOG_TAG,
                "Failed to compile regex '$pattern' in Jyutping existence check, error: ${e.message}"
            )
        }

        Log.i(LOG_TAG, "Failed to find fuzzy Jyutping match for pattern $pattern")
        return false
    }

    suspend fun searchJyutping(
        searchTerm: String,
        useFuzzyJyutping: Boolean,
        unsafeFuzzyJyutping: Boolean
    ): List<Entry> {
        val pattern =
            prepareJyutpingBindValues(jyutping = searchTerm, useFuzzyJyutping, unsafeFuzzyJyutping)
        Log.i(LOG_TAG, "searchJyutping pattern: '$pattern'")

        if (!useFuzzyJyutping) {
            var result: List<Entry> = listOf()
            _manager.read { conn ->
                conn.prepare(searchJyutpingGlobQuery).use { statement ->
                    statement.bindText(1, pattern)
                    result = parseReturnedEntryRecords(statement)
                }
            }
            return result
        }

        val prefixes = literalPrefixesOf(pattern)
        Log.d(LOG_TAG, "searchJyutping prefixes: $prefixes")
        val candidates: MutableSet<Int> = mutableSetOf()
        if (prefixes.isEmpty()) {
            candidates.addAll(allRowIds())
        } else {
            for (p in prefixes) {
                candidates.addAll(rowIdFilteredByJyutping(p))
            }
        }
        currentCoroutineContext().ensureActive()

        val json = Json.encodeToString(candidates)
        var result: List<Entry> = listOf()
        _manager.read { conn ->
            conn.prepare(searchJyutpingRegexQuery).use { statement ->
                statement.bindText(1, json)
                statement.bindText(2, pattern)
                withContext(Dispatchers.Default) {
                    result = parseReturnedEntryRecords(statement)
                }
            }
        }

        return result
    }

    suspend fun searchPinyinExistence(
        searchTerm: String,
        useFuzzyPinyin: Boolean,
    ): Boolean {
        val pattern =
            preparePinyinBindValues(pinyin = searchTerm, useFuzzyPinyin)
        Log.i(
            LOG_TAG,
            "searchPinyinExistence pattern: '$pattern', useFuzzyPinyin: $useFuzzyPinyin"
        )

        if (!useFuzzyPinyin) {
            var result = false
            _manager.read { conn ->
                conn.prepare(pinyinExistenceQuery).use { statement ->
                    statement.bindText(1, pattern)
                    result = parseExistenceRecords(statement)
                }
            }
            return result
        }

        try {
            val regex = Regex(pattern)
            val prefixes = literalPrefixesOf(pattern)
            Log.i(LOG_TAG, "Pinyin prefixes for pattern $pattern: $prefixes")
            val candidates: MutableSet<String> = mutableSetOf()
            if (prefixes.isEmpty()) {
                candidates.addAll(allJyutping())
            } else {
                for (p in prefixes) {
                    candidates.addAll(jyutpingFilteredBy(p))
                }
            }
            currentCoroutineContext().ensureActive()
            candidates.forEachIndexed { index, text ->
                if (index % 10000 == 0) {
                    currentCoroutineContext().ensureActive()
                }
                if (regex.matchAt(text, 0) != null) {
                    Log.i(LOG_TAG, "Found fuzzy Pinyin match for pattern $pattern: $text")
                    return true
                }
            }
        } catch (e: PatternSyntaxException) {
            Log.e(
                LOG_TAG,
                "Failed to compile regex '$pattern' in Pinyin existence check, error: ${e.message}"
            )
        }

        Log.i(LOG_TAG, "Failed to find fuzzy Pinyin match for pattern $pattern")
        return false
    }

    suspend fun searchPinyin(
        searchTerm: String,
        useFuzzyPinyin: Boolean,
    ): List<Entry> {
        val pattern =
            preparePinyinBindValues(pinyin = searchTerm, useFuzzyPinyin)
        Log.i(LOG_TAG, "searchJyutping pattern: '$pattern'")

        if (!useFuzzyPinyin) {
            var result: List<Entry> = listOf()
            _manager.read { conn ->
                conn.prepare(searchPinyinGlobQuery).use { statement ->
                    statement.bindText(1, pattern)
                    result = parseReturnedEntryRecords(statement)
                }
            }
            return result
        }

        val prefixes = literalPrefixesOf(pattern)
        Log.d(LOG_TAG, "searchPinyin prefixes: $prefixes")
        val candidates: MutableSet<Int> = mutableSetOf()
        if (prefixes.isEmpty()) {
            candidates.addAll(allRowIds())
        } else {
            for (p in prefixes) {
                candidates.addAll(rowIdFilteredByPinyin(p))
            }
        }
        currentCoroutineContext().ensureActive()

        val json = Json.encodeToString(candidates)
        var result: List<Entry> = listOf()
        _manager.read { conn ->
            conn.prepare(searchPinyinRegexQuery).use { statement ->
                statement.bindText(1, json)
                statement.bindText(2, pattern)
                withContext(Dispatchers.Default) {
                    result = parseReturnedEntryRecords(statement)
                }
            }
        }

        return result
    }

    suspend fun searchEnglish(searchTerm: String): List<Entry> {
        val (ftsParam, likeParam) = prepareEnglishBindValues(english = searchTerm)
        Log.d(LOG_TAG, "searchEnglish patterns: '$ftsParam', '$likeParam'")

        var result: List<Entry> = listOf()
        _manager.read { conn ->
            conn.prepare(searchEnglishQuery).use { statement ->
                statement.bindText(1, ftsParam)
                statement.bindText(2, likeParam)
                result = parseReturnedEntryRecords(statement)
            }
        }

        return result
    }

    suspend fun searchExamplesByTraditional(searchTerm: String): List<Example> {
        val likeTerm = "%$searchTerm%"
        Log.d(LOG_TAG, "searchExamplesByTraditional pattern: '$likeTerm'")

        var result: List<Example> = listOf()
        _manager.read { conn ->
            conn.prepare(searchExamplesQuery).use { statement ->
                statement.bindText(1, likeTerm)
                withContext(Dispatchers.Default) {
                    result = parseReturnedExampleRecords(statement)
                }
            }
        }

        return result
    }
}