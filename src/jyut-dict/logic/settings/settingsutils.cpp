#include "settingsutils.h"

#include <QCoreApplication>

namespace Settings
{

std::unique_ptr<QSettings> getSettings(QObject *parent)
{
#ifdef PORTABLE
    std::unique_ptr<QSettings> settings
        = std::make_unique<QSettings>(QCoreApplication::applicationDirPath()
                                          + "/../Resources/settings.ini",
                                      QSettings::IniFormat,
                                      parent);
    return settings;
#else
    retun std::make_unique<QSettings>(parent);
#endif
}

}
