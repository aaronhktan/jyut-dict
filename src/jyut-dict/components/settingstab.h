#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include "logic/settings/settings.h"

#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QWidget>

#include <memory>

// The settings tab displays settings to a user.

class SettingsTab : public QWidget
{
public:
    explicit SettingsTab(QWidget *parent = nullptr);

private:
    void setupUI();
    void initializeCharacterComboBox(QComboBox &characterCombobox);
    void initializePhoneticComboBox(QComboBox &phoneticCombobox);
    void initializeMandarinComboBox(QComboBox &mandarinCombobox);
    //    void initializeLanguageComboBox(QComboBox &languageCombobox);

    QComboBox *_characterCombobox;
    QComboBox *_phoneticCombobox;
    QComboBox *_mandarinCombobox;
    //    QComboBox *_languageCombobox;

    QFrame *_divider;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;
};

#endif // SETTINGSTAB_H
