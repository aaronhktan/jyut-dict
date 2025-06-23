#include "entryspeaker.h"
#include "logic/utils/cantoneseutils.h"
#include "logic/utils/mandarinutils.h"

#ifdef Q_OS_MAC
#include "logic/audio/synthesizer_mac.h"
#endif

#include <KZip>

#include <QCoreApplication>
#include <QFileInfo>
#include <QStandardPaths>
#include <QVector>
#include <QtConcurrent/QtConcurrent>

#include <cerrno>
#include <iostream>
#include <thread>

EntrySpeaker::EntrySpeaker()
    : QObject{}
    , _tts{new QTextToSpeech}
{
    _engine = (ma_engine *) malloc(sizeof(*_engine));
    ma_result result = ma_engine_init(NULL, _engine);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize miniaudio engine!" << std::endl;
    }

#ifdef Q_OS_MAC
    _synthesizer = std::make_unique<SynthesizerWrapper>();
#endif

#ifdef Q_OS_LINUX
    _boolReturnWatcher = new QFutureWatcher<bool>{this};
    QFuture<bool> future = QtConcurrent::run([=, this]() {
        KZip zip{getBundleAudioPath() + "audio.zip"};
        if (!zip.open(QIODevice::ReadOnly)) {
            std::cerr << "Failed to read audio zip file!" << std::endl;
            return false;
        }
        if (!zip.directory()->copyTo(getAudioPath())) {
            std::cerr << "Failed to unzip audio files!" << std::endl;
            return false;
        }
        return true;
    });
    _boolReturnWatcher->setFuture(future);
#endif
}

EntrySpeaker::~EntrySpeaker()
{
    delete _tts;
    ma_engine_uninit(_engine);
    delete _engine;
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

int EntrySpeaker::speakWithVoice(const QVoice &voice,
                                 const QString &string) const
{
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
#ifdef Q_OS_MAC
        // It seems like QLocale::Country is broken for China and Taiwan
        bool res = false;
        switch (country) {
        case QLocale::China: {
            res = _synthesizer->setLocale(
                QLocale{language}.bcp47Name().toStdString() + "_CN");
            break;
        }
        case QLocale::Taiwan: {
            res = _synthesizer->setLocale(
                QLocale{language}.bcp47Name().toStdString() + "_TW");
            break;
        }
        case QLocale::HongKong: {
            res = _synthesizer->setLocale(
                QLocale{language}.bcp47Name().toStdString() + "_HK");
            break;
        }
        default:
            break;
        }

        if (!res) {
            return -ENOENT;
        }

        _synthesizer->speak(string.toStdString());
        break;
#endif
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
                               + (country == QLocale::HongKong
                                      ? "Hong Kong"
                                      : QLocale::territoryToString(country));

        std::vector<std::string> syllables;
        if (language == QLocale::Cantonese || country == QLocale::HongKong) {
            CantoneseUtils::segmentJyutping(string, syllables);
        } else {
            QString mutableString = string;
            mutableString.replace("u:", "ü");
            mutableString.replace("v", "ü");
            MandarinUtils::segmentPinyin(mutableString, syllables);
        }

        std::ignore = QtConcurrent::run([=, this]() {
#ifdef Q_OS_LINUX
            while (_boolReturnWatcher && _boolReturnWatcher->isRunning()) {
                std::this_thread::sleep_for(std::chrono::milliseconds{50});
            }
#endif

            for (const auto &syllable : syllables) {
                QString filepath
                    = getAudioPath()
                      + QString{"%1/%2/%3/%4.mp3"}
                            .arg(TextToSpeech::backendNames[backend],
                                 languageName,
                                 TextToSpeech::voiceNames[voice],
                                 QString::fromStdString(syllable));
                if (!QFileInfo::exists(filepath)) {
                    std::cerr << "File " << filepath.toStdString()
                              << " does not exist" << std::endl;
                }

                ma_sound sound;
                ma_result result
                    = ma_sound_init_from_file(_engine,
                                              filepath.toStdString().c_str(),
                                              0,
                                              NULL,
                                              NULL,
                                              &sound);

                if (result != MA_SUCCESS) {
                    continue;
                }

                ma_sound_start(&sound);
                while (ma_sound_is_playing(&sound)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds{100});
                    continue;
                }

                ma_sound_uninit(&sound);
            }
        });
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
