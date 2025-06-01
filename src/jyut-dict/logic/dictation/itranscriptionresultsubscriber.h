#ifndef ITRANSCRIPTIONRESULTSUBSCRIBER_H
#define ITRANSCRIPTIONRESULTSUBSCRIBER_H

#include "logic/utils/utils.h"

#include <string>

// When informed of new transcription results, transcriptionResult()
// is called

class ITranscriptionResultSubscriber
{
public:
    virtual ~ITranscriptionResultSubscriber() = default;
    virtual void transcriptionResult(Utils::Result<std::string>) = 0;
};

#endif // ITRANSCRIPTIONRESULTSUBSCRIBER_H
