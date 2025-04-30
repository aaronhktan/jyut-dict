#import "transcriber.h"

#include "itranscriptionresultpublisher.h"

#include <string>
#include <unordered_set>
#include <variant>

class TranscriptionResultPublisherImpl : public ITranscriptionResultPublisher
{
public:
    void subscribe(ITranscriptionResultSubscriber *subscriber) override
    {
        _subscribers.emplace(subscriber);
    }
    void unsubscribe(ITranscriptionResultSubscriber *subscriber) override
    {
        _subscribers.extract(subscriber);
    }
    void notifySubscribers(std::variant<bool, std::string> result) override
    {
        for (const auto &s : _subscribers) {
            s->transcriptionResult(result);
        }
    }

private:
    std::unordered_set<ITranscriptionResultSubscriber *> _subscribers;
};

@implementation Transcriber {
    TranscriptionResultPublisherImpl *_publisher;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _recognizer = [[SFSpeechRecognizer alloc]
            initWithLocale:[NSLocale localeWithLocaleIdentifier:@"zh-HK"]];
        _audioEngine = [[AVAudioEngine alloc] init];
    }
    _publisher = new TranscriptionResultPublisherImpl;
    return self;
}

- (instancetype)initWithLocaleIdentifier:(NSString *)localeIdentifier
{
    self = [super init];
    if (self) {
        _recognizer = [[SFSpeechRecognizer alloc]
            initWithLocale:[NSLocale localeWithLocaleIdentifier:localeIdentifier]];
        _audioEngine = [[AVAudioEngine alloc] init];
    }
    _publisher = new TranscriptionResultPublisherImpl;
    return self;
}

- (void)dealloc
{
    delete _publisher;
}

- (void)subscribe:(ITranscriptionResultSubscriber *)subscriber
{
    _publisher->subscribe(static_cast<ITranscriptionResultSubscriber *>(subscriber));
}

- (void)unsubscribe:(ITranscriptionResultSubscriber *)subscriber
{
    _publisher->unsubscribe(static_cast<ITranscriptionResultSubscriber *>(subscriber));
}

- (void)start
{
    // SFSpeechAudioBufferRecognitionRequest cannot be re-used, so we have to create
    // a new one every time we want to start speech recognition
    _recognitionRequest = [[SFSpeechAudioBufferRecognitionRequest alloc] init];

    [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus status) {
        if (status != SFSpeechRecognizerAuthorizationStatusAuthorized) {
            NSLog(@"Speech recognition not authorized");
            _publisher->notifySubscribers(false);
            return;
        }

        // Create the input node for audio capture
        AVAudioInputNode *inputNode = self.audioEngine.inputNode;
        AVAudioFormat *format = [inputNode outputFormatForBus:0];

        // Install a tap on the input node to capture audio
        [inputNode removeTapOnBus:0]; // Remove any previous taps, otherwise things fail
        [inputNode installTapOnBus:0
                        bufferSize:1024
                            format:format
                             block:^(AVAudioPCMBuffer *buffer, AVAudioTime *when) {
                                 // Append the audio buffer to the recognition request
                                 [self.recognitionRequest appendAudioPCMBuffer:buffer];
                             }];

        // Check that we have authorization to capture audio from the inputNode
        AVAuthorizationStatus captureStatus = [AVCaptureDevice
            authorizationStatusForMediaType:AVMediaTypeAudio];

        switch (captureStatus) {
        case AVAuthorizationStatusAuthorized:
            NSLog(@"Microphone access granted.");
            break;

        case AVAuthorizationStatusDenied:
            NSLog(@"Microphone access denied.");
            _publisher->notifySubscribers(false);
            return;

        case AVAuthorizationStatusRestricted:
            NSLog(@"Microphone access restricted.");
            _publisher->notifySubscribers(false);
            return;

        case AVAuthorizationStatusNotDetermined:
            [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio
                                     completionHandler:^(BOOL granted) {
                                         if (granted) {
                                             NSLog(@"Microphone access granted after request.");
                                         } else {
                                             NSLog(@"Microphone access denied after request.");
                                             _publisher->notifySubscribers(false);
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
            _publisher->notifySubscribers(false);
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
                                 _publisher->notifySubscribers(std::string{
                                     [result.bestTranscription.formattedString UTF8String]});
                             }
                             if (error) {
                                 // Handle any errors
                                 NSLog(@"Recognition error: %@", error);
                                 _publisher->notifySubscribers(false);
                             }
                         }];
    }];
}

- (void)stop
{
    // Stop the audio engine and end the recognition task
    [_recognitionRequest endAudio];
    [_audioEngine stop];
    [_recognitionTask finish];

    // Remove the tap on the input node, so that the next time
    // start() is called, speech recognition can begin
    AVAudioInputNode *inputNode = self.audioEngine.inputNode;
    [inputNode removeTapOnBus:0];
}

@end
