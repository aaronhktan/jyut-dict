#include "transcriberwrapper.h"

#import "transcriber_mac.h"

#import <Speech/Speech.h>

TranscriberWrapper::TranscriberWrapper(std::string &locale)
{
    // __bridge_retained allows us to treat the Objective-C object as
    // a C-style pointer, transferring ownership to us. This means
    // ARC no longer governs the lifecycle of the object, and we are
    // now responsible for manually releasing the object when we are done
    // with it.
    _transcriber = (__bridge_retained void *) [[Transcriber alloc]
        initWithLocaleIdentifier:[NSString stringWithUTF8String:locale.c_str()]];

    if (!_transcriber) {
        throw std::system_error{ENETDOWN,
                                std::generic_category(),
                                "Could not initialize transcriber because language unavailable"};
    }

    // In order to use the (C-style) Transcriber pointer as an Objective-C class,
    // we must __bridge it. No ownership is transferred in this case, so we are
    // still responsible for releasing it ourselves.
    Transcriber *speechHelper = (__bridge Transcriber *) _transcriber;
    [speechHelper subscribeForTranscript:this];
    [speechHelper subscribeForVolume:this];
}

TranscriberWrapper::~TranscriberWrapper()
{
    Transcriber *speechHelper = (__bridge Transcriber *) _transcriber;
    [speechHelper unsubscribeForTranscript:this];
    [speechHelper unsubscribeForVolume:this];

    if (_transcriber) {
        CFRelease(_transcriber);
    }
}

void TranscriberWrapper::subscribe(IInputVolumeSubscriber *subscriber)
{
    _volumeSubscribers.emplace(subscriber);
}

void TranscriberWrapper::unsubscribe(IInputVolumeSubscriber *subscriber)
{
    _volumeSubscribers.extract(subscriber);
}

void TranscriberWrapper::notifyVolumeResult(Utils::Result<float> result)
{
    for (const auto s : _volumeSubscribers) {
        s->volumeResult(result);
    }
}

void TranscriberWrapper::volumeResult(Utils::Result<float> result)
{
    notifyVolumeResult(result);
}

void TranscriberWrapper::subscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.emplace(subscriber);
}

void TranscriberWrapper::unsubscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.extract(subscriber);
}

void TranscriberWrapper::notifyTranscriptionResult(Utils::Result<std::string> result)
{
    for (const auto s : _transcriptionSubscribers) {
        s->transcriptionResult(result);
    }
}

void TranscriberWrapper::transcriptionResult(Utils::Result<std::string> transcription)
{
    notifyTranscriptionResult(transcription);
}

void TranscriberWrapper::startRecognition()
{
    Transcriber *speechHelper = (__bridge Transcriber *) _transcriber;
    [speechHelper start];
}

void TranscriberWrapper::stopRecognition()
{
    // Ensure the helper is initialized before stopping
    if (_transcriber) {
        Transcriber *speechHelper = (__bridge Transcriber *) _transcriber;
        [speechHelper stop];
    } else {
        NSLog(@"SpeechHelper is not initialized or has been released.");
    }
}
