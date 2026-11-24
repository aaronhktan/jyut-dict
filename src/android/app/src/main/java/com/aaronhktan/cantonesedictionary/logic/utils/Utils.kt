package com.aaronhktan.cantonesedictionary.logic.utils

import androidx.sqlite.SQLiteStatement

const val SAME_CHARACTER_STRING = "－"

fun unfoldBasicRegex(pattern: String): List<String> {
    val out: MutableList<String> = mutableListOf()
    val stringPossibilities: MutableList<String> = mutableListOf()

    // Invariant: there must be only one set of parentheses and one question mark
    if (pattern.count { it == '(' } > 1
        || pattern.count { it == ')' } > 1
        || pattern.count { it == '?' } > 1)
    {
        return out.toList()
    }

    // If there is a parenthesis, then all of the options must be checked
    if (pattern.indexOf("(") != -1
        && pattern.indexOf(")") != -1) {
        val openParenIdx = pattern.indexOf("(")
        var startIdx = openParenIdx + 1
        val endIdx = pattern.indexOf(")")
        var orIdx = pattern.indexOf("|")

        while (orIdx != -1) {
            val preParen = pattern.substring(0..<openParenIdx)
            val possibility = pattern.substring(startIdx..<orIdx)
            val postParen = pattern.substring(endIdx+1)
            stringPossibilities.add("$preParen$possibility$postParen")
            startIdx = orIdx + 1
            orIdx = pattern.indexOf("|", startIdx + 1)
        }
        val preParen = pattern.substring(0..<openParenIdx)
        val possibility = pattern.substring(startIdx..<endIdx)
        val postParen = pattern.substring(endIdx+1)
        stringPossibilities.add("$preParen$possibility$postParen")
    } else {
        stringPossibilities.add(pattern)
    }

    // If there is a "?", then the initial with and the initial without
    // that optional character should be considered
    for (s in stringPossibilities) {
        when (val regexIdx = s.indexOf("?")) {
            -1 -> {
                out.add(s)
            }
            0 -> {
                out.add(s.drop(1))
            }
            else -> {
                out.add(s.removeRange(regexIdx - 1..regexIdx))
                out.add(s.removeRange(regexIdx..regexIdx))
            }
        }
    }

    return out.toList()
}

fun SQLiteStatement.columnIndices(): Map<String, Int> {
    val columnMap: MutableMap<String, Int> = mutableMapOf()
    for (i in 0..<getColumnCount()) {
        columnMap[getColumnName(i)] = i
    }
    return columnMap.toMap()
}