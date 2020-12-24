#include "utils_linux.h"

#include "logic/settings/settingsutils.h"

namespace Utils {

bool isDarkMode(void) {
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    return settings->value("Advanced/forceDarkMode",
                           QVariant{false}).toBool();
}

}
