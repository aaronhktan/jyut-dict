#ifndef TRANSCRIBER_H
#define TRANSCRIBER_H

#include "itranscriptionresultsubscriber.h"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#import <Speech/Speech.h>

// The Transcriber interface is a wrapper to start and stop
// speech-to-text recognition on macOS.

@interface Transcriber : NSObject

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

#endif // TRANSCRIBER_H
