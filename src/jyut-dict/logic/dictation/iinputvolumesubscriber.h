#ifndef IINPUTVOLUMESUBSCRIBER_H
#define IINPUTVOLUMESUBSCRIBER_H

#include <system_error>
#include <variant>

class IInputVolumeSubscriber
{
public:
    virtual ~IInputVolumeSubscriber() = default;
    virtual void volumeResult(std::variant<std::system_error, float> volumeLevel)
        = 0;
};

#endif // IINPUTVOLUMESUBSCRIBER_H
