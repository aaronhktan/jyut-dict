#ifndef ITRANSCRIPTIONRESULTPUBLISHER_H
#define ITRANSCRIPTIONRESULTPUBLISHER_H

#include "itranscriptionresultsubscriber.h"

#include "logic/utils/utils.h"

#include <string>

class ITranscriptionResultPublisher
{
public:
    virtual ~ITranscriptionResultPublisher() = default;

    virtual void subscribe(ITranscriptionResultSubscriber *) = 0;
    virtual void unsubscribe(ITranscriptionResultSubscriber *) = 0;
    virtual void notifyTranscriptionResult(Utils::Result<std::string>) = 0;
};

#endif // ITRANSCRIPTIONRESULTPUBLISHER_H
