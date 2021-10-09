#ifndef UTILS_MAC_H
#define UTILS_MAC_H

#include <QColor>

// Utils_Mac contains utilities that are only useful/compilable on macOS.

namespace Utils {

QColor getAppleControlAccentColor();
bool isDarkMode(void);

}

#endif // UTILS_MAC_H
