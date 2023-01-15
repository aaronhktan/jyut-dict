#ifndef TEXTTAB_H
#define TEXTTAB_H

#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QWidget>

constexpr auto COLOUR_BUTTON_STYLE
    = "QPushButton { "
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

class TextTab : public QWidget
{
    Q_OBJECT
public:
    explicit TextTab(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void translateUI();
    void setStyle(bool use_dark);

    void initializeCharacterComboBox(QComboBox &characterCombobox);

    void initializeInterfaceSizeWidget(QWidget &widget);

    void initializeColourComboBox(QComboBox &colourCombobox);
    void initializeJyutpingColourWidget(QWidget &jyutpingColourWidget);
    void initializePinyinColourWidget(QWidget &pinyinColourWidget);

    void setCharacterComboBoxDefault(QComboBox &characterCombobox);

    void setInterfaceSizeWidgetDefault(QWidget &widget);

    void setColourComboBoxDefault(QComboBox &colourCombobox);
    void setJyutpingColourWidgetDefault(QWidget &jyutpingColourWidget);
    void setPinyinColourWidgetDefault(QWidget &pinyinColourWidget);

    QColor getNewColour(QColor old_colour);

    void saveJyutpingColours();
    void savePinyinColours();

    bool _paletteRecentlyChanged = false;

    QLabel *_characterTitleLabel;
    QComboBox *_characterCombobox;

    QLabel *_interfaceSizeTitleLabel;
    QWidget *_interfaceSizeWidget;
    QGridLayout *_interfaceSizeLayout;
    QSlider *_interfaceSizeSlider;
    QLabel *_interfaceSizeSmallLabel;
    QLabel *_interfaceSizeLargeLabel;

    QLabel *_colourTitleLabel;
    QComboBox *_colourCombobox;
    QWidget *_jyutpingColourWidget;
    QWidget *_pinyinColourWidget;

    QFormLayout *_tabLayout;

    std::unique_ptr<QSettings> _settings;

public slots:
    void resetSettings(void);
};

#endif // TEXTTAB_H
