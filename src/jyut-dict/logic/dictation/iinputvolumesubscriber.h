#ifndef IINPUTVOLUMESUBSCRIBER_H
#define IINPUTVOLUMESUBSCRIBER_H

#include "logic/utils/utils.h"

class IInputVolumeSubscriber
{
public:
    virtual ~IInputVolumeSubscriber() = default;
    virtual void volumeResult(Utils::Result<float> volumeLevel) = 0;
};

#endif // IINPUTVOLUMESUBSCRIBER_H
