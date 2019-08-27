#include "settingsutils.h"

#include <QCoreApplication>
#include <QUuid>

namespace Settings
{

QTranslator systemTranslator;
QTranslator applicationTranslator;

QLocale currentLocale;

std::unique_ptr<QSettings> getSettings(QObject *parent)
{
#ifdef Q_OS_MAC
    QString settingsPath = QCoreApplication::applicationDirPath() + "/../Resources/settings.ini";
#elif defined(Q_OS_WIN)
    QString settingsPath = QCoreApplication::applicationDirPath() + "./settings.ini";
#elif defined(Q_OS_LINUX)
    QString settingsPath = QCoreApplication::applicationDirPath() + "/settings.ini";
#endif

    std::unique_ptr<QSettings> settings;

#ifdef PORTABLE
    settings = std::make_unique<QSettings>(settingsPath,
                                           QSettings::IniFormat,
                                           parent);
#else
    settings = std::make_unique<QSettings>(parent);
#endif

    updateSettings(*settings);

    return settings;
}

bool updateSettings(QSettings &settings)
{
    if (settings.value("Analytics/uuid") == QVariant{}) {
        settings.setValue("Analytics/uuid",
                          QVariant{QUuid::createUuid().toString()});
        settings.sync();
    }

    if (settings.value("Metadata/version", QVariant{SETTINGS_VERSION}).toInt()
        != SETTINGS_VERSION) {
        // Convert to new version here
        settings.setValue("Metadata/version", QVariant{SETTINGS_VERSION});
        settings.sync();
    }
    return true;
}

bool clearSettings(QSettings &settings)
{
    settings.clear();
    settings.setValue("Metadata/version", QVariant{SETTINGS_VERSION});
    settings.sync();
    return true;
}

QLocale getCurrentLocale()
{
    return currentLocale;
}

bool setCurrentLocale(QLocale &locale)
{
    currentLocale = locale;
    return true;
}

bool isCurrentLocaleHan()
{
    return currentLocale.script() == QLocale::HanScript
           || currentLocale.script() == QLocale::SimplifiedHanScript
           || currentLocale.script() == QLocale::TraditionalHanScript;
}

bool isCurrentLocaleTraditionalHan() {
    return currentLocale.script() == QLocale::TraditionalHanScript;
}

bool isCurrentLocaleSimplifiedHan() {
    return currentLocale.script() == QLocale::SimplifiedHanScript;
}

std::string getCurrentLocaleLanguageAndScriptIfChinese() {
    if (isCurrentLocaleHan()) {
        return currentLocale.bcp47Name().toStdString();
    } else {
        return currentLocale.bcp47Name().toStdString().substr(0,
            currentLocale.bcp47Name().toStdString().find_first_of("-"));
    }
}

}
