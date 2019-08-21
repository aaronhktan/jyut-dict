#ifndef SETTINGSUTILS_H
#define SETTINGSUTILS_H

#include <QObject>
#include <QSettings>

#include <memory>

// SettingsUtils should contain all non-STL functions related to settings

namespace Settings {

constexpr auto SETTINGS_VERSION = 1;

std::unique_ptr<QSettings> getSettings(QObject *parent = nullptr);
bool updateSettings(QSettings &settings);

};

#endif // SETTINGSUTILS_H
