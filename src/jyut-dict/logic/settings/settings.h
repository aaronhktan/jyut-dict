#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMetaType>

#include <cstdint>
#include <string>
#include <vector>

// Settings should only contain STL functions and data related to settings

namespace Settings {

extern std::vector<std::string> jyutpingToneColours;
extern std::vector<std::string> pinyinToneColours;

const extern std::vector<std::string> defaultJyutpingToneColours;
const extern std::vector<std::string> defaultPinyinToneColours;

const std::string DEFAULT_JYUTPING_TONE_0 = "grey";
const std::string DEFAULT_JYUTPING_TONE_1 = "#00bcd4";
const std::string DEFAULT_JYUTPING_TONE_2 = "#7cb342";
const std::string DEFAULT_JYUTPING_TONE_3 = "#657ff1";
const std::string DEFAULT_JYUTPING_TONE_4 = "#c2185b";
const std::string DEFAULT_JYUTPING_TONE_5 = "#068900";
const std::string DEFAULT_JYUTPING_TONE_6 = "#7651d0";

const std::string DEFAULT_PINYIN_TONE_0 = "grey";
const std::string DEFAULT_PINYIN_TONE_1 = "#fc433c";
const std::string DEFAULT_PINYIN_TONE_2 = "#02ba1f";
const std::string DEFAULT_PINYIN_TONE_3 = "#18a6f2";
const std::string DEFAULT_PINYIN_TONE_4 = "#9e77ff";
const std::string DEFAULT_PINYIN_TONE_5 = "grey";

enum class InterfaceSize : uint32_t {
    SMALLER = 1,
    SMALL,
    NORMAL,
    LARGE,
    LARGER
};

} // namespace Settings

Q_DECLARE_METATYPE(Settings::InterfaceSize);

#endif // SETTINGS_H
