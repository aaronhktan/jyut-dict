package com.aaronhktan.cantonesedictionary.data.datastore

import androidx.compose.ui.graphics.Color

val defaultJyutpingToneColours: List<Color> = listOf(
    Color(0xff808080),
    Color(0xff00bcd4),
    Color(0xff7cb342),
    Color(0xff657ff1),
    Color(0xffc2185b),
    Color(0xff068900),
    Color(0xff7651d0),
)

val defaultPinyinToneColours: List<Color> = listOf(
    Color(0xff808080),
    Color(0xfffc433c),
    Color(0xff02ba1f),
    Color(0xff18a6f2),
    Color(0xff9e77ff),
    Color(0xff808080),
)

val languageColours: Map<String, Color> = mapOf(
    "cmn" to Color(0xFF0E8B53),
    "deu" to Color(0xFFBAC85F),
    "eng" to Color(0xFF644C8F),
    "fra" to Color(0xFF00308F),
    "yue" to Color(0xFFAD1F5A),
)