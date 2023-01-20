#include "sentenceviewheaderwidget.h"

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

SentenceViewHeaderWidget::SentenceViewHeaderWidget(QWidget *parent) : QWidget(parent)
{
    _settings = Settings::getSettings(this);

    _speaker = std::make_unique<EntrySpeaker>();

    setupUI();
    translateUI();
    setStyle(Utils::isDarkMode());
}

void SentenceViewHeaderWidget::changeEvent(QEvent *event)
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

void SentenceViewHeaderWidget::setSourceSentence(const SourceSentence &sentence)
{
    clearPronunciationLabels();

    _sourceLanguageLabel->setProperty("language",
                                      QString{sentence.getSourceLanguage().c_str()});
    _sourceLanguageLabel->setText(
        Utils::getLanguageFromISO639(sentence.getSourceLanguage()).trimmed());

    _simplifiedLabel->setText(
        QString{sentence.getSimplified().c_str()}.trimmed());
    _traditionalLabel->setText(
        QString{sentence.getTraditional().c_str()}.trimmed());

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

    displaySentenceLabels(
        Settings::getSettings()
            ->value("characterOptions",
                    QVariant::fromValue(
                        EntryCharactersOptions::PREFER_TRADITIONAL))
            .value<EntryCharactersOptions>());
    displayPronunciationLabels(sentence,
                               cantoneseOptions,
                               mandarinOptions);

    _chinese = QString{sentence.getSimplified().empty()
                           ? sentence.getSimplified().c_str()
                           : sentence.getTraditional().c_str()};
    _jyutping = QString{sentence.getJyutping().c_str()};
    _pinyin = QString{
        ChineseUtils::createPinyinWithV(sentence.getPinyin()).c_str()};

    translateUI();
    setStyle(Utils::isDarkMode());
}

void SentenceViewHeaderWidget::setupUI(void)
{
    _sentenceHeaderLayout = new QGridLayout{this};
    _sentenceHeaderLayout->setContentsMargins(0, 0, 0, 0);
    _sentenceHeaderLayout->setSpacing(5);

    _sourceLanguageLabel = new QLabel{this};
    _sourceLanguageLabel->setVisible(false);

    _simplifiedLabel = new QLabel{this};
    _simplifiedLabel->setAttribute(Qt::WA_TranslucentBackground);
    _simplifiedLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _simplifiedLabel->setWordWrap(true);

    _traditionalLabel = new QLabel{this};
    _traditionalLabel->setAttribute(Qt::WA_TranslucentBackground);
    _traditionalLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _traditionalLabel->setWordWrap(true);

    _cantoneseTTS = new QPushButton{this};
    _cantoneseTTS->setAttribute(Qt::WA_TranslucentBackground);
    _cantoneseTTS->setVisible(false);

    _mandarinTTS = new QPushButton{this};
    _mandarinTTS->setAttribute(Qt::WA_TranslucentBackground);
    _mandarinTTS->setVisible(false);

    _sentenceHeaderLayout->addWidget(_sourceLanguageLabel, 0, 0, 1, -1, Qt::AlignLeft);
    _sentenceHeaderLayout->addWidget(_simplifiedLabel, 1, 0, 1, -1);
    _sentenceHeaderLayout->addWidget(_traditionalLabel, 2, 0, 1, -1);
}

void SentenceViewHeaderWidget::translateUI(void)
{
    _sourceLanguageLabel->setText(
        Utils::getLanguageFromISO639(
            _sourceLanguageLabel->property("language").toString().toStdString())
            .trimmed());
    _sourceLanguageLabel->resize(_sourceLanguageLabel->sizeHint());

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

        label->setVisible(true);
    }

    disconnect(_cantoneseTTS, nullptr, nullptr, nullptr);
    connect(_cantoneseTTS, &QPushButton::clicked, this, [=]() {
        if (!_speaker->speakCantonese(_chinese)) {
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
        connect(_mandarinTTS, &QPushButton::clicked, this, [=]() {
            if (!_speaker->speakTaiwaneseMandarin(_chinese)) {
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

void SentenceViewHeaderWidget::setStyle(bool use_dark)
{
#ifdef Q_OS_WIN
    QFont font = QFont{"Microsoft YaHei"};
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    _simplifiedLabel->setFont(font);
    _traditionalLabel->setFont(font);
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

    int borderRadius = static_cast<int>(bodyFontSize * 1);
    int padding = bodyFontSize / 6;

    QString sourceStyleSheet = "QLabel {"
                               "   background: %1; "
                               "   border-radius: %2px; "
                               "   color: %3; "
                               "   font-size: %4px; "
                               "   padding: %5px; "
                               "} ";
    QColor languageColour = Utils::getLanguageColour(
        Utils::getISO639FromLanguage(_sourceLanguageLabel->text().trimmed()));
    QColor languageTextColour = Utils::getContrastingColour(languageColour);
    _sourceLanguageLabel->setStyleSheet(
        sourceStyleSheet.arg(languageColour.name())
            .arg(borderRadius)
            .arg(languageTextColour.name())
            .arg(bodyFontSize)
            .arg(padding));
    _sourceLanguageLabel->setMinimumHeight(borderRadius * 2);

    _simplifiedLabel->setStyleSheet(QString{"QLabel { "
                                            "   font-size: %1px "
                                            "}"}
                                        .arg(h2FontSize));
    _traditionalLabel->setStyleSheet(QString{"QLabel { "
                                             "   font-size: %1px "
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
    for (const auto &label : _pronunciationTypeLabels) {
        label->setAttribute(Qt::WA_TranslucentBackground);
        label->setStyleSheet(pronunciationTypeStyleSheet.arg(textColour.name())
                                 .arg(bodyFontSize));
        label->setFixedWidth(
            label->fontMetrics().boundingRect(label->text()).width());
    }

    QString pronunciationStyleSheet = QString{"QLabel { "
                                              "   font-size: %1px; "
                                              "}"};
    for (const auto &label : _pronunciationLabels) {
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

void SentenceViewHeaderWidget::displaySentenceLabels(
    const EntryCharactersOptions options)
{
    _sourceLanguageLabel->setVisible(true);

    // Display the first label
    switch (options) {
    case EntryCharactersOptions::ONLY_SIMPLIFIED:
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
        _sentenceHeaderLayout->addWidget(_simplifiedLabel, 1, 0, 1, -1);
        _simplifiedLabel->setVisible(true);
        break;
    case EntryCharactersOptions::ONLY_TRADITIONAL:
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        _sentenceHeaderLayout->addWidget(_traditionalLabel, 1, 0, 1, -1);
        _traditionalLabel->setVisible(true);
        break;
    }

    // Display the second label
    switch (options) {
    case EntryCharactersOptions::ONLY_SIMPLIFIED:
        _traditionalLabel->setVisible(false);
        break;
    case EntryCharactersOptions::ONLY_TRADITIONAL:
        _simplifiedLabel->setVisible(false);
        break;
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
        _sentenceHeaderLayout->addWidget(_traditionalLabel, 2, 0, 1, -1);
        _traditionalLabel->setVisible(true);
        break;
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        _sentenceHeaderLayout->addWidget(_simplifiedLabel, 2, 0, 1, -1);
        _simplifiedLabel->setVisible(true);
        break;
    }
}

void SentenceViewHeaderWidget::displayPronunciationLabels(
    const SourceSentence &sentence,
    const CantoneseOptions &cantoneseOptions,
    const MandarinOptions &mandarinOptions)
{
    // The sentence language pill is the first row, and the sentence is the
    // second row (and third row, if simplified and traditional display are
    // both enabled), so we start adding labels starting from the fourth row

    // But for some reason, starting from the fourth row makes the Yale (and
    // only Yale) pronunciation labels super thick? So start from fifth row as
    // a workaround.
    int row = 4;

    if ((cantoneseOptions & CantoneseOptions::RAW_JYUTPING)
        == CantoneseOptions::RAW_JYUTPING) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("jyutpingTypeLabel");
        _sentenceHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                         row,
                                         0,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            sentence.getCantonesePhonetic(CantoneseOptions::RAW_JYUTPING)
                .c_str());
        _sentenceHeaderLayout->addWidget(_pronunciationLabels.back(),
                                         row,
                                         2,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_cantoneseTTSVisible) {
            _sentenceHeaderLayout
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
        _sentenceHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                         row,
                                         0,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            sentence.getCantonesePhonetic(CantoneseOptions::PRETTY_YALE).c_str());
        _sentenceHeaderLayout->addWidget(_pronunciationLabels.back(),
                                         row,
                                         2,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_cantoneseTTSVisible) {
            _sentenceHeaderLayout
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
        _sentenceHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                         row,
                                         0,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            sentence.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)
                .c_str());
        _sentenceHeaderLayout->addWidget(_pronunciationLabels.back(),
                                         row,
                                         2,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_cantoneseTTSVisible) {
            _sentenceHeaderLayout
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
        _sentenceHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                         row,
                                         0,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            sentence.getMandarinPhonetic(MandarinOptions::PRETTY_PINYIN).c_str());
        _sentenceHeaderLayout->addWidget(_pronunciationLabels.back(),
                                         row,
                                         2,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _sentenceHeaderLayout
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
        _sentenceHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                         row,
                                         0,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            sentence.getMandarinPhonetic(MandarinOptions::NUMBERED_PINYIN)
                .c_str());
        _sentenceHeaderLayout->addWidget(_pronunciationLabels.back(),
                                         row,
                                         2,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _sentenceHeaderLayout
                ->addWidget(_mandarinTTS, row, 1, 1, 1, Qt::AlignTop);
            _mandarinTTS->setVisible(true);
            _mandarinTTSVisible = true;
        }

        row++;
    }

    if ((mandarinOptions & MandarinOptions::ZHUYIN)
        == MandarinOptions::ZHUYIN) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName("zhuyinTypeLabel");
        _sentenceHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                         row,
                                         0,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            sentence.getMandarinPhonetic(MandarinOptions::ZHUYIN)
                .c_str());
        _sentenceHeaderLayout->addWidget(_pronunciationLabels.back(),
                                         row,
                                         2,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _sentenceHeaderLayout
                ->addWidget(_mandarinTTS, row, 1, 1, 1, Qt::AlignTop);
            _mandarinTTS->setVisible(true);
            _mandarinTTSVisible = true;
        }

        row++;
    }

    if ((mandarinOptions & MandarinOptions::MANDARIN_IPA)
        == MandarinOptions::MANDARIN_IPA) {
        _pronunciationTypeLabels.emplace_back(new QLabel{this});
        _pronunciationTypeLabels.back()->setObjectName(
            "numberedPinyinTypeLabel");
        _sentenceHeaderLayout->addWidget(_pronunciationTypeLabels.back(),
                                         row,
                                         0,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationTypeLabels.back()->setVisible(true);

        _pronunciationLabels.emplace_back(new QLabel{this});
        _pronunciationLabels.back()->setText(
            sentence.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA).c_str());
        _sentenceHeaderLayout->addWidget(_pronunciationLabels.back(),
                                         row,
                                         2,
                                         1,
                                         1,
                                         Qt::AlignTop);
        _pronunciationLabels.back()->setVisible(true);

        if (!_mandarinTTSVisible) {
            _sentenceHeaderLayout
                ->addWidget(_mandarinTTS, row, 1, 1, 1, Qt::AlignTop);
            _mandarinTTS->setVisible(true);
            _mandarinTTSVisible = true;
        }

        row++;
    }
}

void SentenceViewHeaderWidget::clearPronunciationLabels(void)
{
    for (auto const &label : _pronunciationTypeLabels) {
        _sentenceHeaderLayout->removeWidget(label);
        delete label;
    }
    _pronunciationTypeLabels.clear();

    for (auto const &label : _pronunciationLabels) {
        _sentenceHeaderLayout->removeWidget(label);
        delete label;
    }
    _pronunciationLabels.clear();

    _cantoneseTTS->setVisible(false);
    _cantoneseTTSVisible = false;

    _mandarinTTS->setVisible(false);
    _mandarinTTSVisible = false;
}

void SentenceViewHeaderWidget::showError(const QString &reason, const QString &description)
{
    _message = new EntrySpeakErrorDialog{reason, description, this};
    _message->exec();
}
