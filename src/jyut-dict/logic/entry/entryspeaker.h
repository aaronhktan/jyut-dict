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

    EntrySpeaker(EntrySpeaker &other);
    EntrySpeaker(EntrySpeaker &&other);

    EntrySpeaker &operator=(const EntrySpeaker &other);
    EntrySpeaker &operator=(EntrySpeaker &&other);

    int speak(const QLocale::Language &language,
              const QLocale::Country &country,
              const QString &string) const;

    int speakCantonese(const QString &string) const;
    int speakTaiwaneseMandarin(const QString &string) const;
    int speakMainlandMandarin(const QString &string) const;

private:
    QVector<QVoice> getListOfVoices(const QLocale::Language &language,
                                    const QLocale::Country &country) const;
    int speakWithVoice(const QVoice &voice, const QString &string) const;
#ifdef Q_OS_WIN
    bool checkVoiceName(const QLocale::Language &language,
                        const QLocale::Country &country,
                        const QString &voiceName) const;
    bool filterVoiceNames(const QLocale::Language &language,
                          const QLocale::Country &country,
                          const QVector<QVoice> &voices,
                          QVoice &voice) const;
#endif

    QTextToSpeech *_tts;
};

#endif
#endif // ENTRYSPEAKER_H
