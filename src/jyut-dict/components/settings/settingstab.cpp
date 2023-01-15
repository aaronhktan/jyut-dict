#include "settingstab.h"

#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QApplication>
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
    _previewCantoneseIPA = new QRadioButton{this};
    _previewCantoneseIPA->setProperty("data",
                                      QVariant::fromValue(
                                          CantoneseOptions::CANTONESE_IPA));
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
    _previewZhuyin = new QRadioButton{this};
    _previewZhuyin->setProperty("data",
                                QVariant::fromValue(MandarinOptions::ZHUYIN));
    _previewMandarinIPA = new QRadioButton{this};
    _previewMandarinIPA->setProperty("data",
                                     QVariant::fromValue(
                                         MandarinOptions::MANDARIN_IPA));
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
    _entryCantoneseIPA = new QCheckBox{this};
    _entryCantoneseIPA->setTristate(false);
    _entryCantoneseIPA->setProperty("data",
                                    QVariant::fromValue(
                                        CantoneseOptions::CANTONESE_IPA));
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
    _entryZhuyin = new QCheckBox{this};
    _entryZhuyin->setTristate(false);
    _entryZhuyin->setProperty("data",
                              QVariant::fromValue(MandarinOptions::ZHUYIN));
    _entryMandarinIPA = new QCheckBox{this};
    _entryMandarinIPA->setTristate(false);
    _entryMandarinIPA->setProperty("data",
                                   QVariant::fromValue(
                                       MandarinOptions::MANDARIN_IPA));
    initializeEntryMandarinPronunciation(*_entryMandarinPronunciation);

    _tabLayout->addRow(_previewTitleLabel);
    _tabLayout->addRow(" ", _previewPhoneticWidget);
    _tabLayout->addRow(" ", _previewCantonesePronunciation);
    _tabLayout->addRow(" ", _previewMandarinPronunciation);

    _tabLayout->addRow(_entryDivider);

    _tabLayout->addRow(_entryTitleLabel);
    _tabLayout->addRow(" ", _entryCantonesePronunciation);
    _tabLayout->addRow(" ", _entryMandarinPronunciation);

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

    _previewTitleLabel->setText("<b>" + tr("In search results and examples:")
                                + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_previewPhoneticWidget))
        ->setText(tr("\tShow pronunciation for:"));
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
    _previewCantoneseIPA->setText(tr("Cantonese IPA"));
    static_cast<QLabel *>(
        _tabLayout->labelForField(_previewMandarinPronunciation))
        ->setText(tr("Show Mandarin:"));
    _previewPinyin->setText(tr("Pinyin"));
    _previewNumberedPinyin->setText(tr("Pinyin with digits"));
    _previewZhuyin->setText(tr("Zhuyin"));
    _previewMandarinIPA->setText(tr("Mandarin IPA"));

    _entryTitleLabel->setText(
        "<b>" + tr("In the header at the top of an entry:") + "</b>");
    static_cast<QLabel *>(
        _tabLayout->labelForField(_entryCantonesePronunciation))
        ->setText(tr("Show Cantonese:"));
    _entryJyutping->setText(tr("Jyutping"));
    _entryYale->setText(tr("Yale"));
    _entryCantoneseIPA->setText(tr("Cantonese IPA"));
    static_cast<QLabel *>(_tabLayout->labelForField(_entryMandarinPronunciation))
        ->setText(tr("Show Mandarin:"));
    _entryPinyin->setText(tr("Pinyin"));
    _entryNumberedPinyin->setText(tr("Pinyin with digits"));
    _entryZhuyin->setText(tr("Zhuyin"));
    _entryMandarinIPA->setText(tr("Mandarin IPA"));
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

    setPhoneticComboboxDefault(*_previewPhoneticCombobox);

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
    cantonesePronunciationWidget.layout()->addWidget(_previewCantoneseIPA);

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

    connect(_previewCantoneseIPA, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Preview/cantonesePronunciationOptions",
                            QVariant::fromValue<CantoneseOptions>(
                                CantoneseOptions::CANTONESE_IPA));
        _settings->sync();
    });

    setSearchResultsCantonesePronunciationDefault(cantonesePronunciationWidget);
}

void SettingsTab::initializeSearchResultsMandarinPronunciation(
    QWidget &mandarinPronunciationWidget)
{
    mandarinPronunciationWidget.layout()->addWidget(_previewPinyin);
    mandarinPronunciationWidget.layout()->addWidget(_previewNumberedPinyin);
    mandarinPronunciationWidget.layout()->addWidget(_previewZhuyin);
    mandarinPronunciationWidget.layout()->addWidget(_previewMandarinIPA);

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

    connect(_previewZhuyin, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Preview/mandarinPronunciationOptions",
                            QVariant::fromValue<MandarinOptions>(
                                MandarinOptions::ZHUYIN));
        _settings->sync();
    });

    connect(_previewMandarinIPA, &QRadioButton::clicked, this, [&]() {
        _settings->setValue("Preview/mandarinPronunciationOptions",
                            QVariant::fromValue<MandarinOptions>(
                                MandarinOptions::MANDARIN_IPA));
        _settings->sync();
    });

    setSearchResultsMandarinPronunciationDefault(mandarinPronunciationWidget);
}

void SettingsTab::initializeEntryCantonesePronunciation(
    QWidget &cantonesePronunciationWidget)
{
    cantonesePronunciationWidget.layout()->addWidget(_entryJyutping);
    cantonesePronunciationWidget.layout()->addWidget(_entryYale);
    cantonesePronunciationWidget.layout()->addWidget(_entryCantoneseIPA);

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

    connect(_entryCantoneseIPA, &QCheckBox::stateChanged, this, [&]() {
        CantoneseOptions options
            = _settings
                  ->value("Entry/cantonesePronunciationOptions",
                          QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                  .value<CantoneseOptions>();

        if (_entryCantoneseIPA->isChecked()) {
            _settings->setValue("Entry/cantonesePronunciationOptions",
                                QVariant::fromValue<CantoneseOptions>(
                                    options | CantoneseOptions::CANTONESE_IPA));
        } else {
            _settings->setValue("Entry/cantonesePronunciationOptions",
                                QVariant::fromValue<CantoneseOptions>(
                                    options
                                    & ~(CantoneseOptions::CANTONESE_IPA)));
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
    mandarinPronunciationWidget.layout()->addWidget(_entryZhuyin);
    mandarinPronunciationWidget.layout()->addWidget(_entryMandarinIPA);

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

    connect(_entryZhuyin, &QCheckBox::stateChanged, this, [&]() {
        MandarinOptions options
            = _settings
                  ->value("Entry/mandarinPronunciationOptions",
                          QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();

        if (_entryZhuyin->isChecked()) {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options | MandarinOptions::ZHUYIN));
        } else {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options
                                    & ~(MandarinOptions::ZHUYIN)));
        }
        _settings->sync();
    });

    connect(_entryMandarinIPA, &QCheckBox::stateChanged, this, [&]() {
        MandarinOptions options
            = _settings
                  ->value("Entry/mandarinPronunciationOptions",
                          QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();

        if (_entryMandarinIPA->isChecked()) {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options | MandarinOptions::MANDARIN_IPA));
        } else {
            _settings->setValue("Entry/mandarinPronunciationOptions",
                                QVariant::fromValue<MandarinOptions>(
                                    options & ~(MandarinOptions::MANDARIN_IPA)));
        }
        _settings->sync();
    });

    setEntryMandarinPronunciationDefault(mandarinPronunciationWidget);
}
void SettingsTab::setPhoneticComboboxDefault(QComboBox &phoneticCombobox)
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

void SettingsTab::resetSettings(void)
{
    setPhoneticComboboxDefault(*_previewPhoneticCombobox);
    setSearchResultsCantonesePronunciationDefault(
        *_previewCantonesePronunciation);
    setSearchResultsMandarinPronunciationDefault(*_previewMandarinPronunciation);

    setEntryCantonesePronunciationDefault(*_entryCantonesePronunciation);
    setEntryMandarinPronunciationDefault(*_entryMandarinPronunciation);
}
