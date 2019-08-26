#include "settingstab.h"

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/utils/utils.h"

#include <QApplication>
#include <QColorDialog>
#include <QFrame>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QStyle>
#include <QTimer>
#include <QVariant>

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget(parent)
{
    _settings = Settings::getSettings(this);
    setupUI();
    translateUI();
}

void SettingsTab::changeEvent(QEvent *event)
{
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(100, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        if (!system("defaults read -g AppleInterfaceStyle")) {
            setStyle(/* use_dark = */ true);
        } else {
            setStyle(/* use_dark = */ false);
        }
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
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
    _characterCombobox->setSizeAdjustPolicy(
        QComboBox::AdjustToContents);
    initializeCharacterComboBox(*_characterCombobox);
    _phoneticCombobox = new QComboBox{this};
    _phoneticCombobox->setSizeAdjustPolicy(
        QComboBox::AdjustToContents);
    initializePhoneticComboBox(*_phoneticCombobox);
    _mandarinCombobox = new QComboBox{this};
    _mandarinCombobox->setSizeAdjustPolicy(
        QComboBox::AdjustToContents);
    initializeMandarinComboBox(*_mandarinCombobox);

    QFrame *_divider = new QFrame{this};
    _divider->setObjectName("divider");
    _divider->setFrameShape(QFrame::HLine);
    _divider->setFrameShadow(QFrame::Raised);
    _divider->setFixedHeight(1);

    _colourCombobox = new QComboBox{this};
    _colourCombobox->setSizeAdjustPolicy(
        QComboBox::AdjustToContents);
    initializeColourComboBox(*_colourCombobox);
    _jyutpingColourWidget = new QWidget{this};
    initializeJyutpingColourWidget(*_jyutpingColourWidget);
    _pinyinColourWidget = new QWidget{this};
    initializePinyinColourWidget(*_pinyinColourWidget);

    QFrame *_resetDivider = new QFrame{this};
    _resetDivider->setObjectName("divider");
    _resetDivider->setFrameShape(QFrame::HLine);
    _resetDivider->setFrameShadow(QFrame::Raised);
    _resetDivider->setFixedHeight(1);

    _resetButton = new QPushButton{this};
    _resetButton->setObjectName("reset button");
    initializeResetButton(*_resetButton);

    _tabLayout->addRow(" ", _characterCombobox);
    _tabLayout->addRow(" ", _phoneticCombobox);
    _tabLayout->addRow(" ", _mandarinCombobox);

    _tabLayout->addRow(_divider);

    _tabLayout->addRow(" ", _colourCombobox);
    _tabLayout->addRow(" ", _jyutpingColourWidget);
    _tabLayout->addRow(" ", _pinyinColourWidget);

    _tabLayout->addRow(_resetDivider);

    _tabLayout->addRow(_resetButton);
    _tabLayout->setAlignment(_resetButton, Qt::AlignRight);

#ifdef Q_OS_MAC
    // Set the style to match whether the user started dark mode
    if (!system("defaults read -g AppleInterfaceStyle")) {
        setStyle(/* use_dark = */ true);
    } else {
        setStyle(/* use_dark = */ false);
    }
#else
    setStyle(false);
#endif
}

void SettingsTab::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    for (auto button : buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _characterCombobox->setItemText(0, tr("Only Simplified"));
    _characterCombobox->setItemText(1, tr("Only Traditional"));
    _characterCombobox->setItemText(2, tr("Both, Prefer Simplified"));
    _characterCombobox->setItemText(3, tr("Both, Prefer Traditional"));

    _phoneticCombobox->setItemText(0, tr("Only Jyutping"));
    _phoneticCombobox->setItemText(1, tr("Only Pinyin"));
    _phoneticCombobox->setItemText(2, tr("Both, Prefer Jyutping"));
    _phoneticCombobox->setItemText(3, tr("Both, Prefer Pinyin"));

    _mandarinCombobox->setItemText(0, tr("Pinyin with diacritics"));
    _mandarinCombobox->setItemText(1, tr("Pinyin with numbers"));

    _colourCombobox->setItemText(0, tr("No colours"));
    _colourCombobox->setItemText(1, tr("Jyutping"));
    _colourCombobox->setItemText(2, tr("Pinyin"));

    static_cast<QLabel *>(_tabLayout->labelForField(_characterCombobox))
        ->setText(tr("Simplified/Traditional display options:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_phoneticCombobox))
        ->setText(tr("Jyutping/Pinyin display options:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_mandarinCombobox))
        ->setText(tr("Pinyin display options:"));

    static_cast<QLabel *>(_tabLayout->labelForField(_colourCombobox))
        ->setText(tr("Colour words by tone using:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_jyutpingColourWidget))
        ->setText(tr("Jyutping tone colours:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_pinyinColourWidget))
        ->setText(tr("Pinyin tone colours:"));

    QList<QLabel *> jyutpingLabels = _jyutpingColourWidget
                                         ->findChildren<QLabel *>();
    for (QList<QLabel *>::size_type i = 0; i < jyutpingLabels.size(); i++) {
        switch (i) {
        case 0: {
            jyutpingLabels[i]->setText(tr("No Tone"));
            break;
        }
        default: {
            jyutpingLabels[i]->setText(tr("Tone %1").arg(i));
            break;
        }
        }
    }

    QList<QLabel *> pinyinLabels = _pinyinColourWidget->findChildren<QLabel *>();
    for (QList<QLabel *>::size_type i = 0; i < pinyinLabels.size(); i++) {
        switch (i) {
        case 0: {
            pinyinLabels[i]->setText(tr("No Tone"));
            break;
        }
        case 5: {
            pinyinLabels[i]->setText(tr("Neutral"));
            break;
        }
        default: {
            pinyinLabels[i]->setText(tr("Tone %1").arg(i));
            break;
        }
        }
    }

    _resetButton->setText(tr("Reset all settings"));
}

void SettingsTab::setStyle(bool use_dark)
{
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: "
                  "13px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet(
        "QPushButton[isHan=\"true\"] { font-size: 12px; height: 20px; }");
#endif

    QString colour = use_dark ? "#424242" : "#d5d5d5";
    QString style = "QFrame { border: 1px solid %1; }";
    QList<QFrame *> frames = this->findChildren<QFrame *>("divider");
    for (auto frame : frames) {
        frame->setStyleSheet(style.arg(colour));
    }
}

void SettingsTab::initializeCharacterComboBox(QComboBox &characterCombobox)
{
    characterCombobox.addItem("0",
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::ONLY_JYUTPING));
    characterCombobox.addItem("1",
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::ONLY_PINYIN));
    characterCombobox.addItem("2",
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::PREFER_JYUTPING));
    characterCombobox.addItem("3",
                              QVariant::fromValue<EntryPhoneticOptions>(
                                  EntryPhoneticOptions::PREFER_PINYIN));

    setCharacterComboBoxDefault(characterCombobox);

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
    phoneticCombobox.addItem("0",
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::ONLY_JYUTPING));
    phoneticCombobox.addItem("1",
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::ONLY_PINYIN));
    phoneticCombobox.addItem("2",
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::PREFER_JYUTPING));
    phoneticCombobox.addItem("3",
                             QVariant::fromValue<EntryPhoneticOptions>(
                                 EntryPhoneticOptions::PREFER_PINYIN));

    setPhoneticComboBoxDefault(phoneticCombobox);

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
    mandarinCombobox.addItem("0",
                             QVariant::fromValue<MandarinOptions>(
                                 MandarinOptions::PRETTY_PINYIN));
    mandarinCombobox.addItem("1",
                             QVariant::fromValue<MandarinOptions>(
                                 MandarinOptions::RAW_PINYIN));

    setMandarinComboBoxDefault(mandarinCombobox);

    connect(&mandarinCombobox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                _settings->setValue("mandarinOptions",
                                    mandarinCombobox.itemData(index));
                _settings->sync();
            });
}

void SettingsTab::initializeColourComboBox(QComboBox &colourCombobox)
{
    colourCombobox.addItem("0",
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::NONE));
    colourCombobox.addItem("1",
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::JYUTPING));
    colourCombobox.addItem("2",
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::PINYIN));

    setColourComboBoxDefault(colourCombobox);

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
    // Create buttons that show colours for jyutping tones
    QGridLayout *jyutpingLayout = new QGridLayout{&jyutpingColourWidget};
    jyutpingLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    jyutpingLayout->setContentsMargins(0, 0, 0, 0);
    jyutpingLayout->setVerticalSpacing(5);
    jyutpingColourWidget.setLayout(jyutpingLayout);

    // Create rounded rect colour buttons and label for tone
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::jyutpingToneColours.size();
         i++) {
        // First, add rounded rect colour button
        QPushButton *button = new QPushButton{this};
        button->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
            Settings::jyutpingToneColours[i].c_str()));
        button->setMinimumSize(40, 20);
        button->setFixedWidth(40);
        button->setProperty("tone", static_cast<int>(i));
        jyutpingLayout->addWidget(button,
                                  0,
                                  static_cast<int>(i),
                                  Qt::AlignCenter);

        connect(button, &QPushButton::clicked, this, [&]() {
            // Get new colour from dialog
            QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
            QColor newColour = getNewColour(sender->palette().button().color());
            if (!newColour.isValid()) {
                return;
            }

            // Save colour to both settings file and global jyutping config
            std::vector<std::string>::size_type index
                = static_cast<unsigned long>(sender->property("tone").toInt());
            Settings::jyutpingToneColours[index] = newColour.name().toStdString();
            sender->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
                Settings::jyutpingToneColours[index].c_str()));

            saveJyutpingColours();
        });

        // Then add tone label underneath it
        QLabel *label = new QLabel{&jyutpingColourWidget};
        label->setAlignment(Qt::AlignHCenter);
        label->setMinimumWidth(40);
        jyutpingLayout->addWidget(label, 1, static_cast<int>(i));
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
        Settings::pinyinToneColours[static_cast<unsigned long>(i)]
            = color.name().toStdString();
    }
    _settings->endArray();

    // Create widgets
    QGridLayout *pinyinLayout = new QGridLayout{&pinyinColourWidget};
    pinyinLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    pinyinLayout->setContentsMargins(0, 0, 0, 0);
    pinyinLayout->setVerticalSpacing(5);
    pinyinColourWidget.setLayout(pinyinLayout);
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::pinyinToneColours.size();
         i++) {
        QPushButton *button = new QPushButton{this};
        button->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
            Settings::pinyinToneColours[i].c_str()));
        button->setMinimumSize(40, 20);
        button->setFixedWidth(40);
        button->setProperty("tone", static_cast<int>(i));
        pinyinLayout->addWidget(button, 0, static_cast<int>(i), Qt::AlignCenter);

        connect(button, &QPushButton::clicked, this, [&]() {
            QPushButton *sender = static_cast<QPushButton *>(QObject::sender());
            QColor newColour = getNewColour(sender->palette().button().color());
            if (!newColour.isValid()) {
                return;
            }

            std::vector<std::string>::size_type index
                = static_cast<unsigned long>(sender->property("tone").toInt());
            Settings::pinyinToneColours[index] = newColour.name().toStdString();
            sender->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
                Settings::pinyinToneColours[index].c_str()));

            savePinyinColours();
        });

        QLabel *label = new QLabel{&pinyinColourWidget};
        label->setAlignment(Qt::AlignHCenter);
        label->setMinimumWidth(40);
        pinyinLayout->addWidget(label, 1, static_cast<int>(i));
    }
}

void SettingsTab::initializeResetButton(QPushButton &resetButton)
{
    connect(&resetButton, &QPushButton::clicked, [&]() {
        QMessageBox *_message = new QMessageBox{this};
        Qt::WindowFlags flags = _message->windowFlags()
                                | Qt::CustomizeWindowHint;
        flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
                   | Qt::WindowFullscreenButtonHint);
        _message->setWindowFlags(flags);
        _message->setAttribute(Qt::WA_DeleteOnClose, true);
        _message->setText(tr("Are you sure you want to reset all settings?"));
        _message->setInformativeText(tr("There is no way to restore them!"));
        _message->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        _message->setIcon(QMessageBox::Warning);
#ifdef Q_OS_WIN
        _message->setWindowTitle(
            QCoreApplication::translate(Strings::STRINGS_CONTEXT, Strings::PRODUCT_NAME));
#elif defined(Q_OS_LINUX)
        _message->setWindowTitle(" ");
#endif

        // Setting minimum width also doesn't work, so use this
        // workaround to set a width.
        QSpacerItem *horizontalSpacer = new QSpacerItem(400,
                                                        0,
                                                        QSizePolicy::Minimum,
                                                        QSizePolicy::Minimum);
        QGridLayout *layout = static_cast<QGridLayout *>(_message->layout());
        layout->addItem(horizontalSpacer,
                        layout->rowCount(),
                        0,
                        1,
                        layout->columnCount());

        if (_message->exec() == QMessageBox::Yes) {
            resetSettings(*_settings);
        }
    });

    resetButton.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

QColor SettingsTab::getNewColour(QColor old_colour)
{
    return QColorDialog::getColor(old_colour, this);
}

void SettingsTab::saveJyutpingColours()
{
    _settings->beginWriteArray("jyutpingColours");
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::jyutpingToneColours.size();
         i++) {
        _settings->setArrayIndex(static_cast<int>(i));
        _settings->setValue("colour",
                            QColor{Settings::jyutpingToneColours[i]
                                       .c_str()});
    }
    _settings->endArray();
    _settings->sync();
}

void SettingsTab::savePinyinColours()
{
    _settings->beginWriteArray("pinyinColours");
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::pinyinToneColours.size();
         i++) {
        _settings->setArrayIndex(static_cast<int>(i));
        _settings->setValue("colour",
                            QColor{
                                Settings::pinyinToneColours[i].c_str()});
    }
    _settings->endArray();
    _settings->sync();
}

void SettingsTab::resetSettings(QSettings &settings)
{
    Settings::clearSettings(settings);

    Settings::jyutpingToneColours = Settings::defaultJyutpingToneColours;
    Settings::pinyinToneColours = Settings::defaultPinyinToneColours;

    setCharacterComboBoxDefault(*_characterCombobox);
    setPhoneticComboBoxDefault(*_phoneticCombobox);
    setMandarinComboBoxDefault(*_mandarinCombobox);
    setColourComboBoxDefault(*_colourCombobox);
    setJyutpingColourWidgetDefault(*_jyutpingColourWidget);
    setPinyinColourWidgetDefault(*_pinyinColourWidget);
}

void SettingsTab::setCharacterComboBoxDefault(QComboBox &characterCombobox)
{
    characterCombobox.setCurrentIndex(characterCombobox.findData(
        _settings->value("characterOptions",
                         QVariant::fromValue(
                             EntryCharactersOptions::PREFER_TRADITIONAL))));
}

void SettingsTab::setPhoneticComboBoxDefault(QComboBox &phoneticCombobox)
{
    phoneticCombobox.setCurrentIndex(phoneticCombobox.findData(
        _settings->value("phoneticOptions",
                         QVariant::fromValue(
                             EntryPhoneticOptions::PREFER_JYUTPING))));
}

void SettingsTab::setMandarinComboBoxDefault(QComboBox &mandarinCombobox)
{
    mandarinCombobox.setCurrentIndex(mandarinCombobox.findData(
        _settings->value("mandarinOptions",
                         QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))));
}

void SettingsTab::setColourComboBoxDefault(QComboBox &colourCombobox)
{
    colourCombobox.setCurrentIndex(colourCombobox.findData(
        _settings->value("entryColourPhoneticType",
                         QVariant::fromValue(
                             EntryColourPhoneticType::JYUTPING))));
}

void SettingsTab::setJyutpingColourWidgetDefault(QWidget &jyutpingColourWidget)
{
    QList<QPushButton *> buttons = jyutpingColourWidget
                                       .findChildren<QPushButton *>();
    for (QList<QPushButton *>::size_type i = 0; i < buttons.size(); i++) {
        buttons[i]->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
            Settings::jyutpingToneColours[static_cast<unsigned long>(i)]
                .c_str()));
    }
}

void SettingsTab::setPinyinColourWidgetDefault(QWidget &pinyinColourWidget)
{
    QList<QPushButton *> buttons = pinyinColourWidget
                                       .findChildren<QPushButton *>();
    for (QList<QPushButton *>::size_type i = 0; i < buttons.size(); i++) {
        buttons[i]->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
            Settings::pinyinToneColours[static_cast<unsigned long>(i)]
                .c_str()));
    }
}
