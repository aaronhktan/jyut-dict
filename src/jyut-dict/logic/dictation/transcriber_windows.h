#ifndef TRANSCRIBER_H
#define TRANSCRIBER_H

#include "iinputvolumepublisher.h"
#include "iinputvolumesubscriber.h"
#include "itranscriptionresultpublisher.h"
#include "itranscriptionresultsubscriber.h"

#define INITGUID
#include <Windows.h>
#include <sapi.h>
#include <sphelper.h>

#include <initguid.h>
#include <objbase.h>

#include <string>
#include <unordered_set>

// The TranscriberWrapper class presents a C++ interface for the
// speech-to-text APIs in AVFoundation/Speech macOS frameworks.

class Transcriber    : public IInputVolumePublisher,
                           public IInputVolumeSubscriber,
                           public ITranscriptionResultPublisher,
                           public ITranscriptionResultSubscriber
{
public:
    Transcriber(std::string &locale);
    ~Transcriber();

    void subscribe(IInputVolumeSubscriber *subsciber) override;
    void unsubscribe(IInputVolumeSubscriber *subscriber) override;
    void notifyVolumeResult(
        std::variant<std::system_error, float> result) override;

    void volumeResult(std::variant<std::system_error, float> result) override;

    // We have to forward transcription results to the C++ classes
    void subscribe(ITranscriptionResultSubscriber *subscriber) override;
    void unsubscribe(ITranscriptionResultSubscriber *subscriber) override;
    void notifyTranscriptionResult(
        std::variant<std::system_error, std::string> result) override;

    void transcriptionResult(
        std::variant<std::system_error, std::string> result) override;

    void startRecognition();
    void stopRecognition();

private:
    ISpVoice *voice = nullptr;
    ISpRecognizer *recognizer = nullptr;
    ISpAudio *audio = nullptr;
    ISpRecoContext *recoContext = nullptr;
    ISpRecoGrammar *grammar = nullptr;
    HKL originalLayout;

    std::unordered_set<IInputVolumeSubscriber *> _volumeSubscribers;
    std::unordered_set<ITranscriptionResultSubscriber *>
        _transcriptionSubscribers;
};

#endif // TRANSCRIBER_H
