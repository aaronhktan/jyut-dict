#include "settingsutils.h"

#include <QCoreApplication>

namespace Settings
{

std::unique_ptr<QSettings> getSettings(QObject *parent)
{
#ifdef Q_OS_MAC
    QString settingsPath = QCoreApplication::applicationDirPath() + "/../Resources/settings.ini";
#elif defined(Q_OS_WIN)
    QString settingsPath = QCoreApplication::applicationDirPath() + "./settings.ini";
#elif defined(Q_OS_LINUX)
    QString settingsPath = QCoreApplication::applicationDirPath() + "/settings.ini";
#endif
#ifdef PORTABLE
    std::unique_ptr<QSettings> settings
        = std::make_unique<QSettings>(settingsPath,
                                      QSettings::IniFormat,
                                      parent);
    return settings;
#else
    retun std::make_unique<QSettings>(parent);
#endif
}

}
