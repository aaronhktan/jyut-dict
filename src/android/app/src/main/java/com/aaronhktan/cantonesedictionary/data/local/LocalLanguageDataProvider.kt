package com.aaronhktan.cantonesedictionary.data.local

import androidx.compose.ui.graphics.Color
import kotlin.String
import kotlin.collections.Map

object LocalLanguageDataProvider {
    val iso693ToLanguageName: Map<String, String> = mapOf(
        "cmn" to "Mandarin",
        "deu" to "German",
        "eng" to "English",
        "fra" to "French",
        "yue" to "Cantonese",
        "zh" to "Chinese",
        "zho" to "Chinese",
    )

    val iso693ToColour: Map<String, Color> = mapOf(
        "cmn" to Color(0xFF0E8B53),
        "deu" to Color(0xFFBAC85F),
        "eng" to Color(0xFF644C8F),
        "fra" to Color(0xFF00308F),
        "yue" to Color(0xFFAD1F5A),
    )
}