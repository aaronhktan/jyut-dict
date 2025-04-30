// SpeechRecognizerWrapper.mm
#include "SpeechRecognizerWrapper.h"
#import "SpeechHelper.h"  // Import the Objective-C++ header
#import <Speech/Speech.h> // Import Speech framework for authorization

// Constructor: Initialize the Objective-C++ class
SpeechRecognizerWrapper::SpeechRecognizerWrapper()
{
    // Use __bridge_retained to ensure proper ownership is transferred and not autoreleased
    helper = (__bridge_retained void *) [[SpeechHelper alloc]
        init]; // Make sure helper is initialized and retains the object
}

// Destructor: Clean up (ARC handles memory management)
SpeechRecognizerWrapper::~SpeechRecognizerWrapper()
{
    // We need to ensure the object is released properly without double release
    if (helper) {
        CFRelease(helper); // Properly release the helper object when no longer needed
    }
}

// Start speech recognition by calling the Objective-C++ method
void SpeechRecognizerWrapper::startRecognition()
{
    // Check the speech recognition authorization status
    [SFSpeechRecognizer requestAuthorization:^(SFSpeechRecognizerAuthorizationStatus status) {
        // Cast the helper pointer back to SpeechHelper before accessing
        SpeechHelper *speechHelper = (__bridge SpeechHelper *)
            helper; // Always bridge the helper back

        // Handle authorization status
        switch (status) {
        case SFSpeechRecognizerAuthorizationStatusAuthorized:
            // If authorized, proceed with recognition
            [speechHelper start]; // Start speech recognition
            break;

        case SFSpeechRecognizerAuthorizationStatusDenied:
            // Permission was denied
            NSLog(@"Speech recognition permission denied.");
            break;

        case SFSpeechRecognizerAuthorizationStatusRestricted:
            // Permission is restricted
            NSLog(@"Speech recognition is restricted.");
            break;

        case SFSpeechRecognizerAuthorizationStatusNotDetermined:
            // Permission not determined yet
            NSLog(@"Speech recognition permission not determined.");
            break;
        }
    }];
}

// Stop speech recognition
void SpeechRecognizerWrapper::stopRecognition()
{
    // Ensure the helper is initialized before stopping
    if (helper) {
        SpeechHelper *speechHelper = (__bridge SpeechHelper *) helper;
        [speechHelper stop]; // Call stop method from SpeechHelper
    } else {
        // Handle the case where helper was not initialized or is nil
        NSLog(@"SpeechHelper is not initialized or has been released.");
    }
}
