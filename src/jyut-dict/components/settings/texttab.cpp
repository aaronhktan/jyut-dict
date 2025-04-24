#include "texttab.h"

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined(Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QColorDialog>
#include <QFrame>
#include <QStyle>
#include <QTimer>

TextTab::TextTab(QWidget *parent)
    : QWidget{parent}
{
    _settings = Settings::getSettings(this);
    setupUI();
    translateUI();
}

void TextTab::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() {
            _paletteRecentlyChanged = false;
        });
        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void TextTab::setupUI()
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
    _characterCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initializeCharacterComboBox(*_characterCombobox);

    QFrame *_interfaceSizeDivider = new QFrame{this};
    _interfaceSizeDivider->setObjectName("divider");
    _interfaceSizeDivider->setFrameShape(QFrame::HLine);
    _interfaceSizeDivider->setFrameShadow(QFrame::Raised);
    _interfaceSizeDivider->setFixedHeight(1);

    _interfaceSizeTitleLabel = new QLabel{this};
    _interfaceSizeWidget = new QWidget{this};
    _interfaceSizeLayout = new QGridLayout{_interfaceSizeWidget};
    _interfaceSizeLayout->setContentsMargins(0, 0, 0, 0);
    _interfaceSizeSlider = new QSlider{this};
    _interfaceSizeSmallLabel = new QLabel{this};
    _interfaceSizeLargeLabel = new QLabel{this};
    initializeInterfaceSizeWidget(*_interfaceSizeWidget);

    QFrame *_divider = new QFrame{this};
    _divider->setObjectName("divider");
    _divider->setFrameShape(QFrame::HLine);
    _divider->setFrameShadow(QFrame::Raised);
    _divider->setFixedHeight(1);

    _colourTitleLabel = new QLabel{this};
    _colourCombobox = new QComboBox{this};
    _colourCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    initializeColourComboBox(*_colourCombobox);
    _jyutpingColourWidget = new QWidget{this};
    initializeJyutpingColourWidget(*_jyutpingColourWidget);
    _pinyinColourWidget = new QWidget{this};
    initializePinyinColourWidget(*_pinyinColourWidget);

    _tabLayout->addRow(_characterTitleLabel);
    _tabLayout->addRow(" ", _characterCombobox);

    _tabLayout->addRow(_interfaceSizeDivider);

    _tabLayout->addRow(_interfaceSizeTitleLabel);
    _tabLayout->addRow(" ", _interfaceSizeWidget);

    _tabLayout->addRow(_divider);

    _tabLayout->addRow(_colourTitleLabel);
    _tabLayout->addRow(" ", _colourCombobox);
    _tabLayout->addRow(" ", _jyutpingColourWidget);
    _tabLayout->addRow(" ", _pinyinColourWidget);

    setStyle(Utils::isDarkMode());
}

void TextTab::translateUI()
{
    // Set property so styling automatically changes
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _characterTitleLabel->setText("<b>" + tr("Character set:") + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_characterCombobox))
#ifdef Q_OS_WIN
        ->setText(tr("Show simplified/traditional characters:"));
#else
        ->setText(tr("\tShow simplified/traditional characters:"));
#endif
    _characterCombobox->setItemText(0, tr("Only Simplified"));
    _characterCombobox->setItemText(1, tr("Only Traditional"));
    _characterCombobox->setItemText(2, tr("Both, Prefer Simplified"));
    _characterCombobox->setItemText(3, tr("Both, Prefer Traditional"));

    _interfaceSizeTitleLabel->setText("<b>" + tr("Interface:") + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_interfaceSizeWidget))
        ->setText(tr("Interface size:"));
    _interfaceSizeSmallLabel->setText(tr("Smallest"));
    _interfaceSizeLargeLabel->setText(tr("Largest"));

    _colourTitleLabel->setText("<b>" + tr("Tone colouring:") + "</b>");
    static_cast<QLabel *>(_tabLayout->labelForField(_colourCombobox))
        ->setText(tr("Colour characters by tone using:"));
    _colourCombobox->setItemText(0, tr("No Colours"));
    _colourCombobox->setItemText(1, tr("Cantonese"));
    _colourCombobox->setItemText(2, tr("Mandarin"));
    static_cast<QLabel *>(_tabLayout->labelForField(_jyutpingColourWidget))
        ->setText(tr("Cantonese tone colours:"));
    static_cast<QLabel *>(_tabLayout->labelForField(_pinyinColourWidget))
#ifdef Q_OS_WIN
        ->setText(tr("Mandarin tone colours:"));
#else
        ->setText(tr("\tMandarin tone colours:"));
#endif

    QList<QLabel *> jyutpingLabels
        = _jyutpingColourWidget->findChildren<QLabel *>();
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
}

void TextTab::setStyle(bool use_dark)
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
    foreach (const auto &frame, frames) {
        frame->setStyleSheet(style.arg(colour));
    }
}

void TextTab::initializeCharacterComboBox(QComboBox &characterCombobox)
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

                emit updateStyle();
            });
}

void TextTab::initializeInterfaceSizeWidget(QWidget &widget)
{
    _interfaceSizeSlider->setRange(1, 5);
    _interfaceSizeSlider->setOrientation(Qt::Horizontal);
    _interfaceSizeSlider->setTracking(false);
    _interfaceSizeSlider->setTickInterval(1);
    _interfaceSizeSlider->setTickPosition(QSlider::TicksBelow);
    _interfaceSizeSlider->setMinimumWidth(300);

    _interfaceSizeLayout->addWidget(_interfaceSizeSlider, 0, 0, 1, 5);
    _interfaceSizeLayout->addWidget(_interfaceSizeSmallLabel,
                                    1,
                                    0,
                                    1,
                                    1,
                                    Qt::AlignTop | Qt::AlignLeft);
    _interfaceSizeLayout->addWidget(_interfaceSizeLargeLabel,
                                    1,
                                    4,
                                    1,
                                    1,
                                    Qt::AlignTop | Qt::AlignRight);

    connect(_interfaceSizeSlider, &QSlider::valueChanged, this, [&](int value) {
        _settings->setValue("Interface/size",
                            QVariant::fromValue<Settings::InterfaceSize>(
                                static_cast<Settings::InterfaceSize>(value)));
        _settings->sync();

        emit updateStyle();
    });

    setInterfaceSizeWidgetDefault(widget);
}

void TextTab::initializeColourComboBox(QComboBox &colourCombobox)
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

                emit updateStyle();
            });
}

void TextTab::initializeJyutpingColourWidget(QWidget &jyutpingColourWidget)
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
            Settings::jyutpingToneColours[index]
                = newColour.name().toStdString();
            sender->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
                Settings::jyutpingToneColours[index].c_str()));

            saveJyutpingColours();

            emit updateStyle();
        });

        // Then add tone label underneath it
        QLabel *label = new QLabel{&jyutpingColourWidget};
        label->setAlignment(Qt::AlignHCenter);
        label->setMinimumWidth(40);
        jyutpingLayout->addWidget(label, 1, static_cast<int>(i));
    }
}

void TextTab::initializePinyinColourWidget(QWidget &pinyinColourWidget)
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

            emit updateStyle();
        });

        QLabel *label = new QLabel{&pinyinColourWidget};
        label->setAlignment(Qt::AlignHCenter);
        label->setMinimumWidth(40);
        pinyinLayout->addWidget(label, 1, static_cast<int>(i));
    }
}

void TextTab::setCharacterComboBoxDefault(QComboBox &characterCombobox)
{
    characterCombobox.setCurrentIndex(characterCombobox.findData(
        _settings->value("characterOptions",
                         QVariant::fromValue(
                             EntryCharactersOptions::PREFER_TRADITIONAL))));
}

void TextTab::setInterfaceSizeWidgetDefault(QWidget &widget)
{
    widget.findChild<QSlider *>()->setSliderPosition(static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>()));
}

void TextTab::setColourComboBoxDefault(QComboBox &colourCombobox)
{
    colourCombobox.setCurrentIndex(colourCombobox.findData(
        _settings->value("entryColourPhoneticType",
                         QVariant::fromValue(
                             EntryColourPhoneticType::CANTONESE))));
}

void TextTab::setJyutpingColourWidgetDefault(QWidget &jyutpingColourWidget)
{
    QList<QPushButton *> buttons
        = jyutpingColourWidget.findChildren<QPushButton *>();
    for (QList<QPushButton *>::size_type i = 0; i < buttons.size(); i++) {
        buttons[i]->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
            Settings::jyutpingToneColours[static_cast<unsigned long>(i)]
                .c_str()));
    }
}

void TextTab::setPinyinColourWidgetDefault(QWidget &pinyinColourWidget)
{
    QList<QPushButton *> buttons
        = pinyinColourWidget.findChildren<QPushButton *>();
    for (QList<QPushButton *>::size_type i = 0; i < buttons.size(); i++) {
        buttons[i]->setStyleSheet(QString{COLOUR_BUTTON_STYLE}.arg(
            Settings::pinyinToneColours[static_cast<unsigned long>(i)].c_str()));
    }
}

QColor TextTab::getNewColour(QColor old_colour)
{
    return QColorDialog::getColor(old_colour, this);
}

void TextTab::saveJyutpingColours()
{
    _settings->beginWriteArray("jyutpingColours");
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::jyutpingToneColours.size();
         i++) {
        _settings->setArrayIndex(static_cast<int>(i));
        _settings->setValue("colour",
                            QColor{Settings::jyutpingToneColours[i].c_str()});
    }
    _settings->endArray();
    _settings->sync();
}

void TextTab::savePinyinColours()
{
    _settings->beginWriteArray("pinyinColours");
    for (std::vector<std::string>::size_type i = 0;
         i < Settings::pinyinToneColours.size();
         i++) {
        _settings->setArrayIndex(static_cast<int>(i));
        _settings->setValue("colour",
                            QColor{Settings::pinyinToneColours[i].c_str()});
    }
    _settings->endArray();
    _settings->sync();
}

void TextTab::resetSettings(void)
{
    Settings::jyutpingToneColours = Settings::defaultJyutpingToneColours;
    Settings::pinyinToneColours = Settings::defaultPinyinToneColours;

    setCharacterComboBoxDefault(*_characterCombobox);

    setInterfaceSizeWidgetDefault(*_interfaceSizeWidget);

    setColourComboBoxDefault(*_colourCombobox);
    setJyutpingColourWidgetDefault(*_jyutpingColourWidget);
    setPinyinColourWidgetDefault(*_pinyinColourWidget);
}
