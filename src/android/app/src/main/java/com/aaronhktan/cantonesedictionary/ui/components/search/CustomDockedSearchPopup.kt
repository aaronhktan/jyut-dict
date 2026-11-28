package com.aaronhktan.cantonesedictionary.ui.components.search

import android.view.animation.PathInterpolator
import androidx.compose.animation.AnimatedContent
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.SizeTransform
import androidx.compose.animation.animateContentSize
import androidx.compose.animation.core.Easing
import androidx.compose.animation.core.Spring
import androidx.compose.animation.core.keyframes
import androidx.compose.animation.core.spring
import androidx.compose.animation.core.tween
import androidx.compose.animation.expandHorizontally
import androidx.compose.animation.expandVertically
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.animation.shrinkHorizontally
import androidx.compose.animation.shrinkVertically
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.ime
import androidx.compose.foundation.layout.navigationBars
import androidx.compose.foundation.layout.systemBars
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.onFocusChanged
import androidx.compose.ui.layout.boundsInWindow
import androidx.compose.ui.layout.onGloballyPositioned
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.platform.LocalWindowInfo
import androidx.compose.ui.res.dimensionResource
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntRect
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.LayoutDirection
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Popup
import androidx.compose.ui.window.PopupPositionProvider
import androidx.compose.ui.window.PopupProperties
import com.aaronhktan.cantonesedictionary.R
import kotlin.math.roundToInt

// Based on values in https://m3.material.io/styles/motion/easing-and-duration/tokens-specs
val EmphasizedDecelerateEasing: Easing = Easing { fraction ->
    PathInterpolator(0.05f, 0.7f, 0.1f, 1f).getInterpolation(fraction)
}

val EmphasizedAccelerateEasing: Easing = Easing { fraction ->
    PathInterpolator(0.3f, 0.0f, 0.8f, 0.15f).getInterpolation(fraction)
}

@Composable
fun CustomDockedSearchPopup(
    expanded: Boolean,
    onDismissRequest: () -> Unit,
    modifier: Modifier = Modifier,
    gap: Dp = 4.dp,
    backButton: @Composable () -> Unit,
    inputField: @Composable (FocusRequester, Modifier) -> Unit,
    results: @Composable () -> Unit
) {
    // The focusRequester lets the input field request focus when searching
    val focusRequester = remember { FocusRequester() }

    LaunchedEffect(expanded) {
        if (expanded) {
            focusRequester.requestFocus()
        }
    }

    // The anchor values are properties of the search box
    var anchorWidth by remember { mutableIntStateOf(0) }
    var anchorBottomPx by remember { mutableIntStateOf(0) }

    val density = LocalDensity.current

    val anchorBottom = with(density) { anchorBottomPx.toDp() }
    val containerHeightPx = LocalWindowInfo.current.containerSize.height

    val keyboardHeightPx = WindowInsets.ime.getBottom(density)
    val keyboardTopPx = containerHeightPx - keyboardHeightPx
    val keyboardTop = with(density) { keyboardTopPx.toDp() }

    val navigationBarsHeightPx = WindowInsets.navigationBars.getBottom(density)
    val navigationBarsTopPx = containerHeightPx - navigationBarsHeightPx
    val navigationBarsTop = with(density) { navigationBarsTopPx.toDp() }

    val maxHeight = minOf(
        // Heights of elements that the popup should not overlap with, from bottom to top
        (navigationBarsTop - dimensionResource(R.dimen.content_padding) - gap - anchorBottom).coerceAtLeast(
            0.dp
        ),
        (keyboardTop - dimensionResource(R.dimen.content_padding) - gap - anchorBottom).coerceAtLeast(
            0.dp
        )
    )

    Box(modifier = modifier) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .onGloballyPositioned {
                    anchorWidth = it.size.width
                    anchorBottomPx = it.boundsInWindow().bottom.roundToInt()
                },
            verticalAlignment = Alignment.CenterVertically
        ) {
            AnimatedVisibility(
                visible = expanded,
                enter = expandHorizontally(
                    expandFrom = Alignment.Start
                ) + fadeIn(),
                exit = shrinkHorizontally(
                    shrinkTowards = Alignment.Start
                ) + fadeOut(),
            ) {
                backButton()
            }

            Box(
                modifier = Modifier
                    .weight(1f)
            ) {
                inputField(focusRequester, Modifier)

            }
        }

        if (anchorWidth > 0) {
            val gapPx = with(density) { gap.roundToPx() }

            Popup(
                popupPositionProvider = remember(gapPx, keyboardTopPx) {
                    object : PopupPositionProvider {
                        override fun calculatePosition(
                            anchorBounds: IntRect,
                            windowSize: IntSize,
                            layoutDirection: LayoutDirection,
                            popupContentSize: IntSize
                        ): IntOffset {
                            val x = when (layoutDirection) {
                                LayoutDirection.Ltr -> anchorBounds.left

                                LayoutDirection.Rtl -> anchorBounds.right - popupContentSize.width
                            }.coerceIn(
                                0, (windowSize.width - popupContentSize.width).coerceAtLeast(0)
                            )

                            val y = anchorBounds.bottom + gapPx

                            return IntOffset(x, y)
                        }
                    }
                },
                onDismissRequest = onDismissRequest,
                properties = PopupProperties(
                    focusable = false,
                    dismissOnClickOutside = false,
                )
            ) {
                AnimatedVisibility(
                    visible = expanded, enter = expandVertically(
                        expandFrom = Alignment.Top,
                        animationSpec = tween(300, easing = EmphasizedDecelerateEasing)
                    ) + fadeIn(
                        animationSpec = tween(150)
                    ), exit = shrinkVertically(
                        shrinkTowards = Alignment.Top,
                        animationSpec = tween(300, easing = EmphasizedAccelerateEasing)
                    ) + fadeOut(
                        animationSpec = tween(300)
                    )
                ) {
                    Surface(
                        modifier = Modifier
                            .width(with(density) { anchorWidth.toDp() })
                            .heightIn(max = maxHeight),
                        shape = RoundedCornerShape(28.dp),
                        color = MaterialTheme.colorScheme.surfaceContainerHigh,
                    ) {
                        Box {
                            results()
                        }
                    }
                }
            }
        }
    }
}