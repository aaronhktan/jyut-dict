#ifndef SPEECH_WRAPPER_H
#define SPEECH_WRAPPER_H

#include "iinputvolumepublisher.h"
#include "iinputvolumesubscriber.h"
#include "itranscriptionresultpublisher.h"
#include "itranscriptionresultsubscriber.h"

#include <string>
#include <unordered_set>

// The TranscriberWrapper class presents a C++ interface for the
// speech-to-text APIs in AVFoundation/Speech macOS frameworks.

class TranscriberWrapper : public IInputVolumePublisher,
                           public IInputVolumeSubscriber,
                           public ITranscriptionResultPublisher,
                           public ITranscriptionResultSubscriber
{
public:
    TranscriberWrapper(std::string &locale);
    ~TranscriberWrapper();

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
    // We have to use a void * pointer here since the Speech Helper
    // is an Objective-C++ class. We can't include any Objective-C++
    // constructs in this C++ header.
    void *_transcriber;

    std::unordered_set<IInputVolumeSubscriber *> _volumeSubscribers;
    std::unordered_set<ITranscriptionResultSubscriber *>
        _transcriptionSubscribers;
};

#endif // SPEECH_WRAPPER_H
