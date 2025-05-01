#import "transcriber.h"

#include "itranscriptionresultpublisher.h"

#include <string>
#include <unordered_set>
#include <variant>

namespace {
constexpr auto SILENCE_THRESHOLD = 0.0001;
constexpr auto SILENCE_DURATION_S = 2.0;
} // namespace

// An Objective-C implementation cannot implement a C++ pure virtual class.
// In order to implement the C++ pure virtual class, the Objective-C implementation
// must use a pointer to a C++ object that implements the virtual class.
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
    void notifySubscribers(std::variant<std::system_error, std::string> result) override
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

    // The recognizer may be unavailable for certain locales without an Internet connection
    if (!self.recognizer.available) {
        _publisher->notifySubscribers(
            std::system_error{ENETDOWN,
                              std::generic_category(),
                              "Speech recognizer is not currently available"});
        return;
    }

    // Check authorization status
    SFSpeechRecognizerAuthorizationStatus status = [SFSpeechRecognizer authorizationStatus];
    switch (status) {
    case SFSpeechRecognizerAuthorizationStatusAuthorized: {
        break;
    }
    case SFSpeechRecognizerAuthorizationStatusDenied: {
        _publisher->notifySubscribers(
            std::system_error{EPERM,
                              std::generic_category(),
                              "Speech recognition permission not granted"});
        return;
    }
    case SFSpeechRecognizerAuthorizationStatusRestricted: {
        _publisher->notifySubscribers(
            std::system_error{EPERM,
                              std::generic_category(),
                              "Speech recognition permission is restricted"});
        return;
    }
    case SFSpeechRecognizerAuthorizationStatusNotDetermined: {
        [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus status) {
            if (status != SFSpeechRecognizerAuthorizationStatusAuthorized) {
                _publisher->notifySubscribers(
                    std::system_error{EPERM,
                                      std::generic_category(),
                                      "Speech recognition permission denied"});
            }
        }];
        break;
    }
    }

    // Create the input node for audio capture
    AVAudioInputNode *inputNode = self.audioEngine.inputNode;
    AVAudioFormat *format = [inputNode outputFormatForBus:0];

    // Install a tap on the input node to capture audio
    [inputNode removeTapOnBus:0]; // Remove any previous taps, otherwise things fail

    self.silenceStart = 0;
    self.isSilent = NO;

    [inputNode
        installTapOnBus:0
             bufferSize:1024
                 format:format
                  block:^(AVAudioPCMBuffer *buffer, AVAudioTime *when) {
                      // Append the audio buffer to the recognition request
                      [self.recognitionRequest appendAudioPCMBuffer:buffer];

                      float *data = buffer.floatChannelData[0];
                      UInt32 frameLength = buffer.frameLength;
                      float sum = 0.0;

                      for (int i = 0; i < frameLength; i++) {
                          sum += fabsf(data[i]);
                      }

                      float avg = sum / frameLength;

                      dispatch_async(dispatch_get_main_queue(), ^{
                          if (avg < SILENCE_THRESHOLD) {
                              if (!self.isSilent) {
                                  self.silenceStart = [NSDate timeIntervalSinceReferenceDate];
                                  self.isSilent = YES;
                              } else {
                                  NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
                                  if (now - self.silenceStart > SILENCE_DURATION_S) {
                                      _publisher->notifySubscribers(
                                          std::system_error{ETIMEDOUT,
                                                            std::generic_category(),
                                                            "No speech detected"});
                                  }
                              }
                          } else {
                              self.isSilent = NO;
                          }
                      });
                  }];

    // Check that we have authorization to capture audio from the inputNode
    AVAuthorizationStatus captureStatus = [AVCaptureDevice
        authorizationStatusForMediaType:AVMediaTypeAudio];

    switch (captureStatus) {
    case AVAuthorizationStatusAuthorized:
        break;

    case AVAuthorizationStatusDenied:
        _publisher->notifySubscribers(
            std::system_error{EPERM, std::generic_category(), "Microphone permission not granted"});
        return;

    case AVAuthorizationStatusRestricted:
        _publisher->notifySubscribers(
            std::system_error{EPERM, std::generic_category(), "Microphone permission restricted"});
        return;

    case AVAuthorizationStatusNotDetermined:
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio
                                 completionHandler:^(BOOL granted) {
                                     if (!granted) {
                                         _publisher->notifySubscribers(
                                             std::system_error{EPERM,
                                                               std::generic_category(),
                                                               "Microphone access denied"});
                                     }
                                 }];
        break;
    }

    // Prepare and start the audio engine
    NSError *error = nil;
    [self.audioEngine prepare];
    [self.audioEngine startAndReturnError:&error];

    if (error) {
        _publisher->notifySubscribers(
            std::system_error{ENOENT, std::generic_category(), "Audio engine failed to start"});
        return;
    }

    // Start the recognition task
    self.recognitionTask = [self.recognizer
        recognitionTaskWithRequest:self.recognitionRequest
                     resultHandler:^(SFSpeechRecognitionResult *result, NSError *error) {
                         if (result && !self.recognitionTask.finishing) {
                             std::string resultStr{
                                 [result.bestTranscription.formattedString UTF8String]};
                             if (!resultStr.empty()) {
                                 _publisher->notifySubscribers(resultStr);
                             }
                         }
                         if (error) {
                             switch (error.code) {
                             case 301: {
                                 // Transcription canceled; do nothing
                                 break;
                             }
                             case 1110: {
                                 // No speech detected; this is benign
                                 _publisher->notifySubscribers(
                                     std::system_error{ETIMEDOUT,
                                                       std::generic_category(),
                                                       "No speech detected"});
                                 break;
                             }
                             default: {
                                 _publisher->notifySubscribers(std::system_error{
                                     EINVAL,
                                     std::generic_category(),
                                     "Could not transcribe input with error code: "
                                         + std::string{[[NSString
                                             stringWithFormat:@"%ld",
                                                              (long) error.code] UTF8String]}});
                                 break;
                             }
                             }
                         }
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
