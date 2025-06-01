#ifndef IINPUTVOLUMEPUBLISHER_H
#define IINPUTVOLUMEPUBLISHER_H

#include "iinputvolumesubscriber.h"

#include "logic/utils/utils.h"

class IInputVolumePublisher
{
public:
    virtual ~IInputVolumePublisher() = default;

    virtual void subscribe(IInputVolumeSubscriber *subscriber) = 0;
    virtual void unsubscribe(IInputVolumeSubscriber *subscriber) = 0;
    virtual void notifyVolumeResult(Utils::Result<float>) = 0;
};

#endif // IINPUTVOLUMEPUBLISHER_H
