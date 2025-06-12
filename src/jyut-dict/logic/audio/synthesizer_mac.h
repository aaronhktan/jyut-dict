#ifndef SYNTHESIZER_MAC_H
#define SYNTHESIZER_MAC_H

#include <string>

// The SynthesizerWrapper class provides a C++ interface to
// NSSpeechSynthesizer. Qt Speech used to have the NSSpeechSynthesizer
// backend (labelled as "macos"), but that was removed in Qt 6.9.
// In addition, it was broken in KDE Craft since 6.8. As such,
// I had to roll my own.

class SynthesizerWrapper
{
public:
    SynthesizerWrapper(void);
    ~SynthesizerWrapper(void);

    bool setLocale(std::string locale);
    void speak(std::string text);

private:
    void *_synthesizerImpl;
};

#endif // SYNTHESIZER_MAC_H
