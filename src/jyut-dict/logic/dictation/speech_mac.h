#ifndef SPEECH_MAC_H
#define SPEECH_MAC_H

#include "itranscriptionresultsubscriber.h"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#import <Speech/Speech.h>

// The SpeechHelper interface is a wrapper to start and stop
// speech-to-text recognition on macOS.

@interface SpeechHelper : NSObject

@property(nonatomic, strong) SFSpeechRecognizer *recognizer;
@property(nonatomic, strong) AVAudioEngine *audioEngine;
@property(nonatomic, strong)
    SFSpeechAudioBufferRecognitionRequest *recognitionRequest;
@property(nonatomic, strong) SFSpeechRecognitionTask *recognitionTask;

- (instancetype)initWithLocaleIdentifier:(NSString *)localeIdentifier;

- (void)subscribe:(ITranscriptionResultSubscriber *)subscriber;
- (void)unsubscribe:(ITranscriptionResultSubscriber *)subscriber;

- (void)start;
- (void)stop;

@end

#endif // SPEECH_MAC_H
