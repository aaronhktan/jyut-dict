#import "transcriber_mac.h"

#include "iinputvolumepublisher.h"
#include "itranscriptionresultpublisher.h"

#include <string>
#include <unordered_set>
#include <variant>

namespace {
constexpr auto SILENCE_THRESHOLD = 0.005;
constexpr auto FIRST_SILENCE_DURATION_S = 5.0;
constexpr auto SILENCE_DURATION_S = 3.0;
} // namespace

// An Objective-C implementation cannot implement a C++ pure virtual class.
// In order to implement the C++ pure virtual class, the Objective-C implementation
// must use a pointer to a C++ object that implements the virtual class.
class TranscriptionResultPublisherImpl : public IInputVolumePublisher,
                                         public ITranscriptionResultPublisher
{
public:
    void subscribe(IInputVolumeSubscriber *subscriber) override
    {
        _volumeSubscribers.emplace(subscriber);
    }
    void unsubscribe(IInputVolumeSubscriber *subscriber) override
    {
        _volumeSubscribers.extract(subscriber);
    }
    void notifyVolumeResult(Utils::Result<float> result) override
    {
        for (const auto &s : _volumeSubscribers) {
            s->volumeResult(result);
        }
    }

    void subscribe(ITranscriptionResultSubscriber *subscriber) override
    {
        _transcriptionSubscribers.emplace(subscriber);
    }
    void unsubscribe(ITranscriptionResultSubscriber *subscriber) override
    {
        _transcriptionSubscribers.extract(subscriber);
    }
    void notifyTranscriptionResult(Utils::Result<std::string> result) override
    {
        for (const auto &s : _transcriptionSubscribers) {
            s->transcriptionResult(result);
        }
    }

private:
    std::unordered_set<IInputVolumeSubscriber *> _volumeSubscribers;
    std::unordered_set<ITranscriptionResultSubscriber *> _transcriptionSubscribers;
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

- (void)subscribeForVolume:(IInputVolumeSubscriber *)subscriber
{
    _publisher->subscribe(static_cast<IInputVolumeSubscriber *>(subscriber));
}

- (void)unsubscribeForVolume:(IInputVolumeSubscriber *)subscriber
{
    _publisher->unsubscribe(static_cast<IInputVolumeSubscriber *>(subscriber));
}

- (void)subscribeForTranscript:(ITranscriptionResultSubscriber *)subscriber
{
    _publisher->subscribe(static_cast<ITranscriptionResultSubscriber *>(subscriber));
}

- (void)unsubscribeForTranscript:(ITranscriptionResultSubscriber *)subscriber
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
        _publisher->notifyTranscriptionResult(std::system_error{
            ENETDOWN,
            std::generic_category(),
            "Speech recognizer is not currently available; may require Internet access"});
        return;
    }

    // Check authorization status
    SFSpeechRecognizerAuthorizationStatus status = [SFSpeechRecognizer authorizationStatus];
    dispatch_semaphore_t speech_recognizer_sema = dispatch_semaphore_create(0);

    switch (status) {
    case SFSpeechRecognizerAuthorizationStatusAuthorized: {
        dispatch_semaphore_signal(speech_recognizer_sema);
        break;
    }
    case SFSpeechRecognizerAuthorizationStatusDenied: {
        dispatch_semaphore_signal(speech_recognizer_sema);
        _publisher->notifyTranscriptionResult(
            std::system_error{EPERM,
                              std::generic_category(),
                              "Speech recognition permission not granted"});
        break;
    }
    case SFSpeechRecognizerAuthorizationStatusRestricted: {
        dispatch_semaphore_signal(speech_recognizer_sema);
        _publisher->notifyTranscriptionResult(
            std::system_error{EPERM,
                              std::generic_category(),
                              "Speech recognition permission is restricted"});
        break;
    }
    case SFSpeechRecognizerAuthorizationStatusNotDetermined: {
        [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus status) {
            dispatch_semaphore_signal(speech_recognizer_sema);
            if (status != SFSpeechRecognizerAuthorizationStatusAuthorized) {
                _publisher->notifyTranscriptionResult(
                    std::system_error{EPERM,
                                      std::generic_category(),
                                      "Speech recognition permission denied"});
            }
        }];
        break;
    }
    }

    while (dispatch_semaphore_wait(speech_recognizer_sema, DISPATCH_TIME_NOW)) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
                                 beforeDate:[NSDate dateWithTimeIntervalSinceNow:0]];
    }
    status = [SFSpeechRecognizer authorizationStatus];
    if (status != SFSpeechRecognizerAuthorizationStatusAuthorized) {
        return;
    }

    // Create the input node for audio capture
    AVAudioInputNode *inputNode = self.audioEngine.inputNode;
    AVAudioFormat *format = [inputNode outputFormatForBus:0];

    // Install a tap on the input node to capture audio
    [inputNode removeTapOnBus:0]; // Remove any previous taps, otherwise things fail

    self.silenceStart = 0;
    self.isSilent = NO;
    self.firstSilence = YES;

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
                      _publisher->notifyVolumeResult(static_cast<float>(avg / SILENCE_THRESHOLD));

                      dispatch_async(dispatch_get_main_queue(), ^{
                          if (avg < SILENCE_THRESHOLD) {
                              if (!self.isSilent) {
                                  self.silenceStart = [NSDate timeIntervalSinceReferenceDate];
                                  self.isSilent = YES;
                              } else {
                                  NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
                                  if (self.firstSilence
                                      && (now - self.silenceStart > FIRST_SILENCE_DURATION_S)) {
                                      _publisher->notifyTranscriptionResult(
                                          std::system_error{ETIMEDOUT,
                                                            std::generic_category(),
                                                            "No speech detected"});
                                  } else if (!self.firstSilence
                                             && (now - self.silenceStart > SILENCE_DURATION_S)) {
                                      _publisher->notifyTranscriptionResult(
                                          std::system_error{ETIMEDOUT,
                                                            std::generic_category(),
                                                            "No speech detected"});
                                  }
                              }
                          } else {
                              self.isSilent = NO;
                              self.firstSilence = NO;
                          }
                      });
                  }];

    // Check that we have authorization to capture audio from the inputNode
    AVAuthorizationStatus captureStatus = [AVCaptureDevice
        authorizationStatusForMediaType:AVMediaTypeAudio];
    dispatch_semaphore_t capture_sema = dispatch_semaphore_create(0);

    switch (captureStatus) {
    case AVAuthorizationStatusAuthorized:
        dispatch_semaphore_signal(capture_sema);
        break;

    case AVAuthorizationStatusDenied:
        dispatch_semaphore_signal(capture_sema);
        _publisher->notifyTranscriptionResult(
            std::system_error{EPERM, std::generic_category(), "Microphone permission not granted"});
        break;

    case AVAuthorizationStatusRestricted:
        dispatch_semaphore_signal(capture_sema);
        _publisher->notifyTranscriptionResult(
            std::system_error{EPERM, std::generic_category(), "Microphone permission restricted"});
        break;

    case AVAuthorizationStatusNotDetermined:
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio
                                 completionHandler:^(BOOL granted) {
                                     dispatch_semaphore_signal(capture_sema);
                                     if (!granted) {
                                         _publisher->notifyTranscriptionResult(
                                             std::system_error{EPERM,
                                                               std::generic_category(),
                                                               "Microphone access denied"});
                                     }
                                 }];
        break;
    }

    while (dispatch_semaphore_wait(capture_sema, DISPATCH_TIME_NOW)) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
                                 beforeDate:[NSDate dateWithTimeIntervalSinceNow:0]];
    }
    captureStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    if (captureStatus != AVAuthorizationStatusAuthorized) {
        return;
    }

    // Prepare and start the audio engine
    NSError *error = nil;
    [self.audioEngine prepare];
    [self.audioEngine startAndReturnError:&error];

    if (error) {
        _publisher->notifyTranscriptionResult(
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
                                 _publisher->notifyTranscriptionResult(resultStr);
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
                                 _publisher->notifyTranscriptionResult(
                                     std::system_error{ETIMEDOUT,
                                                       std::generic_category(),
                                                       "No speech detected"});
                                 break;
                             }
                             default: {
                                 _publisher->notifyTranscriptionResult(std::system_error{
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
