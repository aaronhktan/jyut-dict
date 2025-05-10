#include "transcriber_windows.h"

#include <QDebug>

#include <iostream>

namespace {
// See https://learn.microsoft.com/en-us/globalization/windows-keyboard-layouts for more details
constexpr auto CANTONESE_HONGKONG_KEYBOARD_CODE = L"00000C04";
constexpr auto CANTONESE_MACAO_KEYBOARD_CODE = L"00001404";
constexpr auto TRADITIONAL_CHINESE_KEYBOARD_CODE = L"00000404";

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

constexpr auto MAX_RETRIES = 2;
} // namespace

std::string wcharArrayToString(const wchar_t* wideArray) {
    return std::string(wideArray, wideArray + wcslen(wideArray));
}

Transcriber::Transcriber(std::string &locale)
{
    std::cout << locale << std::endl;
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
            = {std::wstring{CANTONESE_HONGKONG_KEYBOARD_CODE},
               std::wstring{CANTONESE_MACAO_KEYBOARD_CODE},
               std::wstring{TRADITIONAL_CHINESE_KEYBOARD_CODE}};
    } else if (locale == "zh_CN") {
        _desiredLayoutPossibilities
            = {std::wstring{MANDARIN_CHINA_KEYBOARD_CODE},
               std::wstring{MANDARIN_SINGAPORE_KEYBOARD_CODE}};
    } else if (locale == "zh_TW") {
        //        _desiredLayoutPossibilities = {
        //            std::wstring{MANDARIN_TAIWAN_KEYBOARD_CODE}};
    }
}

Transcriber::~Transcriber()
{
}

void Transcriber::subscribe(IInputVolumeSubscriber *subscriber)
{
    _volumeSubscribers.emplace(subscriber);
}

void Transcriber::unsubscribe(IInputVolumeSubscriber *subscriber)
{
    _volumeSubscribers.extract(subscriber);
}

void Transcriber::notifyVolumeResult(std::variant<std::system_error, float> result)
{
    for (const auto s : _volumeSubscribers) {
        s->volumeResult(result);
    }
}

void Transcriber::volumeResult(std::variant<std::system_error, float> result)
{
    notifyVolumeResult(result);
}

void Transcriber::subscribe(ITranscriptionResultSubscriber *subscriber)
{
    // Do nothing
}

void Transcriber::unsubscribe(ITranscriptionResultSubscriber *subscriber)
{
    // Do nothing
}

void Transcriber::notifyTranscriptionResult(
    std::variant<std::system_error, std::string> result)
{
    // Do nothing
}

void Transcriber::transcriptionResult(
    std::variant<std::system_error, std::string> transcription)
{
    // Do nothing
}

bool setKeyboardLanguage(std::wstring &i)
{
    int retries = 0;
    wchar_t layoutName[9];

    while (retries < MAX_RETRIES) {
        HKL layout = LoadKeyboardLayout(i.c_str(), KLF_ACTIVATE);
        if (layout == NULL) {
            std::cerr << "Failed to load keyboard layout" << std::endl;
            ++retries;
            continue;
        }

        if (!ActivateKeyboardLayout(layout, KLF_ACTIVATE)) {
            std::cerr << "Failed to activate keyboard layout" << std::endl;
            ++retries;
            continue;
        }

        GetKeyboardLayoutName(layoutName);
        std::wcout << "set keyboard layout " << layoutName << std::endl;
        std::wcout << "desired keyboard layout " << i << std::endl;
        std::wcout << (layoutName == i) << std::endl;
        if (std::wstring{layoutName} != i) {
            ++retries;
            continue;
        } else {
            break;
        }
    }

    qDebug() << "num retries:" << retries;
    if (retries == (MAX_RETRIES)) {
        return false;
    }
    return true;
}

void Transcriber::startRecognition()
{
    qDebug() << "hello";
    wchar_t layoutName[9]; // The layout name will never be longer than 8 wchars

    _originalLayout = GetKeyboardLayout(0);
    GetKeyboardLayoutName(layoutName);

    if (std::find(_desiredLayoutPossibilities.cbegin(),
                  _desiredLayoutPossibilities.cend(),
                  std::wstring{layoutName})
        == _desiredLayoutPossibilities.end()) {
        for (const auto &i : _desiredLayoutPossibilities) {
            std::wstring layout{i};
            if (setKeyboardLanguage(layout)) {
                break;
            } else {
                qDebug() << "failed to set keyboard, trying next possibility";
            }
        }
    } else {
        qDebug() << "activated keyboard is already desired keyboard";
    }

    // Press and release the Win+H key combo to activate dictation
    INPUT input[2] = {};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_LWIN; // Left Windows key
    input[0].ki.dwFlags = 0;   // Key press
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = 'H';   // 'H' key
    input[1].ki.dwFlags = 0; // Key press
    SendInput(2, input, sizeof(INPUT));

    input[0].ki.dwFlags = KEYEVENTF_KEYUP; // Key release
    input[1].ki.dwFlags = KEYEVENTF_KEYUP; // Key release
    SendInput(2, input, sizeof(INPUT));
}

void Transcriber::stopRecognition()
{
    // Restore the original keyboard layout
    ActivateKeyboardLayout(_originalLayout, KLF_ACTIVATE);
}
