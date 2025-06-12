#include "synthesizer_mac.h"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#import <Speech/Speech.h>

#include <iostream>

@interface Synthesizer : NSObject <AVSpeechSynthesizerDelegate>

@property(nonatomic, strong) AVSpeechSynthesizer *synthesizer;
@property(nonatomic, strong) AVSpeechSynthesisVoice *voice;

- (bool)setLocale:(NSString *)locale;
- (void)speak:(NSString *)text;

@end

@implementation Synthesizer

- (instancetype)init
{
    self = [super init];
    if (self) {
        _synthesizer = [[AVSpeechSynthesizer alloc] init];
    }
    return self;
}

- (bool)setLocale:(NSString *)locale
{
    _voice = [AVSpeechSynthesisVoice voiceWithLanguage:locale];
    NSLog(@"%@ %@ %@", locale, _voice.name, _voice.identifier);
    return !(_voice == nil);
}

- (void)speak:(NSString *)text
{
    AVSpeechUtterance *utterance = [AVSpeechUtterance speechUtteranceWithString:text];
    utterance.rate = AVSpeechUtteranceDefaultSpeechRate * 0.75;
    utterance.voice = _voice;
    NSLog(@"Speaking!");

    _synthesizer.delegate = self;
    [_synthesizer speakUtterance:utterance];
}

@end

SynthesizerWrapper::SynthesizerWrapper()
{
    _synthesizerImpl = (__bridge_retained void *) [[Synthesizer alloc] init];
}

SynthesizerWrapper::~SynthesizerWrapper()
{
    std::cout << "destroying" << std::endl;
    if (_synthesizerImpl) {
        CFRelease(_synthesizerImpl);
    }
}

bool SynthesizerWrapper::setLocale(std::string locale)
{
    Synthesizer *synthesizer = (__bridge Synthesizer *) _synthesizerImpl;
    return [synthesizer setLocale:[NSString stringWithUTF8String:locale.c_str()]];
}

void SynthesizerWrapper::speak(std::string text)
{
    Synthesizer *synthesizer = (__bridge Synthesizer *) _synthesizerImpl;
    [synthesizer speak:[NSString stringWithUTF8String:text.c_str()]];
}
