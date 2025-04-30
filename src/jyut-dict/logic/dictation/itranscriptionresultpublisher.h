#ifndef ITRANSCRIPTIONRESULTPUBLISHER_H
#define ITRANSCRIPTIONRESULTPUBLISHER_H

#include "itranscriptionresultsubscriber.h"

#include <string>
#include <variant>

class ITranscriptionResultPublisher
{
public:
    virtual ~ITranscriptionResultPublisher() = default;

    virtual void subscribe(ITranscriptionResultSubscriber *subscriber) = 0;
    virtual void unsubscribe(ITranscriptionResultSubscriber *subscriber) = 0;
    virtual void notifySubscribers(std::variant<bool, std::string>) = 0;
};

#endif // ITRANSCRIPTIONRESULTPUBLISHER_H
