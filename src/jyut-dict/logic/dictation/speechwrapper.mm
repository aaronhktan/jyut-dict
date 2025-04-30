#include "speechwrapper.h"

#import "speech_mac.h"

#include <QDebug>

#import <Speech/Speech.h>

SpeechWrapper::SpeechWrapper(std::string &locale)
{
    // Use __bridge_retained to ensure proper ownership is transferred and not autoreleased
    _helper = (__bridge_retained void *) [[SpeechHelper alloc]
        initWithLocaleIdentifier:[NSString stringWithUTF8String:locale.c_str()]];

    SpeechHelper *speechHelper = (__bridge SpeechHelper *) _helper;
    [speechHelper subscribe:this];
}

SpeechWrapper::~SpeechWrapper()
{
    if (_helper) {
        CFRelease(_helper);
    }
}

void SpeechWrapper::subscribe(ITranscriptionResultSubscriber *subscriber)
{
    _subscribers.emplace(subscriber);
}

void SpeechWrapper::unsubscribe(ITranscriptionResultSubscriber *subscriber)
{
    _subscribers.extract(subscriber);
}

void SpeechWrapper::notifySubscribers(std::variant<bool, std::string> result)
{
    for (const auto s : _subscribers) {
        s->transcriptionResult(result);
    }
}

void SpeechWrapper::transcriptionResult(std::variant<bool, std::string> transcription)
{
    notifySubscribers(transcription);
}

void SpeechWrapper::startRecognition()
{
    // Check the speech recognition authorization status
    [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus status) {
        SpeechHelper *speechHelper = (__bridge SpeechHelper *)
            _helper; // Always bridge the helper back

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

void SpeechWrapper::stopRecognition()
{
    // Ensure the helper is initialized before stopping
    if (_helper) {
        SpeechHelper *speechHelper = (__bridge SpeechHelper *) _helper;
        [speechHelper stop];
    } else {
        NSLog(@"SpeechHelper is not initialized or has been released.");
    }
}
