#ifndef TRANSCRIBER_MAC_H
#define TRANSCRIBER_MAC_H

#include "iinputvolumepublisher.h"
#include "itranscriptionresultpublisher.h"

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
@property(nonatomic, assign) NSTimeInterval silenceStart;
@property(nonatomic, assign) BOOL firstSilence;
@property(nonatomic, assign) BOOL isSilent;

- (instancetype)initWithLocaleIdentifier:(NSString *)localeIdentifier;

- (void)subscribeForVolume:(IInputVolumeSubscriber *)subscriber;
- (void)unsubscribeForVolume:(IInputVolumeSubscriber *)subscriber;

- (void)subscribeForTranscript:(ITranscriptionResultSubscriber *)subscriber;
- (void)unsubscribeForTranscript:(ITranscriptionResultSubscriber *)subscriber;

- (void)start;
- (void)stop;

@end

#endif // TRANSCRIBER_MAC_H
