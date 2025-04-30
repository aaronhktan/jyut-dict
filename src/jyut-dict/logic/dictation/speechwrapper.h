#ifndef SPEECH_WRAPPER_H
#define SPEECH_WRAPPER_H

#include "itranscriptionresultpublisher.h"
#include "itranscriptionresultsubscriber.h"

#include <string>
#include <unordered_set>

// The SpeechWrapper class presents a C++ interface for the
// speech-to-text APIs in AVFoundation/Speech macOS frameworks.

class SpeechWrapper : public ITranscriptionResultPublisher,
                      public ITranscriptionResultSubscriber
{
public:
    SpeechWrapper(std::string &locale);
    ~SpeechWrapper();

    // We have to forward transcription results to the C++ classes
    void subscribe(ITranscriptionResultSubscriber *subscriber) override;
    void unsubscribe(ITranscriptionResultSubscriber *subscriber) override;
    void notifySubscribers(std::variant<bool, std::string> result) override;

    void transcriptionResult(std::variant<bool, std::string> result) override;

    void startRecognition();
    void stopRecognition();

private:
    // We have to use a void * pointer here since the Speech Helper
    // is an Objective-C++ class. We can't include any Objective-C++
    // constructs in this C++ header.
    void *_helper;

    std::unordered_set<ITranscriptionResultSubscriber *> _subscribers;
};

#endif // SPEECH_WRAPPER_H
