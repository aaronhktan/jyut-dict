#include "utils.h"

#include <QColor>

namespace Utils {
    QColor getContrastingColour(const QColor backgroundColour)
    {
        auto brightness = backgroundColour.redF() * 0.299
                          + backgroundColour.greenF() * 0.587
                          + backgroundColour.blueF() * 0.114;
        return (brightness > 0.65) ? QColor{0, 0, 0} : QColor{0xFF, 0xFF, 0xFF};
    }
}
