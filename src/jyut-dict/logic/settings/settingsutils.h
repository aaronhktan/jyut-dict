#ifndef SETTINGSUTILS_H
#define SETTINGSUTILS_H

#include <QLocale>
#include <QObject>
#include <QSettings>
#include <QTranslator>

#include <memory>

// SettingsUtils should contain all non-STL functions related to settings

namespace Settings {

constexpr auto SETTINGS_VERSION = 1;

extern QTranslator systemTranslator;
extern QTranslator applicationTranslator;
extern QLocale currentLocale;

std::unique_ptr<QSettings> getSettings(QObject *parent = nullptr);
bool updateSettings(QSettings &settings);
bool clearSettings(QSettings &settings);

QLocale getCurrentLocale();
bool setCurrentLocale(QLocale &locale);
bool isCurrentLocaleHan();
bool isCurrentLocaleTraditionalHan();
bool isCurrentLocaleSimplifiedHan();

};

#endif // SETTINGSUTILS_H
