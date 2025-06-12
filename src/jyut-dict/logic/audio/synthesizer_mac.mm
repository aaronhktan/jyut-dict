#include "synthesizer_mac.h"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include <iostream>

@interface Synthesizer : NSObject <NSSpeechSynthesizerDelegate>

@property(nonatomic, strong) NSSpeechSynthesizer *synthesizer;

- (bool)setLocale:(NSString *)locale;
- (void)speak:(NSString *)text;

@end

@implementation Synthesizer

- (instancetype)init
{
    self = [super init];

    // NSSpeechSynthesizer init requires having a voice, otherwise it silently fails
    NSArray *voices = [NSSpeechSynthesizer availableVoices];
    if (voices.count > 0) {
        _synthesizer = [[NSSpeechSynthesizer alloc] initWithVoice:voices[0]];
    }
    if (!_synthesizer) {
        NSLog(@"[ERROR] Failed to allocate NSSpeechSynthesizer");
    }
    return self;
}

- (bool)setLocale:(NSString *)locale
{
    NSArray<NSString *> *filteredVoices = [[NSSpeechSynthesizer availableVoices]
        filteredArrayUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(id object,
                                                                          NSDictionary *bindings) {
            if ([locale
                    caseInsensitiveCompare:[NSSpeechSynthesizer
                                               attributesForVoice:object][@"VoiceLocaleIdentifier"]]
                != NSOrderedSame) {
                return NO;
            }

            if ([[NSSpeechSynthesizer attributesForVoice:object][@"VoiceIdentifier"]
                    containsString:@"eloquence"]) {
                // Eloquence voices sound like garbage
                return NO;
            }

            return YES;
        }]];

    if (![filteredVoices count]) {
        NSLog(@"No voices found");
        return false; // No voices were found for this locale
    }

    return [_synthesizer setVoice:filteredVoices[0]];
}

- (void)speak:(NSString *)text
{
    _synthesizer.delegate = self;
    [_synthesizer setRate:_synthesizer.rate * 0.75];
    [_synthesizer startSpeakingString:text];
}

@end

SynthesizerWrapper::SynthesizerWrapper()
{
    _synthesizerImpl = (__bridge_retained void *) [[Synthesizer alloc] init];
}

SynthesizerWrapper::~SynthesizerWrapper()
{
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
