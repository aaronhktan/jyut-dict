#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Utils {
#ifdef Q_OS_MAC
    constexpr auto PLATFORM_NAME = "macOS";
    const std::vector<std::string> ASSET_FORMATS = {".zip", ".dmg"};
#elif defined(Q_OS_WIN)
    constexpr auto PLATFORM_NAME = "Windows";
    const std::vector<std::string> ASSET_FORMATS = {".msi", ".exe"};
#elif defined(Q_OS_LINUX)
    constexpr auto PLATFORM_NAME = "Linux";
    const std::vector<std::string> ASSET_FORMATS = {".deb", ".tar.bz2"};
#endif

    constexpr auto CURRENT_VERSION = "0.19.0614";
    constexpr auto GITHUB_LINK = "https://github.com/aaronhktan/jyut-dict";

    void split(const std::string &string,
               const char delimiter,
               std::vector<std::string> &result);
}

#endif // UTILS_H
