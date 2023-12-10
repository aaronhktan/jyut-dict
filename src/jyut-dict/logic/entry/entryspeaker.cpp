#include "entryspeaker.h"
#include "logic/utils/chineseutils.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMediaPlaylist>
#include <QStandardPaths>
#include <QVector>

#include <cerrno>

EntrySpeaker::EntrySpeaker()
    : _tts{new QTextToSpeech}
    , _player{new QMediaPlayer}
{}

EntrySpeaker::~EntrySpeaker()
{
    delete _tts;
}

EntrySpeaker::EntrySpeaker(EntrySpeaker &other)
    : _tts{new QTextToSpeech}
{
    // Qt classes cannot be copy-constructed, so just create a new one
    // instead of copying from other.
    (void) (other);
}

EntrySpeaker::EntrySpeaker(EntrySpeaker &&other)
    : _tts{new QTextToSpeech{other._tts}}
{
    other._tts = nullptr;
}

EntrySpeaker &EntrySpeaker::operator=(const EntrySpeaker &other)
{
    // Again, copy-constructor is explicitly deleted for Qt classes, so
    // just make a new QTextToSpeech object
    if (&other != this) {
        _tts = new QTextToSpeech;
    }

    return *this;
}

EntrySpeaker &EntrySpeaker::operator=(EntrySpeaker &&other)
{
    if (&other != this) {
        _tts = other._tts;
        other._tts = nullptr;
    }

    return *this;
}

// Returns list of voices that match the target locale
QVector<QVoice> EntrySpeaker::getListOfVoices(const QLocale::Language &language,
                                              const QLocale::Country &country)
const
{
    QLocale locale = QLocale{language, country};
    if (locale.language() != language || locale.territory() != country) {
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
                                  const QString &voiceName) const
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
                                    QVoice &voice) const {
    for (auto checkVoice : voices) {
        if (checkVoiceName(language, country, checkVoice.name())) {
            voice = checkVoice;
            return true;
        }
    }
    return false;
}
#endif

int EntrySpeaker::speakWithVoice(const QVoice &voice, const QString &string) const {
    _tts->setVoice(voice);
    _tts->setRate(-0.25);
    _tts->say(string);
    return 0;
}

int EntrySpeaker::speak(const QLocale::Language &language,
                        const QLocale::Country &country,
                        const QString &string,
                        const TextToSpeech::SpeakerBackend backend,
                        const TextToSpeech::SpeakerVoice voice) const
{
    if (string.isEmpty()) {
        return -EINVAL;
    }

    if ((backend == TextToSpeech::SpeakerBackend::QT_TTS)
        && (voice != TextToSpeech::SpeakerVoice::NONE)) {
        // Setting voices is not supported using Qt TTS
        return -EINVAL;
    }

    if ((backend != TextToSpeech::SpeakerBackend::QT_TTS)
        && (voice == TextToSpeech::SpeakerVoice::NONE)) {
        // Backends other than Qt must specify a voice name
        return -EINVAL;
    }

    switch (backend) {
    case TextToSpeech::SpeakerBackend::QT_TTS: {
        auto voices = getListOfVoices(language, country);
        if (voices.isEmpty()) {
            return -ENOENT;
        }

        QVoice voice;
#ifdef Q_OS_WIN
        if (!filterVoiceNames(language, country, voices, voice)) {
            return -ENOENT;
        }
#else
        voice = voices.at(0);
#endif
        speakWithVoice(voice, string);
        break;
    }
    case TextToSpeech::SpeakerBackend::GOOGLE_OFFLINE_SYLLABLE_TTS: {
        QString languageName = QLocale::languageToString(language) + "_"
                               + QLocale::countryToString(country);

        std::vector<std::string> syllables;
        if (language == QLocale::Cantonese || country == QLocale::HongKong) {
            ChineseUtils::segmentJyutping(string, syllables);
        } else {
            QString mutableString = string;
            mutableString.replace("u:", "ü");
            mutableString.replace("v", "ü");
            ChineseUtils::segmentPinyin(mutableString, syllables);
        }

        QMediaPlaylist *playlist = new QMediaPlaylist;
        for (const auto &syllable : syllables) {
            QString filepath = getAudioPath()
                               + QString{"%1/%2/%3/%4.mp3"}
                                     .arg(TextToSpeech::backendNames[backend],
                                          languageName,
                                          TextToSpeech::voiceNames[voice],
                                          QString::fromStdString(syllable));
            if (!QFileInfo::exists(filepath)) {
                qDebug() << "File " << filepath << " does not exist";
            }
            QUrl url = QUrl::fromLocalFile(filepath);
            playlist->addMedia(url);
        }
        _player->setPlaylist(playlist);
        _player->play();
        break;
    }
    }

    return 0;
}

int EntrySpeaker::speakCantonese(const QString &string,
                                 const TextToSpeech::SpeakerBackend backend,
                                 const TextToSpeech::SpeakerVoice voice) const
{
    if (backend != TextToSpeech::SpeakerBackend::QT_TTS) {
        return speak(QLocale::Chinese, QLocale::HongKong, string, backend, voice);
    } else {
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
            QVoice qVoice;
            if (!filterVoiceNames(QLocale::Chinese,
                                  QLocale::HongKong,
                                  availableVoices,
                                  qVoice)) {
                return rv;
            }
            rv = speakWithVoice(qVoice, string);
        }
        return rv;
#endif
    }
}

int EntrySpeaker::speakTaiwaneseMandarin(
    const QString &string,
    const TextToSpeech::SpeakerBackend backend,
    const TextToSpeech::SpeakerVoice voice) const
{
    return speak(QLocale::Chinese, QLocale::Taiwan, string, backend, voice);
}

int EntrySpeaker::speakMainlandMandarin(
    const QString &string,
    const TextToSpeech::SpeakerBackend backend,
    const TextToSpeech::SpeakerVoice voice) const
{
    return speak(QLocale::Chinese, QLocale::China, string, backend, voice);
}

QString EntrySpeaker::getAudioPath()
{
#ifdef PORTABLE
    return getBundleAudioPath();
#else
    return getLocalAudioPath();
#endif
}

QString EntrySpeaker::getLocalAudioPath()
{
#ifdef Q_OS_DARWIN
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Audio/"};
#elif defined(Q_OS_WIN)
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/Audio/"};
#else
    QFileInfo localFile{
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/audio/"};
#endif
    return localFile.absoluteFilePath();
}

QString EntrySpeaker::getBundleAudioPath()
{
#ifdef Q_OS_DARWIN
    QFileInfo bundlePath{QCoreApplication::applicationDirPath()
                         + "/../Resources/Audio/"};
#elif defined(Q_OS_WIN)
    QFileInfo bundlePath{QCoreApplication::applicationDirPath() + "./Audio/"};
#else // Q_OS_LINUX
#ifdef APPIMAGE
    QFileInfo bundlePath{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/audio/"};
#elif defined(DEBUG)
    QFileInfo bundlePath{"./audio/"};
#elif defined(FLATPAK)
    QFileInfo bundlePath{QCoreApplication::applicationDirPath()
                         + "/../share/jyut-dict/audio/"};
#else
    QFileInfo bundlePath{"/usr/share/jyut-dict/audio/"};
#endif
#endif
    return bundlePath.absoluteFilePath();
}
