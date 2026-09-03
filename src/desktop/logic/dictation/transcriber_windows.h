#ifndef TRANSCRIBER_H
#define TRANSCRIBER_H

#include "itranscriptionresultpublisher.h"
#include "itranscriptionresultsubscriber.h"

#include <string>
#include <unordered_set>
#include <vector>

#include <Windows.h>

// The Transcriber class is kind of a misnomer on Windows.
// On Windows, the APIs to do the kind of speech recognition like on
// macOS don't exist. So what the Transcriber class does is instead
// activate the correct keyboard, and then send Win+H to start dictation.

class Transcriber : public ITranscriptionResultPublisher
{
public:
    Transcriber(std::string &locale);
    ~Transcriber();

    // We have to forward transcription results to the wrapper classes
    void subscribe(ITranscriptionResultSubscriber *subscriber) override;
    void unsubscribe(ITranscriptionResultSubscriber *subscriber) override;
    void notifyTranscriptionResult(
        std::variant<std::system_error, std::string> result) override;

    void startRecognition();
    void stopRecognition();

private:
    HKL _originalLayout;
    std::vector<std::wstring> _desiredLayoutPossibilities;
    std::unordered_set<ITranscriptionResultSubscriber *>
        _transcriptionSubscribers;
};

#endif // TRANSCRIBER_H
