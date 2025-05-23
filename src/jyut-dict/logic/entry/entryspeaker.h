#ifndef ENTRYSPEAKER_H
#define ENTRYSPEAKER_H

#include <QMediaPlayer>
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#include <QString>
#include <QTextToSpeech>

namespace TextToSpeech {

enum class SpeakerBackend : int {
    QT_TTS,

    GOOGLE_OFFLINE_SYLLABLE_TTS = 10,
};

// This enumeration is not currently being exposed via the UI,
// but is defined for experimentation and future expansion
enum class SpeakerVoice : int {
    NONE,

    YUE_1 = 10,
    YUE_2,

    CMN_1 = 20,
    CMN_2,
};

static QMap<TextToSpeech::SpeakerBackend, QString> backendNames{
    {TextToSpeech::SpeakerBackend::QT_TTS, "Qt"},
    {TextToSpeech::SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS, "Google"},
};

static QMap<TextToSpeech::SpeakerVoice, QString> voiceNames{
    {TextToSpeech::SpeakerVoice::NONE, ""},
    {TextToSpeech::SpeakerVoice::YUE_1, "YUE_1"},
    {TextToSpeech::SpeakerVoice::YUE_2, "YUE_2"},
    {TextToSpeech::SpeakerVoice::CMN_1, "CMN_1"},
    {TextToSpeech::SpeakerVoice::CMN_2, "CMN_2"},
};

} // namespace TextToSpeech

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
              const TextToSpeech::SpeakerBackend backend
              = TextToSpeech::SpeakerBackend::QT_TTS,
              const TextToSpeech::SpeakerVoice voiceName
              = TextToSpeech::SpeakerVoice::NONE) const;

    int speakCantonese(const QString &string,
                       const TextToSpeech::SpeakerBackend backend
                       = TextToSpeech::SpeakerBackend::QT_TTS,
                       const TextToSpeech::SpeakerVoice voiceName
                       = TextToSpeech::SpeakerVoice::NONE) const;
    int speakTaiwaneseMandarin(const QString &string,
                               const TextToSpeech::SpeakerBackend backend
                               = TextToSpeech::SpeakerBackend::QT_TTS,
                               const TextToSpeech::SpeakerVoice voiceName
                               = TextToSpeech::SpeakerVoice::NONE) const;
    int speakMainlandMandarin(const QString &string,
                              const TextToSpeech::SpeakerBackend backend
                              = TextToSpeech::SpeakerBackend::QT_TTS,
                              const TextToSpeech::SpeakerVoice voiceName
                              = TextToSpeech::SpeakerVoice::NONE) const;

    static QString getAudioPath();
    static QString getLocalAudioPath();
    static QString getBundleAudioPath();

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
    QMediaPlayer *_player;
};

Q_DECLARE_METATYPE(TextToSpeech::SpeakerBackend);
Q_DECLARE_METATYPE(TextToSpeech::SpeakerVoice);

#endif
#endif // ENTRYSPEAKER_H
