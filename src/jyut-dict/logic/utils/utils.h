#ifndef UTILS_H
#define UTILS_H

#include <qglobal.h>

#include <string>
#include <system_error>
#include <vector>

// The Utils class provides a set of utilities and defines useful
// in several places in the application.

namespace Utils {
#ifdef APPIMAGE
constexpr auto VARIANT = "appimage";
#else
#ifdef PORTABLE
constexpr auto VARIANT = "portable";
#else
constexpr auto VARIANT = "install";
#endif
#endif

#ifdef Q_OS_WIN
    constexpr auto SAME_CHARACTER_STRING = "−";
#else
    constexpr auto SAME_CHARACTER_STRING = "－";
#endif

    template<class T>
    using Result = std::variant<std::system_error, T>;

    // Strings that are not language dependent should go here.
    // If they need to be translated, put them in strings.h
    constexpr auto CURRENT_VERSION = "4.25.0623";
    constexpr auto AUTHOR_EMAIL = "mailto: hi@aaronhktan.com";
    constexpr auto DONATE_LINK = "https://ko-fi.com/aaronhktan";
    constexpr auto AUTHOR_GITHUB_LINK = "https://github.com/aaronhktan/";
    constexpr auto GITHUB_LINK = "https://github.com/aaronhktan/jyut-dict";
    constexpr auto WEBSITE_LINK = "https://jyutdictionary.com";
    constexpr auto DICTIONARY_DOWNLOAD_LINK
        = "https://jyutdictionary.com/#download-addon";
    constexpr auto EASTER_EGG_LINK
        = "https://img02.tooopen.com/20150824/tooopen_sy_139151591238.jpg";

    // Strings used for adding languages
#ifdef Q_OS_MAC
    constexpr auto TTS_LINK = "x-apple.systempreferences:com.apple."
                              "preference.universalaccess?TextToSpeech";
    constexpr auto TTS_HELP_LINK
        = "https://support.apple.com/guide/mac-help/mchlp2290/mac";
    constexpr auto PRIVACY_MICROPHONE_LINK
        = "x-apple.systempreferences:com.apple.preference.security?Privacy_"
          "Microphone";
    constexpr auto PRIVACY_SPEECH_LINK
        = "x-apple.systempreferences:com.apple.preference.security?Privacy_"
          "SpeechRecognition";
#elif defined(Q_OS_LINUX)
    constexpr auto TTS_LINK
        = "https://packages.debian.org/stretch/libspeechd-dev";
    constexpr auto TTS_HELP_LINK
        = "https://packages.debian.org/stretch/libspeechd-dev";
#else
    constexpr auto TTS_LINK
        = "ms-settings:regionlanguage";
    constexpr auto TTS_HELP_LINK
        = "https://support.microsoft.com/en-us/windows/"
          "appendix-a-supported-languages-and-voices-4486e345-7730-53da-fcfe-"
          "55cc64300f01";
#endif

    void split(const std::string &string,
               const char delimiter,
               std::vector<std::string> &result);

    void split(std::string_view string,
               const std::string delimiter,
               std::vector<std::string> &result);

    void trim(const std::string &string, std::string &result);
    } // namespace Utils

#endif // UTILS_H
