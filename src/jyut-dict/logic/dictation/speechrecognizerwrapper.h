// SpeechRecognizerWrapper.h (C++)

#ifndef SPEECH_RECOGNIZER_WRAPPER_H
#define SPEECH_RECOGNIZER_WRAPPER_H

class SpeechRecognizerWrapper
{
public:
    SpeechRecognizerWrapper();
    ~SpeechRecognizerWrapper();

    void startRecognition();
    void stopRecognition();

private:
    void *helper; // Pointer to the Objective-C++ class
};

#endif
