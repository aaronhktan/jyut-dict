#ifndef IINPUTVOLUMEPUBLISHER_H
#define IINPUTVOLUMEPUBLISHER_H

#include "iinputvolumesubscriber.h"

#include <system_error>
#include <variant>

class IInputVolumePublisher
{
public:
    virtual ~IInputVolumePublisher() = default;

    virtual void subscribe(IInputVolumeSubscriber *subscriber) = 0;
    virtual void unsubscribe(IInputVolumeSubscriber *subscriber) = 0;
    virtual void notifyVolumeResult(std::variant<std::system_error, float>) = 0;
};

#endif // IINPUTVOLUMEPUBLISHER_H
