#ifndef ITRANSCRIPTIONRESULTSUBSCRIBER_H
#define ITRANSCRIPTIONRESULTSUBSCRIBER_H

#include <string>
#include <system_error>
#include <variant>

// When informed of new transcription results, transcriptionResult()
// is called

class ITranscriptionResultSubscriber
{
public:
    virtual ~ITranscriptionResultSubscriber() = default;
    virtual void transcriptionResult(
        std::variant<std::system_error, std::string> transcription)
        = 0;
};

#endif // ITRANSCRIPTIONRESULTSUBSCRIBER_H
