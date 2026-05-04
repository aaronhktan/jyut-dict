#ifndef TEXTTAB_H
#define TEXTTAB_H

#include <QSettings>
#include <QWidget>

class QCheckBox;
class QComboBox;
class QEvent;
class QFormLayout;
class QGridLayout;
class QLabel;
class QPushButton;
class QSlider;

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

signals:
    void updateStyle(void);

public slots:
    void resetSettings(void);
};

#endif // TEXTTAB_H
