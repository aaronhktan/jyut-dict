#include "utils_linux.h"

#include "logic/settings/settingsutils.h"

#include <QGuiApplication>
#include <QStyleHints>

namespace Utils {

bool isDarkMode(void) {
    bool systemDarkMode = QGuiApplication::styleHints()->colorScheme()
                          == Qt::ColorScheme::Dark;
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    return systemDarkMode
           || settings->value("Advanced/forceDarkMode", QVariant{false}).toBool();
}

}
