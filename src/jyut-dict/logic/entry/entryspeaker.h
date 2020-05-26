#ifndef ENTRYSPEAKER_H
#define ENTRYSPEAKER_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#include <QString>
#include <QTextToSpeech>

class EntrySpeaker
{
public:
    EntrySpeaker();
    ~EntrySpeaker();

    int speak(const QLocale::Language &language,
              const QLocale::Country &country,
              const QString &string);

    int speakCantonese(const QString &string);
    int speakTaiwaneseMandarin(const QString &string);
    int speakMainlandMandarin(const QString &string);

private:
    QVector<QVoice> getListOfVoices(const QLocale::Language &language,
                                    const QLocale::Country &country);
    int speakWithVoice(const QVoice &voice, const QString &string);
#ifdef Q_OS_WIN
    bool checkVoiceName(const QLocale::Language &language,
                        const QLocale::Country &country,
                        const QString &voiceName);
    bool filterVoiceNames(const QLocale::Language &language,
                          const QLocale::Country &country,
                          const QVector<QVoice> &voices,
                          QVoice &voice);
#endif

    QTextToSpeech *_tts;
};

#endif
#endif // ENTRYSPEAKER_H
