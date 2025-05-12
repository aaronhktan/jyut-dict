#ifndef TRANSCRIBER_WRAPPER_H
#define TRANSCRIBER_WRAPPER_H

#include "iinputvolumepublisher.h"
#include "iinputvolumesubscriber.h"
#include "itranscriptionresultpublisher.h"
#include "itranscriptionresultsubscriber.h"

#include <string>
#include <unordered_set>

// The TranscriberWrapper class presents a C++ interface for the
// speech-to-text APIs in AVFoundation/Speech macOS frameworks
// or Windows Keyboard settings.

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

    // We have to publish transcription results to the C++ classes...
    void subscribe(ITranscriptionResultSubscriber *subscriber) override;
    void unsubscribe(ITranscriptionResultSubscriber *subscriber) override;
    void notifyTranscriptionResult(
        std::variant<std::system_error, std::string> result) override;

    // ... that we receive from the ITranscriptionResultPublisher
    void transcriptionResult(
        std::variant<std::system_error, std::string> result) override;

    void startRecognition();
    void stopRecognition();

private:
    // We have to use a void * here since Transcriber is an
    // Objective-C++ class on macOS. We can't include any Objective-C++
    // constructs in this C++ header.
    void *_transcriber = nullptr;

    std::unordered_set<IInputVolumeSubscriber *> _volumeSubscribers;
    std::unordered_set<ITranscriptionResultSubscriber *>
        _transcriptionSubscribers;
};

#endif // TRANSCRIBER_WRAPPER_H
