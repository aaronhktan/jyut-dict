#include "settingstab.h"

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settings.h"

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget(parent)
{
    _settings = Settings::getSettings(this);
    setupUI();
}

void SettingsTab::setupUI()
{
    _tabLayout = new QFormLayout{this};

    _characterCombobox = new QComboBox{this};
    initializeCharacterComboBox(*_characterCombobox);
    _phoneticCombobox = new QComboBox{this};
    initializePhoneticComboBox(*_phoneticCombobox);
    _mandarinCombobox = new QComboBox{this};
    initializeMandarinComboBox(*_mandarinCombobox);
    //    _languageCombobox = new QComboBox{this};

    _divider = new QFrame{this};
    _divider->setFrameShape(QFrame::HLine);
    _divider->setFrameShadow(QFrame::Raised);
    _divider->setFixedHeight(1);

    _tabLayout->addRow(tr("Simplified/Traditional display options:"),
                       _characterCombobox);
    _tabLayout->addRow(tr("Jyutping/Pinyin display options:"),
                       _phoneticCombobox);
    _tabLayout->addRow(tr("Pinyin display options:"), _mandarinCombobox);
    _tabLayout->addRow(_divider);
    //    _tabLayout->addRow(tr("Interface language:"), _languageCombobox);
}

void SettingsTab::initializeCharacterComboBox(QComboBox &characterCombobox)
{
    characterCombobox.addItem(tr("Only Simplified"),
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::ONLY_JYUTPING));
    characterCombobox.addItem(tr("Only Traditional"),
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::ONLY_PINYIN));
    characterCombobox.addItem(tr("Both, Prefer Simplified"),
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::PREFER_JYUTPING));
    characterCombobox.addItem(tr("Both, Prefer Traditional"),
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::PREFER_PINYIN));

    characterCombobox.setCurrentIndex(characterCombobox.findData(
        _settings->value("characterOptions",
                         QVariant::fromValue(
                             EntryCharactersOptions::PREFER_TRADITIONAL))));

    connect(&characterCombobox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                _settings->setValue("characterOptions",
                                    characterCombobox.itemData(index));
                _settings->sync();
            });
}

void SettingsTab::initializePhoneticComboBox(QComboBox &phoneticCombobox)
{
    phoneticCombobox.addItem(tr("Only Jyutping"),
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::ONLY_JYUTPING));
    phoneticCombobox.addItem(tr("Only Pinyin"),
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::ONLY_PINYIN));
    phoneticCombobox.addItem(tr("Both, Prefer Jyutping"),
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::PREFER_JYUTPING));
    phoneticCombobox.addItem(tr("Both, Prefer Pinyin"),
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::PREFER_PINYIN));

    phoneticCombobox.setCurrentIndex(phoneticCombobox.findData(
        _settings->value("phoneticOptions",
                         QVariant::fromValue(
                             EntryPhoneticOptions::PREFER_JYUTPING))));

    connect(&phoneticCombobox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                _settings->setValue("phoneticOptions",
                                    phoneticCombobox.itemData(index));
                _settings->sync();
            });
}

void SettingsTab::initializeMandarinComboBox(QComboBox &mandarinCombobox)
{
    mandarinCombobox.addItem(tr("Pinyin with diacritics"),
                             QVariant::fromValue<MandarinOptions>(
                                 MandarinOptions::PRETTY_PINYIN));
    mandarinCombobox.addItem(tr("Pinyin with numbers"),
                             QVariant::fromValue<MandarinOptions>(
                                 MandarinOptions::RAW_PINYIN));

    mandarinCombobox.setCurrentIndex(mandarinCombobox.findData(
        _settings->value("mandarinOptions",
                         QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))));

    connect(&mandarinCombobox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                _settings->setValue("mandarinOptions",
                                    mandarinCombobox.itemData(index));
                _settings->sync();
            });
}

//void SettingsTab::initializeLanguageComboBox(QComboBox &languageCombobox)
//{
//    phoneticCombobox.addItem(tr("Only Jyutping"),
//                             QVariant::fromValue<EntryPhoneticOptions>(
//                                 EntryPhoneticOptions::ONLY_JYUTPING));
//    phoneticCombobox.addItem(tr("Only Pinyin"),
//                             QVariant::fromValue<EntryPhoneticOptions>(
//                                 EntryPhoneticOptions::ONLY_PINYIN));
//    phoneticCombobox.addItem(tr("Both, Prefer Jyutping"),
//                             QVariant::fromValue<EntryPhoneticOptions>(
//                                 EntryPhoneticOptions::PREFER_JYUTPING));
//    phoneticCombobox.addItem(tr("Both, Prefer Pinyin"),
//                             QVariant::fromValue<EntryPhoneticOptions>(
//                                 EntryPhoneticOptions::PREFER_PINYIN));

//    phoneticCombobox.setCurrentIndex(phoneticCombobox.findData(
//        _settings
//            ->value("phonetic_options",
//                    QVariant::fromValue(
//                        QVariant::fromValue(MandarinOptions::RAW_PINYIN)))
//            .value<MandarinOptions>()));

//    connect(&phoneticCombobox,
//            QOverload<int>::of(&QComboBox::currentIndexChanged),
//            this,
//            [&](int index) {
//                _settings->setValue("phonetic_options",
//                                    phoneticCombobox.itemData(index));
//                _settings->sync();
//            });
//}
