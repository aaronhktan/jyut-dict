#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

#include <memory>

namespace Settings {

std::unique_ptr<QSettings> getSettings(QObject *parent = nullptr);

};

#endif // SETTINGS_H
