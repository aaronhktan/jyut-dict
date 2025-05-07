#ifndef ITRANSCRIPTIONRESULTPUBLISHER_H
#define ITRANSCRIPTIONRESULTPUBLISHER_H

#include "itranscriptionresultsubscriber.h"

#include <string>
#include <system_error>
#include <variant>

class ITranscriptionResultPublisher
{
public:
    virtual ~ITranscriptionResultPublisher() = default;

    virtual void subscribe(ITranscriptionResultSubscriber *subscriber) = 0;
    virtual void unsubscribe(ITranscriptionResultSubscriber *subscriber) = 0;
    virtual void notifyTranscriptionResult(
        std::variant<std::system_error, std::string>)
        = 0;
};

#endif // ITRANSCRIPTIONRESULTPUBLISHER_H
