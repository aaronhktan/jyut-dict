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

constexpr auto DEFAULT_JYUTPING_TONE_0 = "grey";
constexpr auto DEFAULT_JYUTPING_TONE_1 = "#00bcd4";
constexpr auto DEFAULT_JYUTPING_TONE_2 = "#7cb342";
constexpr auto DEFAULT_JYUTPING_TONE_3 = "#657ff1";
constexpr auto DEFAULT_JYUTPING_TONE_4 = "#c2185b";
constexpr auto DEFAULT_JYUTPING_TONE_5 = "#068900";
constexpr auto DEFAULT_JYUTPING_TONE_6 = "#7651d0";

constexpr auto DEFAULT_PINYIN_TONE_0 = "grey";
constexpr auto DEFAULT_PINYIN_TONE_1 = "#fc433c";
constexpr auto DEFAULT_PINYIN_TONE_2 = "#02ba1f";
constexpr auto DEFAULT_PINYIN_TONE_3 = "#18a6f2";
constexpr auto DEFAULT_PINYIN_TONE_4 = "#9e77ff";
constexpr auto DEFAULT_PINYIN_TONE_5 = "grey";

enum class InterfaceSize : uint32_t {
    SMALLER = 1,
    SMALL,
    NORMAL,
    LARGE,
    LARGER
};

const extern std::vector<int> bodyFontSize;
const extern std::vector<int> bodyFontSizeHan;
const extern std::vector<int> h4FontSize;

} // namespace Settings

Q_DECLARE_METATYPE(Settings::InterfaceSize);

#endif // SETTINGS_H
