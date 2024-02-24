#include "entryheaderwidget.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#include "logic/utils/chineseutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QtGlobal>
#include <QCoreApplication>
#include <QIcon>
#include <QTimer>
#include <QVariant>

EntryHeaderWidget::EntryHeaderWidget(QWidget *parent) : QWidget(parent)
{
    _settings = Settings::getSettings(this);

    _entryHeaderLayout = new QGridLayout{this};
    _entryHeaderLayout->setContentsMargins(0, 0, 0, 0);
    _entryHeaderLayout->setSpacing(5);

    _speaker = std::make_unique<EntrySpeaker>();

    _wordLabel = new QLabel{this};
    _wordLabel->setAttribute(Qt::WA_TranslucentBackground);
    _wordLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _wordLabel->setWordWrap(true);

    _cantoneseTTS = new QPushButton{this};
    _cantoneseTTS->setAttribute(Qt::WA_TranslucentBackground);
    _cantoneseTTS->setVisible(false);

    _mandarinTTS = new QPushButton{this};
    _mandarinTTS->setAttribute(Qt::WA_TranslucentBackground);
    _mandarinTTS->setVisible(false);

    _entryHeaderLayout->addWidget(_wordLabel, 1, 0, 1, -1);

    setStyle(Utils::isDarkMode());
    translateUI();
}

void EntryHeaderWidget::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::FontChange) {
        // This is just to correctly resize the JP/PY etc. labels
        translateUI();
    }
#endif
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

void EntryHeaderWidget::setEntry(const Entry &entry)
{
    clearPronunciationLabels();

    _wordLabel->setText(
        entry
            .getCharacters(
                Settings::getSettings()
                    ->value("characterOptions",
                            QVariant::fromValue(
                                EntryCharactersOptions::PREFER_TRADITIONAL))
                    .value<EntryCharactersOptions>(),
                true)
            .c_str());

    CantoneseOptions cantoneseOptions
        = Settings::getSettings()
              ->value("Entry/cantonesePronunciationOptions",
                      QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
              .value<CantoneseOptions>();
    MandarinOptions mandarinOptions
        = Settings::getSettings()
              ->value("Entry/mandarinPronunciationOptions",
                      QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
              .value<MandarinOptions>();
    displayPronunciationLabels(entry,
                               cantoneseOptions,
                               mandarinOptions);

    _chinese = QString{entry.getSimplified().c_str()};
    _jyutping = QString{entry.getJyutping().c_str()};
    // Pinyin by default follows CEDICT's convention of denoting "ü" with "u:",
    // but most TTS systems denote that vowel with "v". To get the TTS systems
    // to properly pronounce that word, we must convert "u:" to "v".
    _pinyin = QString{
        ChineseUtils::createPinyinWithV(entry.getPinyin()).c_str()};

    translateUI();
    setStyle(Utils::isDarkMode());
}

void EntryHeaderWidget::translateUI()
{
    for (const auto &label : _pronunciationTypeLabels) {
        if (label->objectName() == "jyutpingTypeLabel") {
            label->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                       Strings::JYUTPING_SHORT));
        } else if (label->objectName() == "yaleTypeLabel") {
            label->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                       Strings::YALE_SHORT));
        } else if (label->objectName() == "cantoneseIPATypeLabel") {
            label->setText(
                QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                            Strings::CANTONESE_IPA_SHORT));
        } else if (label->objectName() == "numberedPinyinTypeLabel"
                   || label->objectName() == "prettyPinyinTypeLabel") {
            label->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                       Strings::PINYIN_SHORT));
        } else if (label->objectName() == "zhuyinTypeLabel") {
            label->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                       Strings::ZHUYIN_SHORT));
        } else if (label->objectName() == "mandarinIPATypeLabel") {
            label->setText(
                QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                            Strings::MANDARIN_IPA_SHORT));
        }

        label->setFixedWidth(
            label->fontMetrics().boundingRect(label->text()).width());
        label->setVisible(true);
    }

    disconnect(_cantoneseTTS, nullptr, nullptr, nullptr);
    connect(_cantoneseTTS, &QPushButton::clicked, this, [=]() {
        TextToSpeech::SpeakerBackend backend
            = Settings::getSettings()
                  ->value("Advanced/CantoneseTextToSpeech::SpeakerBackend",
                          QVariant::fromValue(
                              TextToSpeech::SpeakerBackend::QT_TTS))
                  .value<TextToSpeech::SpeakerBackend>();
        TextToSpeech::SpeakerVoice voice
            = Settings::getSettings()
                  ->value("Advanced/CantoneseTextToSpeech::SpeakerVoice",
                          QVariant::fromValue(TextToSpeech::SpeakerVoice::NONE))
                  .value<TextToSpeech::SpeakerVoice>();
#ifdef Q_OS_MAC
        if (!_speaker->speakCantonese(_jyutping, backend, voice)) {
            return;
        }
#endif
        if ((backend != TextToSpeech::SpeakerBackend::QT_TTS)) {
            if (!_speaker->speakCantonese(_jyutping, backend, voice)) {
                return;
            }
        }
        if (!_speaker->speakCantonese(_chinese, backend, voice)) {
            return;
        }
        showError(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                              Strings::YUE_ERROR_STRING),
                  QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                              Strings::YUE_DESCRIPTION_STRING)
                      .arg(Settings::getCurrentLocale().bcp47Name()));
    });

    disconnect(_mandarinTTS, nullptr, nullptr, nullptr);
    if (Settings::getCurrentLocale().country() == QLocale::Taiwan) {
        TextToSpeech::SpeakerBackend backend
            = Settings::getSettings()
                  ->value("Advanced/MandarinTextToSpeech::SpeakerBackend",
                          QVariant::fromValue(
                              TextToSpeech::SpeakerBackend::QT_TTS))
                  .value<TextToSpeech::SpeakerBackend>();
        TextToSpeech::SpeakerVoice voice
            = Settings::getSettings()
                  ->value("Advanced/MandarinTextToSpeech::SpeakerVoice",
                          QVariant::fromValue(TextToSpeech::SpeakerVoice::NONE))
                  .value<TextToSpeech::SpeakerVoice>();
        connect(_mandarinTTS, &QPushButton::clicked, this, [=]() {
#ifdef Q_OS_MAC
            if (!_speaker->speakTaiwaneseMandarin(_pinyin, backend, voice)) {
                return;
            }
#endif
            if (backend != TextToSpeech::SpeakerBackend::QT_TTS) {
                if (!_speaker->speakTaiwaneseMandarin(_pinyin, backend, voice)) {
                    return;
                }
            }
            if (!_speaker->speakTaiwaneseMandarin(_chinese, backend, voice)) {
                return;
            }
            showError(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                  Strings::ZH_TW_ERROR_STRING),
                      QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                  Strings::ZH_TW_DESCRIPTION_STRING)
                          .arg(Settings::getCurrentLocale().bcp47Name()));
        });
    } else {
        connect(_mandarinTTS, &QPushButton::clicked, this, [=]() {
            TextToSpeech::SpeakerBackend backend
                = Settings::getSettings()
                      ->value("Advanced/MandarinTextToSpeech::SpeakerBackend",
                              QVariant::fromValue(
                                  TextToSpeech::SpeakerBackend::QT_TTS))
                      .value<TextToSpeech::SpeakerBackend>();
            TextToSpeech::SpeakerVoice voice
                = Settings::getSettings()
                      ->value("Advanced/MandarinTextToSpeech::SpeakerVoice",
                              QVariant::fromValue(
                                  TextToSpeech::SpeakerVoice::NONE))
                      .value<TextToSpeech::SpeakerVoice>();
#ifdef Q_OS_MAC
            if (!_speaker->speakMainlandMandarin(_pinyin, backend, voice)) {
                return;
            }
#endif
            if (backend != TextToSpeech::SpeakerBackend::QT_TTS) {
                if (!_speaker->speakMainlandMandarin(_pinyin, backend, voice)) {
                    return;
                }
            }
            if (!_speaker->speakMainlandMandarin(_chinese)) {
                return;
            }
            showError(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                  Strings::ZH_CN_ERROR_STRING),
                      QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                  Strings::ZH_CN_DESCRIPTION_STRING)
                          .arg(Settings::getCurrentLocale().bcp47Name()));
        });
    }
}

void EntryHeaderWidget::setStyle(bool use_dark)
{
#ifdef Q_OS_WIN
    QFont font = QFont{"Microsoft YaHei"};
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    _wordLabel->setFont(font);
#endif

    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int h2FontSize = Settings::h2FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));

    _wordLabel->setStyleSheet(QString{"QLabel { "
                                      "   font-size: %1px; "
                                      "}"}
                                  .arg(h2FontSize));

    QString pronunciationTypeStyleSheet = QString{"QLabel { "
                                                  "   color: %1; "
                                                  "   font-size: %2px; "
                                                  "}"};
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    for (const auto& label : _pronunciationTypeLabels) {
        label->setAttribute(Qt::WA_TranslucentBackground);
        label->setStyleSheet(pronunciationTypeStyleSheet.arg(textColour.name())
                                 .arg(bodyFontSize));
        label->setFixedWidth(
            label->fontMetrics().boundingRect(label->text()).width());
    }

    QString pronunciationStyleSheet = QString{"QLabel { "
                                              "   font-size: %1px; "
                                              "}"};
    for (const auto& label : _pronunciationLabels) {
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setWordWrap(true);
        label->setStyleSheet(pronunciationStyleSheet.arg(bodyFontSize));
    }

    _cantoneseTTS->setIcon(use_dark ? QIcon{":/images/speak_inverted.png"}
                                    : QIcon{":/images/speak.png"});
    _cantoneseTTS->setFlat(true);
    _cantoneseTTS->setObjectName("cantoneseTTS");
    _cantoneseTTS->setStyleSheet(QString{"QPushButton#cantoneseTTS { "
                                         "   background-color: none;"
                                         "   border: 1px solid transparent; "
                                         "   font-size: %1px; "
                                         "   padding: 0px; "
                                         "} "
                                         ""
                                         "QPushButton:pressed#cantoneseTTS { "
                                         "   background-color: none; "
                                         "   border: 1px solid transparent; "
                                         "   font-size: %1px; "
                                         "} "}
                                     .arg(bodyFontSize));
    _cantoneseTTS->setCursor(Qt::PointingHandCursor);
    _cantoneseTTS->setFixedWidth(bodyFontSize);
    _cantoneseTTS->setFixedHeight(
        _cantoneseTTS->fontMetrics().boundingRect("123xyz").height());

    _mandarinTTS->setIcon(use_dark ? QIcon{":/images/speak_inverted.png"}
                                   : QIcon{":/images/speak.png"});
    _mandarinTTS->setFlat(true);
    _mandarinTTS->setObjectName("mandarinTTS");
    _mandarinTTS->setStyleSheet(QString{"QPushButton#mandarinTTS { "
                                        "   background-color: none;"
                                        "   border: 1px solid transparent; "
                                        "   font-size: %1px; "
                                        "   padding: 0px; "
                                        "} "
                                        ""
                                        "QPushButton:pressed#mandarinTTS { "
                                        "   background-color: none; "
                                        "   border: 1px solid transparent; "
                                        "   font-size: %1px; "
                                        "} "}
                                    .arg(bodyFontSize));
    _mandarinTTS->setCursor(Qt::PointingHandCursor);
    _mandarinTTS->setFixedWidth(bodyFontSize);
    _mandarinTTS->setFixedHeight(
        _mandarinTTS->fontMetrics().boundingRect("123xyz").height());
}

void EntryHeaderWidget::displayPronunciationLabels(
    const Entry& entry,
    const CantoneseOptions &cantoneseOptions,
    const MandarinOptions &mandarinOptions)
{
    // The entry headword is the first row, so we start adding labels starting
    // from the second row
    int row = 2;

    if ((cantoneseOptions & CantoneseOptions::RAW_JYUTPING)
        == CantoneseOptions::RAW_JYUTPING) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("jyutpingTypeLabel");
        _entryHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                      row,
                                      0,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            entry.getCantonesePhonetic(CantoneseOptions::RAW_JYUTPING).c_str());
        _entryHeaderLayout->addWidget(_pronunciationLabels.back(),
                                      row,
                                      2,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_cantoneseTTSVisible) {
            _entryHeaderLayout
                ->addWidget(_cantoneseTTS, row, 1, 1, 1, Qt::AlignTop);
            _cantoneseTTS->setVisible(true);
            _cantoneseTTSVisible = true;
        }

        row++;
    }

    if ((cantoneseOptions & CantoneseOptions::PRETTY_YALE)
        == CantoneseOptions::PRETTY_YALE) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("yaleTypeLabel");
        _entryHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                      row,
                                      0,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            entry.getCantonesePhonetic(CantoneseOptions::PRETTY_YALE).c_str());
        _entryHeaderLayout->addWidget(_pronunciationLabels.back(),
                                      row,
                                      2,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_cantoneseTTSVisible) {
            _entryHeaderLayout
                ->addWidget(_cantoneseTTS, row, 1, 1, 1, Qt::AlignTop);
            _cantoneseTTS->setVisible(true);
            _cantoneseTTSVisible = true;
        }

        row++;
    }

    if ((cantoneseOptions & CantoneseOptions::CANTONESE_IPA)
        == CantoneseOptions::CANTONESE_IPA) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("cantoneseIPATypeLabel");
        _entryHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                      row,
                                      0,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            entry.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA).c_str());
        _entryHeaderLayout->addWidget(_pronunciationLabels.back(),
                                      row,
                                      2,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_cantoneseTTSVisible) {
            _entryHeaderLayout
                ->addWidget(_cantoneseTTS, row, 1, 1, 1, Qt::AlignTop);
            _cantoneseTTS->setVisible(true);
            _cantoneseTTSVisible = true;
        }

        row++;
    }

    if ((mandarinOptions & MandarinOptions::PRETTY_PINYIN)
        == MandarinOptions::PRETTY_PINYIN) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("prettyPinyinTypeLabel");
        _entryHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                      row,
                                      0,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            entry.getMandarinPhonetic(MandarinOptions::PRETTY_PINYIN).c_str());
        _entryHeaderLayout->addWidget(_pronunciationLabels.back(),
                                      row,
                                      2,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _entryHeaderLayout
                ->addWidget(_mandarinTTS, row, 1, 1, 1, Qt::AlignTop);
            _mandarinTTS->setVisible(true);
            _mandarinTTSVisible = true;
        }

        row++;
    }

    if ((mandarinOptions & MandarinOptions::NUMBERED_PINYIN)
        == MandarinOptions::NUMBERED_PINYIN) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName(
            "numberedPinyinTypeLabel");
        _entryHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                      row,
                                      0,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            entry.getMandarinPhonetic(MandarinOptions::NUMBERED_PINYIN).c_str());
        _entryHeaderLayout->addWidget(_pronunciationLabels.back(),
                                      row,
                                      2,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _entryHeaderLayout
                ->addWidget(_mandarinTTS, row, 1, 1, 1, Qt::AlignTop);
            _mandarinTTS->setVisible(true);
            _mandarinTTSVisible = true;
        }

        row++;
    }

    if ((mandarinOptions & MandarinOptions::ZHUYIN) == MandarinOptions::ZHUYIN) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("zhuyinTypeLabel");
        _entryHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                      row,
                                      0,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            entry.getMandarinPhonetic(MandarinOptions::ZHUYIN).c_str());
        _entryHeaderLayout->addWidget(_pronunciationLabels.back(),
                                      row,
                                      2,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _entryHeaderLayout
                ->addWidget(_mandarinTTS, row, 1, 1, 1, Qt::AlignTop);
            _mandarinTTS->setVisible(true);
            _mandarinTTSVisible = true;
        }

        row++;
    }

    if ((mandarinOptions & MandarinOptions::MANDARIN_IPA)
        == MandarinOptions::MANDARIN_IPA) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("mandarinIPATypeLabel");
        _entryHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                      row,
                                      0,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            entry.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA).c_str());
        _entryHeaderLayout->addWidget(_pronunciationLabels.back(),
                                      row,
                                      2,
                                      1,
                                      1,
                                      Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _entryHeaderLayout
                ->addWidget(_mandarinTTS, row, 1, 1, 1, Qt::AlignTop);
            _mandarinTTS->setVisible(true);
            _mandarinTTSVisible = true;
        }

        row++;
    }
}

void EntryHeaderWidget::clearPronunciationLabels(void)
{
    for (auto const &label : _pronunciationTypeLabels) {
        _entryHeaderLayout->removeWidget(label);
        delete label;
    }
    _pronunciationTypeLabels.clear();

    for (auto const &label : _pronunciationLabels) {
        _entryHeaderLayout->removeWidget(label);
        delete label;
    }
    _pronunciationLabels.clear();

    _cantoneseTTS->setVisible(false);
    _cantoneseTTSVisible = false;

    _mandarinTTS->setVisible(false);
    _mandarinTTSVisible = false;
}

void EntryHeaderWidget::showError(const QString &reason, const QString &description)
{
    _message = new EntrySpeakErrorDialog{reason, description, this};
    _message->exec();
}
