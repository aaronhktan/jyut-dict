#ifndef ENTRYSPEAKER_H
#define ENTRYSPEAKER_H

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#include <QString>
#include <QTextToSpeech>

enum class SpeakerBackend : int {
    QT_TTS,

    GOOGLE_OFFLINE_SYLLABLE_TTS = 10,
};

enum class Voice : int {
    NONE,

    YUE_1 = 10,
    YUE_2,

    CMN_1 = 20,
    CMN_2,
};

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
              const QString &string,
              const SpeakerBackend backend = SpeakerBackend::QT_TTS,
              const Voice voiceName = Voice::NONE) const;

    int speakCantonese(const QString &string,
                       const SpeakerBackend backend = SpeakerBackend::QT_TTS,
                       const Voice voiceName = Voice::NONE) const;
    int speakTaiwaneseMandarin(const QString &string,
                               const SpeakerBackend backend
                               = SpeakerBackend::QT_TTS,
                               const Voice voiceName = Voice::NONE) const;
    int speakMainlandMandarin(const QString &string,
                              const SpeakerBackend backend
                              = SpeakerBackend::QT_TTS,
                              const Voice voiceName = Voice::NONE) const;

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

    QString getAudioPath() const;
    QString getLocalAudioPath() const;
    QString getBundleAudioPath() const;

    QTextToSpeech *_tts;
    QAudioOutput *_output;
    QMediaPlayer *_player;

    QMap<SpeakerBackend, QString> _backendNames{
        {SpeakerBackend::QT_TTS, "Qt"},
        {SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS, "Google"},
    };

    QMap<Voice, QString> _voiceNames{
        {Voice::NONE, ""},
        {Voice::YUE_1, "YUE_1"},
        {Voice::YUE_2, "YUE_2"},
    };
};

Q_DECLARE_METATYPE(SpeakerBackend);
Q_DECLARE_METATYPE(Voice);

#endif
#endif // ENTRYSPEAKER_H
