#include "sentenceviewheaderwidget.h"

#include "logic/settings/settingsutils.h"
#include "logic/strings/strings.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QtGlobal>
#include <QCoreApplication>
#include <QIcon>
#include <QTimer>
#include <QVariant>

SentenceViewHeaderWidget::SentenceViewHeaderWidget(QWidget *parent) : QWidget(parent)
{
    _entryHeaderLayout = new QGridLayout{this};
    _entryHeaderLayout->setContentsMargins(0, 0, 0, 0);
    _entryHeaderLayout->setSpacing(5);

    _speaker = std::make_unique<EntrySpeaker>();

    _simplifiedLabel = new QLabel{this};
    _simplifiedLabel->setStyleSheet("QLabel { font-size: 30px }");
    _simplifiedLabel->setAttribute(Qt::WA_TranslucentBackground);
    _simplifiedLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _simplifiedLabel->setWordWrap(true);

    _traditionalLabel = new QLabel{this};
    _traditionalLabel->setStyleSheet("QLabel { font-size: 30px }");
    _traditionalLabel->setAttribute(Qt::WA_TranslucentBackground);
    _traditionalLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _traditionalLabel->setWordWrap(true);

    _jyutpingLabel = new QLabel{this};
    _jyutpingLabel->setAttribute(Qt::WA_TranslucentBackground);
    _jyutpingLabel->setVisible(false);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    _jyutpingTTS = new QPushButton{this};
    _jyutpingTTS->setMaximumWidth(10);
    _jyutpingTTS->setFixedHeight(18);
    _jyutpingTTS->setAttribute(Qt::WA_TranslucentBackground);
    _jyutpingTTS->setVisible(false);
#endif
    _jyutpingPronunciation = new QLabel{this};
    _jyutpingPronunciation->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _jyutpingPronunciation->setWordWrap(true);

    _pinyinLabel = new QLabel{this};
    _pinyinLabel->setAttribute(Qt::WA_TranslucentBackground);
    _pinyinLabel->setVisible(false);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    _pinyinTTS = new QPushButton{this};
    _pinyinTTS->setMaximumWidth(10);
    _pinyinTTS->setFixedHeight(18);
    _pinyinTTS->setAttribute(Qt::WA_TranslucentBackground);
    _pinyinTTS->setVisible(false);
#endif
    _pinyinPronunciation = new QLabel{this};
    _pinyinPronunciation->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _pinyinPronunciation->setWordWrap(true);

    _entryHeaderLayout->addWidget(_simplifiedLabel, 1, 0, 1, -1);
    _entryHeaderLayout->addWidget(_traditionalLabel, 2, 0, 1, -1);
    _entryHeaderLayout->addWidget(_jyutpingLabel, 3, 0, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_jyutpingTTS, 3, 1, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_jyutpingPronunciation, 3, 2, 1, 1);
    _entryHeaderLayout->addWidget(_pinyinLabel, 4, 0, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_pinyinTTS, 4, 1, 1, 1, Qt::AlignTop);
    _entryHeaderLayout->addWidget(_pinyinPronunciation, 4, 2, 1, 1);

#ifdef Q_OS_MAC
    setStyle(Utils::isDarkMode());
#else
    setStyle(/* use_dark = */false);
#endif
    translateUI();
}

SentenceViewHeaderWidget::~SentenceViewHeaderWidget()
{
}

void SentenceViewHeaderWidget::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::FontChange) {
        _jyutpingLabel->setFixedWidth(_jyutpingLabel->fontMetrics().boundingRect("JP").width());
        _pinyinLabel->setFixedWidth(_pinyinLabel->fontMetrics().boundingRect("PY").width());
    }
#endif
#if defined(Q_OS_DARWIN)
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void SentenceViewHeaderWidget::setSourceSentence(const SourceSentence &sentence)
{
    _jyutpingLabel->setVisible(true);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    _jyutpingTTS->setVisible(true);
#endif
    _pinyinLabel->setVisible(true);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    _pinyinTTS->setVisible(true);
#endif

    _simplifiedLabel->setText(
        QString{sentence.getSimplified().c_str()}.trimmed());
    _traditionalLabel->setText(
        QString{sentence.getTraditional().c_str()}.trimmed());

    _jyutpingPronunciation->setText(QString{
        sentence
            .getCantonesePhonetic(
                Settings::getSettings()
                    ->value("cantoneseOptions",
                            QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
                    .value<CantoneseOptions>())
            .c_str()}.trimmed());
    _pinyinPronunciation->setText(QString{
        sentence
            .getMandarinPhonetic(
                Settings::getSettings()
                    ->value("mandarinOptions",
                            QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                    .value<MandarinOptions>())
            .c_str()}.trimmed());

    displaySentenceLabels(
        Settings::getSettings()
            ->value("characterOptions",
                    QVariant::fromValue(
                        EntryCharactersOptions::PREFER_TRADITIONAL))
            .value<EntryCharactersOptions>());
    displayPronunciationLabels(
        Settings::getSettings()
            ->value("phoneticOptions",
                    QVariant::fromValue(EntryPhoneticOptions::PREFER_JYUTPING))
            .value<EntryPhoneticOptions>());

    _chinese = QString{sentence.getSimplified().c_str()};
    _jyutping = QString{sentence.getJyutping().c_str()};
    _pinyin = QString{sentence.getPinyin().c_str()};

#ifdef Q_OS_WIN
    QFont font = QFont{"Microsoft YaHei", 30};
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    _wordLabel->setFont(font);
#endif
}

void SentenceViewHeaderWidget::setStyle(bool use_dark)
{
    _simplifiedLabel->setStyleSheet("QLabel { font-size: 30px }");
    _traditionalLabel->setStyleSheet("QLabel { font-size: 30px }");

    QString styleSheet = "QLabel { color: %1; }";
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_R};
    _jyutpingLabel->setStyleSheet(styleSheet.arg(textColour.name()));
    _pinyinLabel->setStyleSheet(styleSheet.arg(textColour.name()));

#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    _jyutpingTTS->setIcon(use_dark ? QIcon{":/images/speak_inverted.png"}
                                   : QIcon{":/images/speak.png"});
    _jyutpingTTS->setFlat(true);
    _jyutpingTTS->setObjectName("jyutpingTTS");
    _jyutpingTTS->setStyleSheet(
        "QPushButton#jyutpingTTS { padding: 0px; }"
        "QPushButton:pressed#jyutpingTTS { border: none; }");

    _pinyinTTS->setIcon(use_dark ? QIcon{":/images/speak_inverted.png"}
                                 : QIcon{":/images/speak.png"});
    _pinyinTTS->setFlat(true);
    _pinyinTTS->setObjectName("pinyinTTS");
    _pinyinTTS->setStyleSheet("QPushButton#pinyinTTS { padding: 0px; }"
                              "QPushButton:pressed#pinyinTTS { border: none; }");
#endif
}

void SentenceViewHeaderWidget::translateUI()
{
    _jyutpingLabel->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                        Strings::JYUTPING_SHORT));
    _jyutpingLabel->setFixedWidth(
        _jyutpingLabel->fontMetrics().boundingRect(_jyutpingLabel->text()).width());

    _pinyinLabel->setText(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                                      Strings::PINYIN_SHORT));
    _pinyinLabel->setFixedWidth(
        _pinyinLabel->fontMetrics().boundingRect(_pinyinLabel->text()).width());

#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    disconnect(_jyutpingTTS, nullptr, nullptr, nullptr);
    connect(_jyutpingTTS, &QPushButton::clicked, this, [=]() {
        if (!_speaker->speakCantonese(_chinese)) {
            return;
        }
        showError(QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                              Strings::YUE_ERROR_STRING),
                  QCoreApplication::translate(Strings::STRINGS_CONTEXT,
                                              Strings::YUE_DESCRIPTION_STRING)
                      .arg(Settings::getCurrentLocale().bcp47Name()));
    });

    disconnect(_pinyinTTS, nullptr, nullptr, nullptr);
    if (Settings::getCurrentLocale().country() == QLocale::Taiwan) {
        connect(_pinyinTTS, &QPushButton::clicked, this, [=]() {
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
        connect(_pinyinTTS, &QPushButton::clicked, this, [=]() {
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
#endif
}

void SentenceViewHeaderWidget::displaySentenceLabels(
    const EntryCharactersOptions options)
{
    // Display the first label
    switch (options) {
    case EntryCharactersOptions::ONLY_SIMPLIFIED:
        [[clang::fallthrough]];
    case EntryCharactersOptions::PREFER_SIMPLIFIED:
        _entryHeaderLayout->addWidget(_simplifiedLabel, 1, 0, 1, -1);
        break;
    case EntryCharactersOptions::ONLY_TRADITIONAL:
        [[clang::fallthrough]];
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        _entryHeaderLayout->addWidget(_traditionalLabel, 1, 0, 1, -1);
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
        _entryHeaderLayout->addWidget(_traditionalLabel, 2, 0, 1, -1);
        break;
    case EntryCharactersOptions::PREFER_TRADITIONAL:
        _entryHeaderLayout->addWidget(_simplifiedLabel, 2, 0, 1, -1);
        break;
    }
}

void SentenceViewHeaderWidget::displayPronunciationLabels(const EntryPhoneticOptions options)
{
    switch (options) {
        case EntryPhoneticOptions::PREFER_JYUTPING: {
            _entryHeaderLayout->addWidget(_jyutpingLabel, 3, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_jyutpingTTS, 3, 1, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_jyutpingPronunciation, 3, 2, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_pinyinLabel, 4, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_pinyinTTS, 4, 1, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_pinyinPronunciation, 4, 2, 1, 1, Qt::AlignTop);
            _jyutpingLabel->setVisible(true);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
            _jyutpingTTS->setVisible(true);
#endif
            _jyutpingPronunciation->setVisible(true);
            _pinyinLabel->setVisible(true);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
            _pinyinTTS->setVisible(true);
#endif
            _pinyinPronunciation->setVisible(true);
            break;
        }
        case EntryPhoneticOptions::PREFER_PINYIN: {
            _entryHeaderLayout->addWidget(_pinyinLabel, 3, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_pinyinTTS, 3, 1, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_pinyinPronunciation, 3, 2, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_jyutpingLabel, 4, 0, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_jyutpingTTS, 4, 1, 1, 1, Qt::AlignTop);
            _entryHeaderLayout->addWidget(_jyutpingPronunciation, 4, 2, 1, 1, Qt::AlignTop);
            _pinyinLabel->setVisible(true);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
            _pinyinTTS->setVisible(true);
#endif
            _pinyinPronunciation->setVisible(true);
            _jyutpingLabel->setVisible(true);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
            _jyutpingTTS->setVisible(true);
#endif
            _jyutpingPronunciation->setVisible(true);
            break;
        }
        case EntryPhoneticOptions::ONLY_JYUTPING: {
            _jyutpingLabel->setVisible(true);
            _jyutpingTTS->setVisible(true);
            _jyutpingPronunciation->setVisible(true);
            _pinyinLabel->setVisible(false);
            _pinyinTTS->setVisible(false);
            _pinyinPronunciation->setVisible(false);
            break;
        }
        case EntryPhoneticOptions::ONLY_PINYIN: {
            _jyutpingLabel->setVisible(false);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
            _jyutpingTTS->setVisible(false);
#endif
            _jyutpingPronunciation->setVisible(false);
            _pinyinLabel->setVisible(true);
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
            _pinyinTTS->setVisible(true);
#endif
            _pinyinPronunciation->setVisible(true);
            break;
        }
    }
}

void SentenceViewHeaderWidget::showError(const QString &reason, const QString &description)
{
    _message = new EntrySpeakErrorDialog{reason, description, this};
    _message->exec();
}
