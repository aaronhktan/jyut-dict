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

    constexpr auto PRODUCT_NAME = "Jyut Dictionary";
    constexpr auto CURRENT_VERSION = "0.19.614";
    constexpr auto PRODUCT_DESCRIPTION
        = "A free, open-source offline Cantonese Dictionary.";
    constexpr auto CREDITS_TEXT
        = "Icon based on <a "
          "href=https://commons.wikimedia.org/wiki/"
          "Commons:Ancient_Chinese_characters_project "
          "style=\"color: %1; text-decoration: none\">Wikimedia Ancient "
          "Chinese characters project</a> files."
          "<br>UI icons from <a href=https://feathericons.com "
          "style=\"color: %1; text-decoration: none\">Feather Icons</a>."
          "<br>Made with <a href=\"#\" style=\"color: %1; text-decoration: none\">"
          "Qt</a>.";
    constexpr auto GITHUB_LINK = "https://github.com/aaronhktan/jyut-dict";

    void split(const std::string &string,
               const char delimiter,
               std::vector<std::string> &result);

    void split(const std::string &string,
               const std::string delimiter,
               std::vector<std::string> &result);
}

#endif // UTILS_H
