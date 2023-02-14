#ifndef UTILS_H
#define UTILS_H

#include "logic/strings/strings.h"

#include <string>
#include <vector>

// The Utils class provides a set of utilities and defines useful
// in several places in the application.

namespace Utils {
#ifdef Q_OS_MAC
    constexpr auto PLATFORM_NAME = "macOS";
    const std::vector<std::string> ASSET_FORMATS = {".zip", ".dmg"};
#elif defined(Q_OS_WIN)
    constexpr auto PLATFORM_NAME = "Windows";
    const std::vector<std::string> ASSET_FORMATS = {".msi", ".exe", ".zip"};
#elif defined(Q_OS_LINUX)
    constexpr auto PLATFORM_NAME = "Linux";
#ifdef FLATPAK
    const std::vector<std::string> ASSET_FORMATS = {".flatpak"};
#elif defined(APPIMAGE)
    const std::vector<std::string> ASSET_FORMATS = {".appimage"};
#else
    const std::vector<std::string> ASSET_FORMATS = {".deb", ".tar.bz2"};
#endif
#endif

#if defined(Q_PROCESSOR_X86_64)
    constexpr auto ARCHITECTURE = "64";
#elif defined(Q_PROCESSOR_X86_32)
    constexpr auto ARCHITECTURE = "32";
#else
    constexpr auto ARCHITECTURE = "";
#endif

#ifdef PORTABLE
    constexpr auto PORTABILITY = "portable";
#else
    constexpr auto PORTABILITY = "install";
#endif

#ifdef Q_OS_WIN
    constexpr auto SAME_CHARACTER_STRING = "−";
#else
    constexpr auto SAME_CHARACTER_STRING = "－";
#endif

    // Index of buttons for Searching
    enum ButtonOptionIndex {
        SIMPLIFIED_BUTTON_INDEX,
        TRADITIONAL_BUTTON_INDEX,
        JYUTPING_BUTTON_INDEX,
        PINYIN_BUTTON_INDEX,
        ENGLISH_BUTTON_INDEX,
        MAXIMUM_BUTTON_INDEX
    };

    // Strings that are not language dependent should go here.
    // If they need to be translated, put them in strings.h
    constexpr auto CURRENT_VERSION = "1.22.0207";
    constexpr auto AUTHOR_EMAIL = "mailto: hi@aaronhktan.com";
    constexpr auto DONATE_LINK = "https://www.paypal.me/cheeseisdisgusting";
    constexpr auto AUTHOR_GITHUB_LINK = "https://github.com/aaronhktan/";
    constexpr auto GITHUB_LINK = "https://github.com/aaronhktan/jyut-dict";

    // Strings used for adding languages
#ifdef Q_OS_MAC
    constexpr auto TTS_LINK = "x-apple.systempreferences:com.apple."
                              "preference.universalaccess?TextToSpeech";
    constexpr auto TTS_HELP_LINK
        = "https://support.apple.com/guide/mac-help/mchlp2290/mac";
#elif defined(Q_OS_LINUX)
    constexpr auto TTS_LINK
        = "https://packages.debian.org/stretch/libspeechd-dev";
    constexpr auto TTS_HELP_LINK
        = "https://packages.debian.org/stretch/libspeechd-dev";
#else
    constexpr auto TTS_LINK
        = "ms-settings:regionlanguage";
    constexpr auto TTS_HELP_LINK
        = "https://support.microsoft.com/en-ca/help/22805/"
          "windows-10-supported-narrator-languages-voices";
#endif

    void split(const std::string &string,
               const char delimiter,
               std::vector<std::string> &result);

    void split(const std::string &string,
               const std::string delimiter,
               std::vector<std::string> &result);

    void trim(const std::string &string, std::string &result);
}

#endif // UTILS_H
