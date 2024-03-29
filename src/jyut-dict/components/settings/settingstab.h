#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

// The settings tab displays settings to a user.

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

    void initializePreviewPhonetic(QWidget &previewPhoneticWidget);
    void initializeSearchResultsCantonesePronunciation(
        QWidget &cantonesePronunciationWidget);
    void initializeSearchResultsMandarinPronunciation(
        QWidget &mandarinPronunciationWidget);

    void initializeEntryCantonesePronunciation(QWidget &cantonesePronunciationWidget);
    void initializeEntryMandarinPronunciation(QWidget &mandarinPronunciationWidget);

    void setPhoneticComboboxDefault(QComboBox &phoneticCombobox);
    void setSearchResultsCantonesePronunciationDefault(QWidget &widget);
    void setSearchResultsMandarinPronunciationDefault(QWidget &widget);

    void setEntryCantonesePronunciationDefault(QWidget &widget);
    void setEntryMandarinPronunciationDefault(QWidget &widget);

    bool _paletteRecentlyChanged = false;

    std::unique_ptr<QSettings> _settings;

    QLabel *_previewTitleLabel;
    QWidget *_previewPhoneticWidget;
    QHBoxLayout *_previewPhoneticLayout;
    QComboBox *_previewPhoneticCombobox;
    QWidget *_previewCantonesePronunciation;
    QHBoxLayout *_previewCantonesePronunciationLayout;
    QRadioButton *_previewJyutping;
    QRadioButton *_previewYale;
    QRadioButton *_previewCantoneseIPA;
    QWidget *_previewMandarinPronunciation;
    QHBoxLayout *_previewMandarinPronunciationLayout;
    QRadioButton *_previewPinyin;
    QRadioButton *_previewNumberedPinyin;
    QRadioButton *_previewZhuyin;
    QRadioButton *_previewMandarinIPA;

    QLabel *_entryTitleLabel;
    QWidget *_entryCantonesePronunciation;
    QGridLayout *_entryCantonesePronunciationLayout;
    QCheckBox *_entryJyutping;
    QCheckBox *_entryYale;
    QCheckBox *_entryCantoneseIPA;
    QWidget *_entryMandarinPronunciation;
    QGridLayout *_entryMandarinPronunciationLayout;
    QCheckBox *_entryPinyin;
    QCheckBox *_entryNumberedPinyin;
    QCheckBox *_entryZhuyin;
    QCheckBox *_entryMandarinIPA;

    QLabel *_cantoneseReference;
    QLabel *_learnJyutping;
    QLabel *_learnYale;
    QLabel *_learnCantoneseIPA;
    QLabel *_mandarinReference;
    QLabel *_learnPinyin;
    QLabel *_learnZhuyin;
    QLabel *_learnMandarinIPA;

    QFormLayout *_tabLayout;

signals:
    void updateStyle(void);

public slots:
    void resetSettings(void);
};

#endif // SETTINGSTAB_H
