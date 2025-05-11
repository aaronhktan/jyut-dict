#include "transcriber_windows.h"

#include <algorithm>
#include <iostream>

namespace {
// See https://learn.microsoft.com/en-us/globalization/windows-keyboard-layouts for more details/codes
constexpr auto TRADITIONAL_CHINESE_KEYBOARD_CODE = L"00000404";
constexpr auto CANTONESE_HONGKONG_KEYBOARD_CODE = L"00000C04";
constexpr auto CANTONESE_MACAO_KEYBOARD_CODE = L"00001404";

constexpr auto MANDARIN_CHINA_KEYBOARD_CODE = L"00000804";
constexpr auto MANDARIN_SINGAPORE_KEYBOARD_CODE = L"00001004";

constexpr auto ENGLISH_CANADA_MS_KEYBOARD_CODE = L"00011009";
constexpr auto ENGLISH_US_KEYBOARD_CODE = L"00000409";
constexpr auto ENGLISH_UK_KEYBOARD_CODE = L"00000809";

constexpr auto FRENCH_CANADA_KEYBOARD_CODE = L"00001009";
constexpr auto FRENCH_LEGACY_CANADA_KEYBOARD_CODE = L"00000C0C";
constexpr auto FRENCH_FRANCE_KEYBOARD_CODE = L"0000040C";
constexpr auto FRENCH_BEPO_FRANCE_KEYBOARD_CODE = L"0002040C";
constexpr auto FRENCH_LEGACY_FRANCE_KEYBOARD_CODE = L"0001040C";

bool setKeyboardLanguage(std::wstring &lang)
{
    wchar_t layoutName[9];

    HKL layout = LoadKeyboardLayout(lang.c_str(), KLF_ACTIVATE);
    if (layout == NULL) {
        std::cerr << "Failed to load keyboard layout" << std::endl;
        return false;
    }

    if (!ActivateKeyboardLayout(layout, KLF_ACTIVATE)) {
        std::cerr << "Failed to activate keyboard layout" << std::endl;
        return false;
    }

    // Sometimes ActivateKeyboardLayout will return success even
    // though the requested keyboard layout wasn't actually set.
    // Check for that here.
    GetKeyboardLayoutName(layoutName);
    if (std::wstring{layoutName} != lang) {
        return false;
    }

    return true;
}
} // namespace

Transcriber::Transcriber(std::string &locale)
{
    if (locale == "en_US" || locale == "en_UK") {
        _desiredLayoutPossibilities = {std::wstring{
                                           ENGLISH_CANADA_MS_KEYBOARD_CODE},
                                       std::wstring{ENGLISH_US_KEYBOARD_CODE},
                                       std::wstring{ENGLISH_UK_KEYBOARD_CODE}};
    } else if (locale == "fr_FR" || locale == "fr_CA") {
        _desiredLayoutPossibilities
            = {std::wstring{FRENCH_CANADA_KEYBOARD_CODE},
               std::wstring{FRENCH_LEGACY_CANADA_KEYBOARD_CODE},
               std::wstring{FRENCH_FRANCE_KEYBOARD_CODE},
               std::wstring{FRENCH_BEPO_FRANCE_KEYBOARD_CODE},
               std::wstring{FRENCH_LEGACY_FRANCE_KEYBOARD_CODE}};
    } else if (locale == "zh_HK") {
        _desiredLayoutPossibilities
            = {std::wstring{TRADITIONAL_CHINESE_KEYBOARD_CODE},
               std::wstring{CANTONESE_HONGKONG_KEYBOARD_CODE},
               std::wstring{CANTONESE_MACAO_KEYBOARD_CODE}};
    } else if (locale == "zh_CN" || locale == "zh_TW") {
        // On Windows, the Hong Kong traditional keyboard that enables
        // dictation in Cantonese is mutually exclusive with the Taiwan
        // keyboard (for unknown reasons). If the user's locale is zh_TW,
        // we try to activate the simplified Chinese keyboard (if it
        // exists) to enable dictation in Mandarin.
        _desiredLayoutPossibilities
            = {std::wstring{MANDARIN_CHINA_KEYBOARD_CODE},
               std::wstring{MANDARIN_SINGAPORE_KEYBOARD_CODE}};
    }

    _originalLayout = GetKeyboardLayout(0);
}

Transcriber::~Transcriber()
{
    ActivateKeyboardLayout(_originalLayout, KLF_ACTIVATE);
}

void Transcriber::subscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.emplace(subscriber);
}

void Transcriber::unsubscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.extract(subscriber);
}

void Transcriber::notifyTranscriptionResult(
    std::variant<std::system_error, std::string> result)
{
    for (const auto &s : _transcriptionSubscribers) {
        s->transcriptionResult(result);
    }
}

void Transcriber::startRecognition()
{
    wchar_t layoutName[9]; // The layout name will never be longer than 8 wchars
    bool layoutGood = false;

    GetKeyboardLayoutName(layoutName);

    if (std::find(_desiredLayoutPossibilities.cbegin(),
                  _desiredLayoutPossibilities.cend(),
                  std::wstring{layoutName})
        == _desiredLayoutPossibilities.end()) {
        for (const auto &i : _desiredLayoutPossibilities) {
            std::wstring layout{i};
            if (setKeyboardLanguage(layout)) {
                layoutGood = true;
                std::wcout << "Set keyboard to " << layout << std::endl;
                break;
            } else {
                std::wcerr << "Failed to set keyboard " << layout
                           << ", trying next possibility" << std::endl;
            }
        }
    } else {
        layoutGood = true;
        std::wcout << "Current keyboard matches desired keyboard " << layoutName
                   << std::endl;
    }

    if (!layoutGood) {
        notifyTranscriptionResult(
            std::system_error{ENXIO,
                              std::generic_category(),
                              "No matching keyboard layout found"});
        return;
    }

    // Press and release the Win+H key combo to activate dictation
    INPUT input[2] = {};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_LWIN;
    input[0].ki.dwFlags = 0;   // Key press
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = 'H';
    input[1].ki.dwFlags = 0; // Key press
    SendInput(2, input, sizeof(INPUT));

    input[0].ki.dwFlags = KEYEVENTF_KEYUP;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, input, sizeof(INPUT));
}

void Transcriber::stopRecognition()
{
    // There is no way to programmatically end dictation on Windows.
    std::cerr
        << "Transcriber::stopRecognition called but not implemented on Windows"
        << std::endl;
}
