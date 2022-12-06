#include "settingstab.h"

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settings.h"
#include "logic/utils/utils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QApplication>
#include <QColorDialog>
#include <QFrame>
#include <QGridLayout>
#include <QStyle>
#include <QTimer>
#include <QVariant>

SettingsTab::SettingsTab(QWidget *parent)
    : QWidget{parent}
{
    _settings = Settings::getSettings(this);
    setupUI();
    translateUI();
}

void SettingsTab::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void SettingsTab::setupUI()
{
    _tabLayout = new QFormLayout{this};
#ifdef Q_OS_MAC
    _tabLayout->setContentsMargins(20, 20, 20, 20);
#elif defined(Q_OS_WIN)
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
#elif defined(Q_OS_LINUX)
    _tabLayout->setVerticalSpacing(15);
    _tabLayout->setContentsMargins(20, 20, 20, 20);
    _tabLayout->setLabelAlignment(Qt::AlignRight);
#endif

    _characterTitleLabel = new QLabel{this};
    _characterCombobox = new QComboBox{this};
    _characterCombobox->setSizeAdjustPolicy(
        QComboBox::AdjustToContents);
    initializeCharacterComboBox(*_characterCombobox);

    QFrame *_searchResultsDivider = new QFrame{this};
    _searchResultsDivider->setObjectName("divider");
    _searchResultsDivider->setFrameShape(QFrame::HLine);
    _searchResultsDivider->setFrameShadow(QFrame::Raised);
    _searchResultsDivider->setFixedHeight(1);

    _previewTitleLabel = new QLabel{this};

    _previewPhoneticWidget = new QWidget{this};
    _previewPhoneticLayout = new QHBoxLayout{_previewPhoneticWidget};
    _previewPhoneticLayout->setContentsMargins(0, 0, 0, 0);
    _previewPhoneticCombobox = new QComboBox{this};
    _previewPhoneticCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initializePreviewPhonetic(*_previewPhoneticWidget);

    _previewCantonesePronunciation = new QWidget{this};
    _previewCantonesePronunciationLayout = new QHBoxLayout{
        _previewCantonesePronunciation};
    _previewCantonesePronunciationLayout->setContentsMargins(0, 0, 0, 0);
    _previewJyutping = new QRadioButton{this};
    _previewJyutping->setProperty("data",
                                  QVariant::fromValue(
                                      CantoneseOptions::RAW_JYUTPING));
    _previewYale = new QRadioButton{this};
    _previewYale->setProperty("data",
                              QVariant::fromValue(
                                  CantoneseOptions::PRETTY_YALE));
    initializeSearchResultsCantonesePronunciation(
        *_previewCantonesePronunciation);

    _previewMandarinPronunciation = new QWidget{this};
    _previewMandarinPronunciationLayout = new QHBoxLayout{
        _previewMandarinPronunciation};
    _previewMandarinPronunciationLayout->setContentsMargins(0, 0, 0, 0);
    _previewPinyin = new QRadioButton{this};
    _previewPinyin->setProperty("data",
                                QVariant::fromValue(
                                    MandarinOptions::PRETTY_PINYIN));
    _previewNumberedPinyin = new QRadioButton{this};
    _previewNumberedPinyin->setProperty("data",
                                        QVariant::fromValue(
                                            MandarinOptions::NUMBERED_PINYIN));
    initializeSearchResultsMandarinPronunciation(*_previewMandarinPronunciation);

    QFrame *_entryDivider = new QFrame{this};
    _entryDivider->setObjectName("divider");
    _entryDivider->setFrameShape(QFrame::HLine);
    _entryDivider->setFrameShadow(QFrame::Raised);
    _entryDivider->setFixedHeight(1);

    _entryTitleLabel = new QLabel{this};

    _entryCantonesePronunciation = new QWidget{this};
    _entryCantonesePronunciationLayout = new QVBoxLayout{
        _entryCantonesePronunciation};
    _entryCantonesePronunciationLayout->setContentsMargins(0, 0, 0, 0);
    _entryJyutping = new QCheckBox{this};
    _entryJyutping->setTristate(false);
    _entryJyutping->setProperty("data",
                                QVariant::fromValue(
                                    CantoneseOptions::RAW_JYUTPING));
    _entryYale = new QCheckBox{this};
    _entryYale->setTristate(false);
    _entryYale->setProperty("data",
                            QVariant::fromValue(CantoneseOptions::PRETTY_YALE));
    initializeEntryCantonesePronunciation(*_entryCantonesePronunciation);

    _entryMandarinPronunciation = new QWidget{this};
    _entryMandarinPronunciationLayout = new QVBoxLayout{
        _entryMandarinPronunciation};
    _entryMandarinPronunciationLayout->setContentsMargins(0, 0, 0, 0);
    _entryPinyin = new QCheckBox{this};
    _entryPinyin->setTristate(false);
    _entryPinyin->setProperty("data",
                              QVariant::fromValue(
                                  MandarinOptions::PRETTY_PINYIN));
    _entryNumberedPinyin = new QCheckBox{this};
    _entryNumberedPinyin->setTristate(false);
    _entryNumberedPinyin->setProperty("data",
                                 QVariant::fromValue(
                                     MandarinOptions::NUMBERED_PINYIN));
    initializeEntryMandarinPronunciation(*_entryMandarinPronunciation);

    QFrame *_divider = new QFrame{this};
    _divider->setObjectName("divider");
    _divider->setFrameShape(QFrame::HLine);
    _divider->setFrameShadow(QFrame::Raised);
    _divider->setFixedHeight(1);

    _colourTitleLabel = new QLabel{this};
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

    _tabLayout->addRow(_characterTitleLabel);
    _tabLayout->addRow(" ", _characterCombobox);

    _tabLayout->addRow(_searchResultsDivider);

    _tabLayout->addRow(_previewTitleLabel);
    _tabLayout->addRow(" ", _previewPhoneticWidget);
    _tabLayout->addRow(" ", _previewCantonesePronunciation);
    _tabLayout->addRow(" ", _previewMandarinPronunciation);

    _tabLayout->addRow(_entryDivider);

    _tabLayout->addRow(_entryTitleLabel);
    _tabLayout->addRow(" ", _entryCantonesePronunciation);
    _tabLayout->addRow(" ", _entryMandarinPronunciation);

    _tabLayout->addRow(_divider);

    _tabLayout->addRow(_colourTitleLabel);
    _tabLayout->addRow(" ", _colourCombobox);
    _tabLayout->addRow(" ", _jyutpingColourWidget);
    _tabLayout->addRow(" ", _pinyinColourWidget);

    _tabLayout->addRow(_resetDivider);

    _tabLayout->addRow(_resetButton);
    _tabLayout->setAlignment(_resetButton, Qt::AlignRight);

    // Set the style to match whether the user started dark mode
    setStyle(Utils::isDarkMode());
}

void SettingsTab::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto & button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _characterTitleLabel->setText(
        "<b>" + tr("Character set:") + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_characterCombobox))
        ->setText(tr("\tShow simplified/traditional characters:"));
    _characterCombobox->setItemText(0, tr("Only Simplified"));
    _characterCombobox->setItemText(1, tr("Only Traditional"));
    _characterCombobox->setItemText(2, tr("Both, Prefer Simplified"));
    _characterCombobox->setItemText(3, tr("Both, Prefer Traditional"));

    _previewTitleLabel->setText("<b>" + tr("In search results and examples:")
                                + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_previewPhoneticWidget))
        ->setText(tr("Show pronunciation for:"));
    _previewPhoneticCombobox->setItemText(0, tr("only Cantonese"));
    _previewPhoneticCombobox->setItemText(1, tr("only Mandarin"));
    _previewPhoneticCombobox
        ->setItemText(2, tr("both languages, prefer Cantonese"));
    _previewPhoneticCombobox
        ->setItemText(3, tr("both languages, prefer Mandarin"));
    static_cast<QLabel *>(
        _tabLayout->labelForField(_previewCantonesePronunciation))
        ->setText(tr("Show Cantonese:"));
    _previewJyutping->setText(tr("Jyutping"));
    _previewYale->setText(tr("Yale"));
    static_cast<QLabel *>(
        _tabLayout->labelForField(_previewMandarinPronunciation))
        ->setText(tr("Show Mandarin:"));
    _previewPinyin->setText(tr("Pinyin"));
    _previewNumberedPinyin->setText(tr("Pinyin with digits"));

    _entryTitleLabel->setText(
        "<b>" + tr("In the header at the top of an entry:") + "</b>");
    static_cast<QLabel *>(
        _tabLayout->labelForField(_entryCantonesePronunciation))
        ->setText(tr("Show Cantonese:"));
    _entryJyutping->setText(tr("Jyutping"));
    _entryYale->setText(tr("Yale"));
    static_cast<QLabel *>(_tabLayout->labelForField(_entryMandarinPronunciation))
        ->setText(tr("Show Mandarin:"));
    _entryPinyin->setText(tr("Pinyin"));
    _entryNumberedPinyin->setText(tr("Pinyin with digits"));

    _colourTitleLabel->setText(
        "<b>" + tr("Tone colouring:") + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_colourCombobox))
        ->setText(tr("Colour characters by tone using:"));
    _colourCombobox->setItemText(0, tr("No Colours"));
    _colourCombobox->setItemText(1, tr("Cantonese"));
    _colourCombobox->setItemText(2, tr("Mandarin"));
    static_cast<QLabel *>(_tabLayout->labelForField(_jyutpingColourWidget))
        ->setText(tr("Cantonese tone colours:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_pinyinColourWidget))
        ->setText(tr("Mandarin tone colours:"));

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
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("DictionaryTab");
    setStyleSheet("QPushButton[isHan=\"true\"] { "
                  "   font-size: 12px; height: 20px; "
                  "} "
                  ""
                  "QWidget#DictionaryTab { "
                  "   background-color: palette(base); "
                  "} ");
#endif

    QString colour = use_dark ? "#424242" : "#d5d5d5";
    QString style = "QFrame { border: 1px solid %1; }";
    QList<QFrame *> frames = this->findChildren<QFrame *>("divider");
    foreach (const auto & frame, frames) {
        frame->setStyleSheet(style.arg(colour));
    }
}

void SettingsTab::initializeCharacterComboBox(QComboBox &characterCombobox)
{
    characterCombobox.addItem("0",
                              QVariant::fromValue<EntryCharactersOptions>(
                                  EntryCharactersOptions::ONLY_SIMPLIFIED));
    characterCombobox.addItem("1",
                              QVariant::fromValue<EntryCharactersOptions>(
                                  EntryCharactersOptions::ONLY_TRADITIONAL));
    characterCombobox.addItem("2",
                              QVariant::fromValue<EntryCharactersOptions>(
                                  EntryCharactersOptions::PREFER_SIMPLIFIED));
    characterCombobox.addItem("3",
                              QVariant::fromValue<EntryCharactersOptions>(
                                  EntryCharactersOptions::PREFER_TRADITIONAL));

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

void SettingsTab::initializePreviewPhonetic(QWidget &previewPhoneticWidget)
{
    previewPhoneticWidget.layout()->addWidget(_previewPhoneticCombobox);

    _previewPhoneticCombobox->addItem("0",
                                      QVariant::fromValue<EntryPhoneticOptions>(
                                          EntryPhoneticOptions::ONLY_CANTONESE));
    _previewPhoneticCombobox->addItem("1",
                                      QVariant::fromValue<EntryPhoneticOptions>(
                                          EntryPhoneticOptions::ONLY_MANDARIN));
    _previewPhoneticCombobox
        ->addItem("2",
                  QVariant::fromValue<EntryPhoneticOptions>(
                      EntryPhoneticOptions::PREFER_CANTONESE));
    _previewPhoneticCombobox->addItem("3",
                                      QVariant::fromValue<EntryPhoneticOptions>(
                                          EntryPhoneticOptions::PREFER_MANDARIN));

    setPhoneticComboBoxDefault(*_previewPhoneticCombobox);

    connect(_previewPhoneticCombobox,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                _settings->setValue("Preview/phoneticOptions",
                                    _previewPhoneticCombobox->itemData(index));
                _settings->sync();
            });
}

void SettingsTab::initializeSearchResultsCantonesePronunciation(
    QWidget &cantonesePronunciationWidget)
{
    cantonesePronunciationWidget.layout()->addWidget(_previewJyutping);
    cantonesePronunciationWidget.layout()->addWidget(_previewYale);

    connect(_previewJyutping, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Preview/cantonesePronunciationOptions",
                            QVariant::fromValue<CantoneseOptions>(
                                CantoneseOptions::RAW_JYUTPING));
        _settings->sync();
    });

    connect(_previewYale, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Preview/cantonesePronunciationOptions",
                            QVariant::fromValue<CantoneseOptions>(
                                CantoneseOptions::PRETTY_YALE));
        _settings->sync();
    });

    setSearchResultsCantonesePronunciationDefault(cantonesePronunciationWidget);
}

void SettingsTab::initializeSearchResultsMandarinPronunciation(
    QWidget &mandarinPronunciationWidget)
{
    mandarinPronunciationWidget.layout()->addWidget(_previewPinyin);
    mandarinPronunciationWidget.layout()->addWidget(_previewNumberedPinyin);

    connect(_previewPinyin, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Preview/mandarinPronunciationOptions",
                            QVariant::fromValue<MandarinOptions>(
                                MandarinOptions::PRETTY_PINYIN));
        _settings->sync();
    });

    connect(_previewNumberedPinyin, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Preview/mandarinPronunciationOptions",
                            QVariant::fromValue<MandarinOptions>(
                                MandarinOptions::NUMBERED_PINYIN));
        _settings->sync();
    });

    setSearchResultsMandarinPronunciationDefault(mandarinPronunciationWidget);
}

void SettingsTab::initializeEntryCantonesePronunciation(
    QWidget &cantonesePronunciationWidget)
{
    cantonesePronunciationWidget.layout()->addWidget(_entryJyutping);
    cantonesePronunciationWidget.layout()->addWidget(_entryYale);

    connect(_entryJyutping, &QCheckBox::stateChanged, this, [&]() {
        CantoneseOptions options
            = _settings
                  ->value("Entry/cantonesePronunciationOptions",
                          QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                  .value<CantoneseOptions>();

        if (_entryJyutping->isChecked()) {
            _settings->setValue("Entry/cantonesePronunciationOptions",
                                QVariant::fromValue<CantoneseOptions>(
                                    options | CantoneseOptions::RAW_JYUTPING));
        } else {
            _settings->setValue("Entry/cantonesePronunciationOptions",
                                QVariant::fromValue<CantoneseOptions>(
                                    options
                                    & ~(CantoneseOptions::RAW_JYUTPING)));
        }
        _settings->sync();
    });

    connect(_entryYale, &QCheckBox::stateChanged, this, [&]() {
        CantoneseOptions options
            = _settings
                  ->value("Entry/cantonesePronunciationOptions",
                          QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                  .value<CantoneseOptions>();

        if (_entryYale->isChecked()) {
            _settings->setValue("Entry/cantonesePronunciationOptions",
                                QVariant::fromValue<CantoneseOptions>(
                                    options | CantoneseOptions::PRETTY_YALE));
        } else {
            _settings->setValue("Entry/cantonesePronunciationOptions",
                                QVariant::fromValue<CantoneseOptions>(
                                    options
                                    & ~(CantoneseOptions::PRETTY_YALE)));
        }
        _settings->sync();
    });

    setEntryCantonesePronunciationDefault(cantonesePronunciationWidget);
}

void SettingsTab::initializeEntryMandarinPronunciation(
    QWidget &mandarinPronunciationWidget)
{
    mandarinPronunciationWidget.layout()->addWidget(_entryPinyin);
    mandarinPronunciationWidget.layout()->addWidget(_entryNumberedPinyin);

    connect(_entryPinyin, &QCheckBox::stateChanged, this, [&]() {
        MandarinOptions options
            = _settings
                  ->value("Entry/mandarinPronunciationOptions",
                          QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();

        if (_entryPinyin->isChecked()) {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options | MandarinOptions::PRETTY_PINYIN));
        } else {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options
                                    & ~(MandarinOptions::PRETTY_PINYIN)));
        }
        _settings->sync();
    });

    connect(_entryNumberedPinyin, &QCheckBox::stateChanged, this, [&]() {
        MandarinOptions options
            = _settings
                  ->value("Entry/mandarinPronunciationOptions",
                          QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();

        if (_entryNumberedPinyin->isChecked()) {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options | MandarinOptions::NUMBERED_PINYIN));
        } else {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options
                                    & ~(MandarinOptions::NUMBERED_PINYIN)));
        }
        _settings->sync();
    });

    setEntryMandarinPronunciationDefault(mandarinPronunciationWidget);
}

void SettingsTab::initializeColourComboBox(QComboBox &colourCombobox)
{
    colourCombobox.addItem("0",
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::NONE));
    colourCombobox.addItem("1",
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::CANTONESE));
    colourCombobox.addItem("2",
                           QVariant::fromValue<EntryColourPhoneticType>(
                               EntryColourPhoneticType::MANDARIN));

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
            QPushButton *sender = static_cast<QPushButton *>(QObject::sender());

            // Get new colour from dialog
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
    connect(&resetButton, &QPushButton::clicked, this, [&]() {
        ResetSettingsDialog *_message = new ResetSettingsDialog{this};
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

    setPhoneticComboBoxDefault(*_previewPhoneticCombobox);
    setSearchResultsCantonesePronunciationDefault(
        *_previewCantonesePronunciation);
    setSearchResultsMandarinPronunciationDefault(
        *_previewMandarinPronunciation);

    setEntryCantonesePronunciationDefault(*_entryCantonesePronunciation);
    setEntryMandarinPronunciationDefault(*_entryMandarinPronunciation);

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
        _settings->value("Preview/phoneticOptions",
                         QVariant::fromValue(
                             EntryPhoneticOptions::PREFER_CANTONESE))));
}

void SettingsTab::setSearchResultsCantonesePronunciationDefault(
    QWidget &widget)
{
    CantoneseOptions lastSelected
        = Settings::getSettings()
              ->value("Preview/cantonesePronunciationOptions",
                      QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
              .value<CantoneseOptions>();

    QList<QRadioButton *> buttons = widget.findChildren<QRadioButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("data").value<CantoneseOptions>() == lastSelected) {
            button->click();
#ifdef Q_OS_MAC
            // Makes the button selection show up correctly on macOS
            button->setDown(true);
#endif
            break;
        }
    }
}

void SettingsTab::setSearchResultsMandarinPronunciationDefault(
    QWidget &widget)
{
    MandarinOptions lastSelected
        = Settings::getSettings()
              ->value("Preview/mandarinPronunciationOptions",
                      QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
              .value<MandarinOptions>();

    QList<QRadioButton *> buttons = widget.findChildren<QRadioButton *>();
    foreach (const auto &button, buttons) {
        if (button->property("data").value<MandarinOptions>() == lastSelected) {
            button->click();
#ifdef Q_OS_MAC
            // Makes the button selection show up correctly on macOS
            button->setDown(true);
#endif
            break;
        }
    }
}

void SettingsTab::setEntryCantonesePronunciationDefault(QWidget &widget)
{
    CantoneseOptions lastSelected
        = Settings::getSettings()
              ->value("Entry/cantonesePronunciationOptions",
                      QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
              .value<CantoneseOptions>();

    QList<QCheckBox *> boxes = widget.findChildren<QCheckBox *>();
    foreach (const auto &box, boxes) {
        CantoneseOptions boxOption = box->property("data")
                                         .value<CantoneseOptions>();
        if ((lastSelected & boxOption) == boxOption) {
            box->setChecked(true);
        } else {
            box->setChecked(false);
        }
    }
}

void SettingsTab::setEntryMandarinPronunciationDefault(QWidget &widget)
{
    MandarinOptions lastSelected
        = Settings::getSettings()
              ->value("Entry/mandarinPronunciationOptions",
                      QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
              .value<MandarinOptions>();

    QList<QCheckBox *> boxes = widget.findChildren<QCheckBox *>();
    foreach (const auto &box, boxes) {
        MandarinOptions boxOption = box->property("data")
                                        .value<MandarinOptions>();
        if ((lastSelected & boxOption) == boxOption) {
            box->setChecked(true);
        } else {
            box->setChecked(false);
        }
    }
}

void SettingsTab::setColourComboBoxDefault(QComboBox &colourCombobox)
{
    colourCombobox.setCurrentIndex(colourCombobox.findData(
        _settings->value("entryColourPhoneticType",
                         QVariant::fromValue(
                             EntryColourPhoneticType::CANTONESE))));
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
