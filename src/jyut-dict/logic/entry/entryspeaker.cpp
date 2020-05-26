#include "entryspeaker.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)

#include <QVector>

EntrySpeaker::EntrySpeaker()
    : _tts{nullptr}
{
    _tts = new QTextToSpeech{};
}

EntrySpeaker::~EntrySpeaker()
{
    delete _tts;
}

// Returns list of voices that match the target locale
QVector<QVoice> EntrySpeaker::getListOfVoices(const QLocale::Language &language,
                                              const QLocale::Country &country)
{
    QLocale locale = QLocale{language, country};
    if (locale.language() != language || locale.country() != country) {
        return QVector<QVoice>();
    }
    _tts->setLocale(locale);
    return _tts->availableVoices();
}

#ifdef Q_OS_WIN
// On some versions of Windows, the Cantonese and Chinese voices are both
// labelled as "Chinese". It is impossible to distinguish what language they
// actually speak in other than doing pattern matching on the voice name
bool EntrySpeaker::checkVoiceName(const QLocale::Language &language,
                                  const QLocale::Country &country,
                                  const QString &voiceName)
{
    if (language == QLocale::Chinese && country == QLocale::China) {
        if (voiceName.contains("Huihui") || voiceName.contains("Yaoyao") ||
                voiceName.contains("Kangkang")) {
            return true;
        }
        return false;
    }
    if (language == QLocale::Chinese && country == QLocale::Taiwan) {
        if (voiceName.contains("Yating") || voiceName.contains("Hanhan") ||
                voiceName.contains("Zhiwei")) {
            return true;
        }
        return false;
    }
    if (language == QLocale::Chinese && country == QLocale::HongKong) {
        if (voiceName.contains("Tracy") || voiceName.contains("Danny")) {
            return true;
        }
        return false;
    }

    return false;
}

bool EntrySpeaker::filterVoiceNames(const QLocale::Language &language,
                                    const QLocale::Country &country,
                                    const QVector<QVoice> &voices,
                                    QVoice &voice) {
    for (auto checkVoice : voices) {
        if (checkVoiceName(language, country, checkVoice.name())) {
            voice = checkVoice;
            return true;
        }
    }
    return false;
}
#endif

int EntrySpeaker::speakWithVoice(const QVoice &voice, const QString &string) {
    _tts->setVoice(voice);
    _tts->setRate(-0.25);
    _tts->say(string);
    return 0;
}

int EntrySpeaker::speak(const QLocale::Language &language,
                        const QLocale::Country &country,
                        const QString &string)
{
    if (string.isEmpty()) {
        return -1;
    }

    auto voices = getListOfVoices(language, country);
    if (voices.isEmpty()) {
        return -2;
    }

    QVoice voice;
#ifdef Q_OS_WIN
    if (!filterVoiceNames(language, country, voices, voice)) {
        return -2;
    }
#else
    voice = voices.at(0);
#endif
    speakWithVoice(voice, string);
    return 0;
}

int EntrySpeaker::speakCantonese(const QString &string)
{
#ifdef Q_OS_LINUX
    return speak(QLocale::Cantonese, QLocale::HongKong, string);
#elif defined(Q_OS_MAC)
    return speak(QLocale::Chinese, QLocale::HongKong, string);
#else
    // On Windows, attempt to speak in zh-HK first, but if that
    // fails, get the list of zh_CN voices and see if any are Cantonese.
    // (This is because some Cantonese voices are labelled zh-CN)
    int rv = speak(QLocale::Chinese, QLocale::HongKong, string);
    if (rv) {
        QVector<QVoice> availableVoices = getListOfVoices(QLocale::Chinese,
                                                          QLocale::China);
        QVoice voice;
        if (!filterVoiceNames(QLocale::Chinese, QLocale::HongKong,
                              availableVoices, voice)) {
            return rv;
        }
        rv = speakWithVoice(voice, string);
    }
    return rv;
#endif
}

int EntrySpeaker::speakTaiwaneseMandarin(const QString &string)
{
    return speak(QLocale::Chinese, QLocale::Taiwan, string);
}

int EntrySpeaker::speakMainlandMandarin(const QString &string)
{
    return speak(QLocale::Chinese, QLocale::China, string);
}

#endif
