#ifndef UTILS_QT_H
#define UTILS_QT_H

#include "logic/search/searchparameters.h"

#include <QColor>

// Whereas utils.h should contain only STL functions, utils_qt can contain
// stuff specific to Qt.

namespace Utils {
constexpr auto TITLE_BAR_BACKGROUND_COLOR_DARK_R = 42;
constexpr auto TITLE_BAR_BACKGROUND_COLOR_DARK_G = 42;
constexpr auto TITLE_BAR_BACKGROUND_COLOR_DARK_B = 42;

constexpr auto TITLE_BAR_INACTIVE_BACKGROUND_COLOR_DARK_R = 45;
constexpr auto TITLE_BAR_INACTIVE_BACKGROUND_COLOR_DARK_G = 45;
constexpr auto TITLE_BAR_INACTIVE_BACKGROUND_COLOR_DARK_B = 45;

constexpr auto TITLE_BAR_BORDER_COLOR_DARK_R = 0;
constexpr auto TITLE_BAR_BORDER_COLOR_DARK_G = 0;
constexpr auto TITLE_BAR_BORDER_COLOR_DARK_B = 0;

constexpr auto LABEL_TEXT_COLOUR_DARK_R = 168;
constexpr auto LABEL_TEXT_COLOUR_DARK_G = 168;
constexpr auto LABEL_TEXT_COLOUR_DARK_B = 168;

constexpr auto HEADER_BACKGROUND_COLOUR_DARK_R = 60;
constexpr auto HEADER_BACKGROUND_COLOUR_DARK_G = 60;
constexpr auto HEADER_BACKGROUND_COLOUR_DARK_B = 60;

constexpr auto CONTENT_BACKGROUND_COLOUR_DARK_R = 50;
constexpr auto CONTENT_BACKGROUND_COLOUR_DARK_G = 50;
constexpr auto CONTENT_BACKGROUND_COLOUR_DARK_B = 50;

constexpr auto BACKGROUND_COLOUR_DARK_R = 30;
constexpr auto BACKGROUND_COLOUR_DARK_G = 30;
constexpr auto BACKGROUND_COLOUR_DARK_B = 30;

constexpr auto LIST_ITEM_ACTIVE_COLOUR_DARK_R = 204;
constexpr auto LIST_ITEM_ACTIVE_COLOUR_DARK_G = 0;
constexpr auto LIST_ITEM_ACTIVE_COLOUR_DARK_B = 1;

#ifdef Q_OS_MAC
constexpr auto LIST_ITEM_INACTIVE_COLOUR_DARK_R = 220;
constexpr auto LIST_ITEM_INACTIVE_COLOUR_DARK_G = 220;
constexpr auto LIST_ITEM_INACTIVE_COLOUR_DARK_B = 220;
#else
constexpr auto LIST_ITEM_INACTIVE_COLOUR_DARK_R = 45;
constexpr auto LIST_ITEM_INACTIVE_COLOUR_DARK_G = 45;
constexpr auto LIST_ITEM_INACTIVE_COLOUR_DARK_B = 45;
#endif

constexpr auto TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_R = 128;
constexpr auto TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_G = 128;
constexpr auto TOOLBAR_TEXT_INACTIVE_COLOUR_DARK_B = 128;

constexpr auto TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_R = 255;
constexpr auto TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_G = 255;
constexpr auto TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_DARK_B = 255;

constexpr auto LABEL_TEXT_COLOUR_LIGHT_R = 111;
constexpr auto LABEL_TEXT_COLOUR_LIGHT_G = 111;
constexpr auto LABEL_TEXT_COLOUR_LIGHT_B = 111;

constexpr auto TITLE_BAR_BACKGROUND_COLOR_R = 212;
constexpr auto TITLE_BAR_BACKGROUND_COLOR_G = 212;
constexpr auto TITLE_BAR_BACKGROUND_COLOR_B = 212;

constexpr auto TITLE_BAR_INACTIVE_BACKGROUND_COLOR_R = 246;
constexpr auto TITLE_BAR_INACTIVE_BACKGROUND_COLOR_G = 246;
constexpr auto TITLE_BAR_INACTIVE_BACKGROUND_COLOR_B = 246;

constexpr auto TITLE_BAR_BORDER_COLOR_R = 191;
constexpr auto TITLE_BAR_BORDER_COLOR_G = 191;
constexpr auto TITLE_BAR_BORDER_COLOR_B = 191;

constexpr auto HEADER_BACKGROUND_COLOUR_LIGHT_R = 215;
constexpr auto HEADER_BACKGROUND_COLOUR_LIGHT_G = 215;
constexpr auto HEADER_BACKGROUND_COLOUR_LIGHT_B = 215;

constexpr auto CONTENT_BACKGROUND_COLOUR_LIGHT_R = 235;
constexpr auto CONTENT_BACKGROUND_COLOUR_LIGHT_G = 235;
constexpr auto CONTENT_BACKGROUND_COLOUR_LIGHT_B = 235;

constexpr auto BACKGROUND_COLOUR_LIGHT_R = 255;
constexpr auto BACKGROUND_COLOUR_LIGHT_G = 255;
constexpr auto BACKGROUND_COLOUR_LIGHT_B = 255;

constexpr auto LIST_ITEM_ACTIVE_COLOUR_LIGHT_R = 204;
constexpr auto LIST_ITEM_ACTIVE_COLOUR_LIGHT_G = 0;
constexpr auto LIST_ITEM_ACTIVE_COLOUR_LIGHT_B = 1;

constexpr auto LIST_ITEM_INACTIVE_COLOUR_LIGHT_R = 220;
constexpr auto LIST_ITEM_INACTIVE_COLOUR_LIGHT_G = 220;
constexpr auto LIST_ITEM_INACTIVE_COLOUR_LIGHT_B = 220;

constexpr auto TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_R = 128;
constexpr auto TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_G = 128;
constexpr auto TOOLBAR_TEXT_INACTIVE_COLOUR_LIGHT_B = 128;

constexpr auto TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_R = 0;
constexpr auto TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_G = 0;
constexpr auto TOOLBAR_TEXT_NOT_FOCUSED_COLOUR_LIGHT_B = 0;

// The following is used to calculate a contrasting colour
// https://stackoverflow.com/questions/946544/good-text-foreground-color-for-a-given-background-color
QColor getContrastingColour(const QColor &backgroundColour);

QColor getLanguageColour(const std::string &language);

QString getLanguageFromISO639(const std::string &language);
std::string getISO639FromLanguage(const QString &language);

std::string getStringFromSearchParameter(SearchParameters parameter);

void refreshLanguageMap();

QString getLicenseFolderPath();
} // namespace Utils

#endif // UTILS_QT_H
