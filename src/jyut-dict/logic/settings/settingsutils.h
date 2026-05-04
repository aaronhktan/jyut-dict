#ifndef SETTINGSUTILS_H
#define SETTINGSUTILS_H

#include <QSettings>

#include <memory>
#include <string>

class QObject;
class QLocale;
class QTranslator;

// SettingsUtils should contain all non-STL functions related to settings

namespace Settings {

constexpr auto SETTINGS_VERSION = 3;

extern QTranslator systemTranslator;
extern QTranslator applicationTranslator;
extern QLocale currentLocale;

std::unique_ptr<QSettings> getSettings(QObject *parent = nullptr);
bool updateSettings(QSettings &settings);
bool clearSettings(QSettings &settings);

bool migrateSettingsFromOneToTwo(QSettings &settings);
bool migrateSettingsFromTwoToThree(QSettings &settings);

QLocale getCurrentLocale();
bool setCurrentLocale(const QLocale &locale);
bool isCurrentLocaleHan();
bool isCurrentLocaleTraditionalHan();
bool isCurrentLocaleSimplifiedHan();
std::string getCurrentLocaleLanguageAndScriptIfChinese();

};

#endif // SETTINGSUTILS_H
