#ifndef SETTINGSUTILS_H
#define SETTINGSUTILS_H

#include <QObject>
#include <QSettings>

#include <memory>

namespace Settings {

std::unique_ptr<QSettings> getSettings(QObject *parent = nullptr);

};

#endif // SETTINGSUTILS_H
