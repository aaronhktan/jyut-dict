#include "transcriberwrapper.h"

#import "transcriber.h"

#include <QDebug>

#import <Speech/Speech.h>

TranscriberWrapper::TranscriberWrapper(std::string &locale)
{
    // __bridge_retained allows us to treat the Objective-C object as
    // a C-style pointer, transferring ownership to us. This means
    // ARC no longer governs the lifecycle of the object, and we are
    // now responsible for manually releasing the object when we are done
    // with it.
    _helper = (__bridge_retained void *) [[Transcriber alloc]
        initWithLocaleIdentifier:[NSString stringWithUTF8String:locale.c_str()]];

    // In order to use the (C-style) Transcriber pointer as an Objective-C class,
    // we must __bridge it. No ownership is transferred in this case, so we are
    // still responsible for releasing it ourselves.
    Transcriber *speechHelper = (__bridge Transcriber *) _helper;
    [speechHelper subscribe:this];
}

TranscriberWrapper::~TranscriberWrapper()
{
    if (_helper) {
        CFRelease(_helper);
    }
}

void TranscriberWrapper::subscribe(ITranscriptionResultSubscriber *subscriber)
{
    _subscribers.emplace(subscriber);
}

void TranscriberWrapper::unsubscribe(ITranscriptionResultSubscriber *subscriber)
{
    _subscribers.extract(subscriber);
}

void TranscriberWrapper::notifySubscribers(std::variant<bool, std::string> result)
{
    for (const auto s : _subscribers) {
        s->transcriptionResult(result);
    }
}

void TranscriberWrapper::transcriptionResult(std::variant<bool, std::string> transcription)
{
    notifySubscribers(transcription);
}

void TranscriberWrapper::startRecognition()
{
    // Check the speech recognition authorization status
    [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus status) {
        Transcriber *speechHelper = (__bridge Transcriber *) _helper;

        switch (status) {
        case SFSpeechRecognizerAuthorizationStatusAuthorized:
            [speechHelper start];
            break;

        case SFSpeechRecognizerAuthorizationStatusDenied:
            NSLog(@"Speech recognition permission denied.");
            break;

        case SFSpeechRecognizerAuthorizationStatusRestricted:
            NSLog(@"Speech recognition is restricted.");
            break;

        case SFSpeechRecognizerAuthorizationStatusNotDetermined:
            NSLog(@"Speech recognition permission not determined.");
            break;
        }
    }];
}

void TranscriberWrapper::stopRecognition()
{
    // Ensure the helper is initialized before stopping
    if (_helper) {
        Transcriber *speechHelper = (__bridge Transcriber *) _helper;
        [speechHelper stop];
    } else {
        NSLog(@"SpeechHelper is not initialized or has been released.");
    }
}
