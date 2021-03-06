#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include "dialogs/resetsettingsdialog.h"
#include "logic/settings/settingsutils.h"

#include <QColor>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
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
    Q_OBJECT

public:
    explicit SettingsTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();
    void setStyle(bool use_dark);

    void initializeCharacterComboBox(QComboBox &characterCombobox);
    void initializePhoneticComboBox(QComboBox &phoneticCombobox);
    void initializeMandarinComboBox(QComboBox &mandarinCombobox);

    void initializeColourComboBox(QComboBox &colourCombobox);
    void initializeJyutpingColourWidget(QWidget &jyutpingColourWidget);
    void initializePinyinColourWidget(QWidget &pinyinColourWidget);

    void initializeResetButton(QPushButton &resetButton);

    QColor getNewColour(QColor old_colour);

    void saveJyutpingColours();
    void savePinyinColours();

    void resetSettings(QSettings &settings);

    void setCharacterComboBoxDefault(QComboBox &characterCombobox);
    void setPhoneticComboBoxDefault(QComboBox &phoneticCombobox);
    void setMandarinComboBoxDefault(QComboBox &mandarinCombobox);

    void setColourComboBoxDefault(QComboBox &colourCombobox);
    void setJyutpingColourWidgetDefault(QWidget &jyutpingColourWidget);
    void setPinyinColourWidgetDefault(QWidget &pinyinColourWidget);

    bool _paletteRecentlyChanged = false;

    QComboBox *_characterCombobox;
    QComboBox *_phoneticCombobox;
    QComboBox *_mandarinCombobox;

    QComboBox *_colourCombobox;
    QWidget *_jyutpingColourWidget;
    QWidget *_pinyinColourWidget;

    QPushButton *_resetButton;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;
};

#endif // SETTINGSTAB_H
