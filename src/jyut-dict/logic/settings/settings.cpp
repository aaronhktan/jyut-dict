#include "settings.h"

#include <QCoreApplication>

namespace Settings
{

std::unique_ptr<QSettings> getSettings(QObject *parent)
{
#ifdef PORTABLE
    return std::make_unique<QSettings>(QCoreApplication::applicationDirPath()
                                           + "/../Resources/settings.ini",
                                       QSettings::IniFormat,
                                       parent);
#else
    retun std::make_unique<QSettings>(parent);
#endif
}

}
