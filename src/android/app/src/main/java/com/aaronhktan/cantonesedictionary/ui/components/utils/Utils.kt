package com.aaronhktan.cantonesedictionary.ui.components.utils

import androidx.compose.ui.graphics.Color

fun getContrastingColour(backgroundColour: Color): Color {
    val brightness =
        backgroundColour.red * 0.299 + backgroundColour.green * 0.587 + backgroundColour.blue * 0.114
    return if (brightness > 0.65) Color.Black else Color.White
}