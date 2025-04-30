// SpeechHelper.mm
#import "speechhelper.h"

@implementation SpeechHelper

- (instancetype)init
{
    self = [super init];
    if (self) {
        // Initialize the speech recognizer
        _recognizer = [[SFSpeechRecognizer alloc]
            initWithLocale:[NSLocale localeWithLocaleIdentifier:@"zh-HK"]];
        _audioEngine = [[AVAudioEngine alloc] init];
        _recognitionRequest = [[SFSpeechAudioBufferRecognitionRequest alloc] init];
    }
    return self;
}

- (void)start
{
    // Request authorization for speech recognition
    [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus status) {
        if (status != SFSpeechRecognizerAuthorizationStatusAuthorized) {
            NSLog(@"Speech recognition not authorized");
            return;
        }

        // Create the input node for audio capture
        AVAudioInputNode *inputNode = self.audioEngine.inputNode;
        AVAudioFormat *format = [inputNode outputFormatForBus:0];

        // Install a tap on the input node to capture audio
        [inputNode installTapOnBus:0
                        bufferSize:1024
                            format:format
                             block:^(AVAudioPCMBuffer *buffer, AVAudioTime *when) {
                                 // Append the audio buffer to the recognition request
                                 [self.recognitionRequest appendAudioPCMBuffer:buffer];
                             }];

        AVAuthorizationStatus captureStatus = [AVCaptureDevice
            authorizationStatusForMediaType:AVMediaTypeAudio];

        switch (captureStatus) {
        case AVAuthorizationStatusAuthorized:
            // Access has been granted.
            NSLog(@"Microphone access granted.");
            break;

        case AVAuthorizationStatusDenied:
            // Access has been denied.
            NSLog(@"Microphone access denied.");
            break;

        case AVAuthorizationStatusRestricted:
            // Access is restricted (e.g., parental controls).
            NSLog(@"Microphone access restricted.");
            break;

        case AVAuthorizationStatusNotDetermined:
            // The user hasn't been prompted yet.
            [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio
                                     completionHandler:^(BOOL granted) {
                                         if (granted) {
                                             NSLog(@"Microphone access granted after request.");
                                         } else {
                                             NSLog(@"Microphone access denied after request.");
                                         }
                                     }];
            break;
        }

        // Prepare and start the audio engine
        NSError *error = nil;
        [self.audioEngine prepare];
        [self.audioEngine startAndReturnError:&error];

        if (error) {
            NSLog(@"Failed to start audio engine: %@", error);
            return;
        }

        // Start the recognition task
        self.recognitionTask = [self.recognizer
            recognitionTaskWithRequest:self.recognitionRequest
                         resultHandler:^(SFSpeechRecognitionResult *result, NSError *error) {
                             if (result) {
                                 // Handle the transcription result
                                 NSLog(@"Transcription: %@",
                                       result.bestTranscription.formattedString);
                             }
                             if (error) {
                                 // Handle any errors
                                 NSLog(@"Recognition error: %@", error);
                             }
                         }];
    }];
}

- (void)stop
{
    // Stop the audio engine and cancel the recognition task
    [_recognitionRequest endAudio];
    [_audioEngine stop];
    [_recognitionTask cancel];

    AVAudioInputNode *inputNode = self.audioEngine.inputNode;
    [inputNode removeTapOnBus:0];

    // _audioEngine = nil; // Release and reset the engine
    // _audioEngine = [[AVAudioEngine alloc] init];
}

@end
