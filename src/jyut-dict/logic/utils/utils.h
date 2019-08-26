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
    const std::vector<std::string> ASSET_FORMATS = {".deb", ".tar.bz2"};
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

    // Strings that are not language dependent should go here.
    // If they need to be translated, put them in strings.h
    constexpr auto CURRENT_VERSION = "0.18.0614";
    constexpr auto AUTHOR_EMAIL = "mailto: hi@aaronhktan.com";
    constexpr auto DONATE_LINK = "https://www.paypal.me/cheeseisdisgusting";
    constexpr auto AUTHOR_GITHUB_LINK = "https://github.com/aaronhktan/";
    constexpr auto GITHUB_LINK = "https://github.com/aaronhktan/jyut-dict";

    void split(const std::string &string,
               const char delimiter,
               std::vector<std::string> &result);

    void split(const std::string &string,
               const std::string delimiter,
               std::vector<std::string> &result);
}

#endif // UTILS_H
