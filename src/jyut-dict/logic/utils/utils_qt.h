#ifndef UTILS_QT_H
#define UTILS_QT_H

#include <QColor>

// Whereas utils.h should contain only STL functions, utils_qt can contain
// stuff specific to Qt.

namespace Utils {

const QColor LABEL_TEXT_COLOUR_DARK = QColor{168, 168, 168};
const QColor HEADER_BACKGROUND_COLOUR_DARK = QColor{60, 60, 60};
const QColor CONTENT_BACKGROUND_COLOUR_DARK = QColor{50, 50, 50};
const QColor BACKGROUND_COLOUR_DARK = QColor{30, 30, 30};
const QColor LIST_ITEM_ACTIVE_COLOUR_DARK = QColor{204, 0, 1};
const QColor LIST_ITEM_INACTIVE_COLOUR_DARK = QColor{220, 220, 220};
const QColor TOOLBAR_TEXT_INACTIVE_COLOUR_DARK = QColor{128, 128, 128};
const QColor TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK = QColor{255, 255, 255};

const QColor LABEL_TEXT_COLOUR_LIGHT = QColor{111, 111, 111};
const QColor HEADER_BACKGROUND_COLOUR_LIGHT = QColor{235, 235, 235};
const QColor CONTENT_BACKGROUND_COLOUR_LIGHT = QColor{245, 245, 245};
const QColor BACKGROUND_COLOUR_LIGHT = QColor{255, 255, 255};
const QColor LIST_ITEM_ACTIVE_COLOUR_LIGHT = QColor{204, 0, 1};
const QColor LIST_ITEM_INACTIVE_COLOUR_LIGHT = QColor{220, 220, 220};
const QColor TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT = QColor{128, 128, 128};
const QColor TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT = QColor{0, 0, 0};

// The following is used to calculate a contrasting colour
// https://stackoverflow.com/questions/946544/good-text-foreground-color-for-a-given-background-color
QColor getContrastingColour(const QColor backgroundColour);

}

#endif // UTILS_QT_H
