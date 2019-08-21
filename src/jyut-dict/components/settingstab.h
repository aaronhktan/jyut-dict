#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include "logic/settings/settingsutils.h"

#include <QColor>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <memory>
#include <string>
#include <vector>

// The settings tab displays settings to a user.

constexpr auto COLOUR_BUTTON_STYLE = "QPushButton { "
                                     "   background: %1; border: 1px solid darkgrey; "
                                     "   border-radius: 3px; "
                                     "   margin: 0px; "
                                     "   padding: 0px; "
                                     "} "
                                     " "
                                     "QPushButton:pressed { "
                                     "   background: %1; border: 2px solid lightgrey; "
                                     "   border-radius: 3px; "
                                     "   margin: 0px; "
                                     "   padding: 0px; "
                                     "} ";

class SettingsTab : public QWidget
{
public:
    explicit SettingsTab(QWidget *parent = nullptr);

private:
    void setupUI();
    void initializeCharacterComboBox(QComboBox &characterCombobox);
    void initializePhoneticComboBox(QComboBox &phoneticCombobox);
    void initializeMandarinComboBox(QComboBox &mandarinCombobox);

    void initializeColourComboBox(QComboBox &colourCombobox);
    void initializeJyutpingColourWidget(QWidget &jyutpingColourWidget);
    void initializePinyinColourWidget(QWidget &pinyinColourWidget);

    QColor getNewColour(QColor old_colour);

    void saveJyutpingColours();
    void savePinyinColours();

    QComboBox *_characterCombobox;
    QComboBox *_phoneticCombobox;
    QComboBox *_mandarinCombobox;

    QFrame *_divider;

    QComboBox *_colourCombobox;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;
};

#endif // SETTINGSTAB_H
