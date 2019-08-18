#include "settingstab.h"

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"

#include <QColorDialog>
#include <QGridLayout>
#include <QVariant>

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget(parent)
{
    _settings = Settings::getSettings(this);
    setupUI();
}

void SettingsTab::setupUI()
{
    _tabLayout = new QFormLayout{this};
#ifdef Q_OS_WIN
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
#elif defined(Q_OS_LINUX)
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
    _tabLayout->setLabelAlignment(Qt::AlignRight);
#endif

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

    _colourCombobox = new QComboBox{this};
    initializeColourComboBox(*_colourCombobox);
    QWidget *jyutpingColourWidget = new QWidget{this};
    initializeJyutpingColourWidget(*jyutpingColourWidget);
    QWidget *pinyinColourWidget = new QWidget{this};
    initializePinyinColourWidget(*pinyinColourWidget);

    _tabLayout->addRow(tr("Simplified/Traditional display options:"),
                       _characterCombobox);
    _tabLayout->addRow(tr("Jyutping/Pinyin display options:"),
                       _phoneticCombobox);
    _tabLayout->addRow(tr("Pinyin display options:"), _mandarinCombobox);

    _tabLayout->addRow(_divider);

    _tabLayout->addRow(tr("Colour words by tone in:"), _colourCombobox);
    _tabLayout->addRow(tr("Jyutping tone colours:"), jyutpingColourWidget);
    _tabLayout->addRow(tr("Pinyin tone colours:"), pinyinColourWidget);

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

void SettingsTab::initializeColourComboBox(QComboBox &colourCombobox)
{
    colourCombobox.addItem(tr("Jyutping"),
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::JYUTPING));
    colourCombobox.addItem(tr("Pinyin"),
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::PINYIN));

    colourCombobox.setCurrentIndex(colourCombobox.findData(
        _settings->value("entryColourPhoneticType",
                         QVariant::fromValue(
                             EntryColourPhoneticType::JYUTPING))));

    connect(&colourCombobox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                _settings->setValue("entryColourPhoneticType",
                                    colourCombobox.itemData(index));
                _settings->sync();
            });
}

void SettingsTab::initializeJyutpingColourWidget(QWidget &jyutpingColourWidget)
{
    // Create widgets
    QGridLayout *jyutpingLayout = new QGridLayout{&jyutpingColourWidget};
    jyutpingLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    jyutpingLayout->setContentsMargins(0, 0, 0, 0);
    jyutpingLayout->setVerticalSpacing(5);
    jyutpingColourWidget.setLayout(jyutpingLayout);
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::jyutpingToneColours.size();
         i++) {
        QPushButton *button = new QPushButton{this};
        button->setStyleSheet(
            QString{"QPushButton { "
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
                    "} "}
                .arg(Settings::jyutpingToneColours[i].c_str()));
        button->setMinimumSize(40, 20);
        button->setFixedWidth(40);
        button->setProperty("tone", static_cast<int>(i));
        jyutpingLayout->addWidget(button,
                                  0,
                                  static_cast<int>(i),
                                  Qt::AlignCenter);

        connect(button, &QPushButton::clicked, this, [&]() {
            QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
            QColor newColour = QColorDialog::getColor(static_cast<QPushButton *>(
                                                          QObject::sender())
                                                          ->palette()
                                                          .button()
                                                          .color(),
                                                      this);
            if (!newColour.isValid()) {
                return;
            }
            Settings::jyutpingToneColours[sender->property("tone").toInt()]
                = newColour.name().toStdString();
            sender->setStyleSheet(
                QString{"QPushButton { "
                        "   background: %1; border: 1px solid darkgrey; "
                        "   border-radius: 3px; "
                        "   margin: 0px; "
                        "   padding: 0px; "
                        "} "
                        " "
                        "QPushButton:pressed { "
                        "   background: %1; border: 1px solid lightgrey; "
                        "   border-radius: 3px; "
                        "   margin: 0px; "
                        "   padding: 0px; "
                        "} "}
                    .arg(Settings::jyutpingToneColours[sender->property("tone")
                                                           .toInt()]
                             .c_str()));

            _settings->beginWriteArray("jyutpingColours");
            for (std::vector<std::string>::size_type i = 0;
                 i < Settings::jyutpingToneColours.size();
                 i++) {
                _settings->setArrayIndex(i);
                _settings->setValue("colour",
                                    QColor{Settings::jyutpingToneColours[i]
                                               .c_str()});
            }
            _settings->endArray();
            _settings->sync();
        });

        if (i == 0) {
            jyutpingLayout->addWidget(new QLabel{tr("No Tone"),
                                                 &jyutpingColourWidget},
                                      1,
                                      static_cast<int>(i));
            continue;
        }

        jyutpingLayout->addWidget(new QLabel{tr("Tone %1").arg(i),
                                             &jyutpingColourWidget},
                                  1,
                                  static_cast<int>(i));
    }
}

void SettingsTab::initializePinyinColourWidget(QWidget &pinyinColourWidget)
{
    // Get colours from QSettings
    _settings->beginReadArray("pinyinColors");
    for (int i = 0; i < 5; ++i) {
        _settings->setArrayIndex(i);
        QColor color
            = _settings
                  ->value("colour",
                          QColor{
                              Settings::pinyinToneColours[static_cast<ulong>(i)]
                                  .c_str()})
                  .value<QColor>();
        Settings::pinyinToneColours[i] = color.name().toStdString();
    }
    _settings->endArray();

    // Create widgets
    QGridLayout *pinyinLayout = new QGridLayout{&pinyinColourWidget};
    pinyinLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    pinyinLayout->setContentsMargins(0, 0, 0, 0);
    pinyinLayout->setVerticalSpacing(5);
    pinyinColourWidget.setLayout(pinyinLayout);
    for (std::vector<std::string>::size_type i = 0; i < Settings::pinyinToneColours.size(); i++) {
        QPushButton *button = new QPushButton{this};
        button->setStyleSheet(
            QString{"QPushButton { "
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
                    "} "}
                .arg(Settings::pinyinToneColours[i].c_str()));
        button->setMinimumSize(40, 20);
        button->setFixedWidth(40);
        button->setProperty("tone", static_cast<int>(i));
        pinyinLayout->addWidget(button, 0, static_cast<int>(i), Qt::AlignCenter);

        connect(button, &QPushButton::clicked, this, [&]() {
            QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
            QColor newColour = QColorDialog::getColor(static_cast<QPushButton *>(
                                                          QObject::sender())
                                                          ->palette()
                                                          .button()
                                                          .color(),
                                                      this);
            if (!newColour.isValid()) {
                return;
            }
            Settings::pinyinToneColours[sender->property("tone").toInt()]
                = newColour.name().toStdString();
            sender->setStyleSheet(
                QString{"QPushButton { "
                        "   background: %1; border: 1px solid darkgrey; "
                        "   border-radius: 3px; "
                        "   margin: 0px; "
                        "   padding: 0px; "
                        "} "
                        " "
                        "QPushButton:pressed { "
                        "   background: %1; border: 1px solid lightgrey; "
                        "   border-radius: 3px; "
                        "   margin: 0px; "
                        "   padding: 0px; "
                        "} "}
                    .arg(Settings::pinyinToneColours[sender->property("tone")
                                                         .toInt()]
                             .c_str()));

            _settings->beginWriteArray("pinyinColours");
            for (std::vector<std::string>::size_type i = 0;
                 i < Settings::pinyinToneColours.size();
                 i++) {
                _settings->setArrayIndex(i);
                _settings->setValue("colour",
                                    QColor{
                                        Settings::pinyinToneColours[i].c_str()});
            }
            _settings->endArray();
            _settings->sync();
        });

        if (i == 0) {
            pinyinLayout->addWidget(new QLabel{tr("No Tone"),
                                               &pinyinColourWidget},
                                    1,
                                    static_cast<int>(i));
            continue;
        }

        if (i == 5) {
            pinyinLayout->addWidget(new QLabel{tr("Neutral"),
                                               &pinyinColourWidget},
                                    1,
                                    static_cast<int>(i));
            continue;
        }
        pinyinLayout->addWidget(new QLabel{tr("Tone %1").arg(i),
                                           &pinyinColourWidget},
                                1,
                                static_cast<int>(i));
    }
}
