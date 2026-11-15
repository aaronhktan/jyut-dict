#include "transcriberwrapper.h"

#include "transcriber_windows.h"

#include <iostream>

TranscriberWrapper::TranscriberWrapper(std::string &locale)
{
    _transcriber = new Transcriber(locale);
    static_cast<Transcriber *>(_transcriber)->subscribe(static_cast<ITranscriptionResultSubscriber *>(this));
}

TranscriberWrapper::~TranscriberWrapper()
{
    delete static_cast<Transcriber *>(_transcriber);
}

// On Windows, the input volume is not sampled, so these functions are
// unimplemented.
void TranscriberWrapper::subscribe(IInputVolumeSubscriber *subscriber)
{
    std::cerr
        << "TranscriberWrapper::subscribe called but not implemented on Windows"
        << std::endl;
}

void TranscriberWrapper::unsubscribe(IInputVolumeSubscriber *subscriber)
{
    std::cerr << "TranscriberWrapper::unsubscribe called but not implemented "
                 "on Windows"
              << std::endl;
}

void TranscriberWrapper::notifyVolumeResult(std::variant<std::system_error, float> result)
{
    std::cerr << "TranscriberWrapper::notifyVolumeResult called but not "
                 "implemented on Windows"
              << std::endl;
}

void TranscriberWrapper::volumeResult(std::variant<std::system_error, float> result)
{
    std::cerr << "TranscriberWrapper::volumeResult called but not "
                 "implemented on Windows"
              << std::endl;
}

void TranscriberWrapper::subscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.emplace(subscriber);
}

void TranscriberWrapper::unsubscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.extract(subscriber);
}

void TranscriberWrapper::notifyTranscriptionResult(
    std::variant<std::system_error, std::string> result)
{
    for (const auto s : _transcriptionSubscribers) {
        s->transcriptionResult(result);
    }
}

void TranscriberWrapper::transcriptionResult(
    std::variant<std::system_error, std::string> transcription)
{
    notifyTranscriptionResult(transcription);
}

void TranscriberWrapper::startRecognition()
{
    static_cast<Transcriber *>(_transcriber)->startRecognition();
}

void TranscriberWrapper::stopRecognition()
{
    static_cast<Transcriber *>(_transcriber)->stopRecognition();
}
