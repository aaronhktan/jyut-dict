#ifndef SYNTHESIZER_MAC_H
#define SYNTHESIZER_MAC_H

#include <string>

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
