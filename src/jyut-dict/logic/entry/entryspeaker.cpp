#include "entryspeaker.h"

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

int EntrySpeaker::speak(const QLocale::Language &language,
                        const QLocale::Country &country,
                        const QString &string)
{
    if (string.isEmpty()) {
        return -1;
    }

    QLocale locale = QLocale{language, country};
    if (locale.language() != language || locale.country() != country) {
        return -2;
    }
    _tts->setLocale(locale);

    auto voices = _tts->availableVoices();
    if (voices.isEmpty()) {
        return -3;
    }

    _tts->setVoice(voices.at(0));
    _tts->setRate(-0.25);
    _tts->say(string);
    return 0;
}

int EntrySpeaker::speakCantonese(const QString &string)
{
#ifdef Q_OS_LINUX
    return speak(QLocale::Cantonese, QLocale::HongKong, string);
#else
    return speak(QLocale::Chinese, QLocale::HongKong, string);
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
