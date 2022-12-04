#ifndef ENTRYPHONETICOPTIONS_H
#define ENTRYPHONETICOPTIONS_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QDataStream>
#else
#include "logic/utils/qvariantutils.h"
#endif
#include <QMetaType>

#include <type_traits>

// An enum that tells how to show the phonetic guide of an entry

enum class EntryPhoneticOptions : int {
    ONLY_CANTONESE,
    ONLY_MANDARIN,
    PREFER_CANTONESE,
    PREFER_MANDARIN,
};

enum class EntryColourPhoneticType : int {
    NONE,
    JYUTPING,
    PINYIN,
};

enum class CantoneseOptions : int {
    RAW_JYUTPING = (0x1 << 0),
    PRETTY_YALE = (0x1 << 1),
};

// These are required so that bitwise operations are allowed on the enum class
constexpr inline CantoneseOptions operator~ (CantoneseOptions a) { return static_cast<CantoneseOptions>( ~static_cast<std::underlying_type<CantoneseOptions>::type>(a) ); }
constexpr inline CantoneseOptions operator| (CantoneseOptions a, CantoneseOptions b) { return static_cast<CantoneseOptions>( static_cast<std::underlying_type<CantoneseOptions>::type>(a) | static_cast<std::underlying_type<CantoneseOptions>::type>(b) ); }
constexpr inline CantoneseOptions operator& (CantoneseOptions a, CantoneseOptions b) { return static_cast<CantoneseOptions>( static_cast<std::underlying_type<CantoneseOptions>::type>(a) & static_cast<std::underlying_type<CantoneseOptions>::type>(b) ); }
constexpr inline CantoneseOptions operator^ (CantoneseOptions a, CantoneseOptions b) { return static_cast<CantoneseOptions>( static_cast<std::underlying_type<CantoneseOptions>::type>(a) ^ static_cast<std::underlying_type<CantoneseOptions>::type>(b) ); }

enum class MandarinOptions : int {
    NUMBERED_PINYIN = (0x1 << 0),
    PRETTY_PINYIN = (0x1 << 1),
};

constexpr inline MandarinOptions operator~ (MandarinOptions a) { return static_cast<MandarinOptions>( ~static_cast<std::underlying_type<MandarinOptions>::type>(a) ); }
constexpr inline MandarinOptions operator| (MandarinOptions a, MandarinOptions b) { return static_cast<MandarinOptions>( static_cast<std::underlying_type<MandarinOptions>::type>(a) | static_cast<std::underlying_type<MandarinOptions>::type>(b) ); }
constexpr inline MandarinOptions operator& (MandarinOptions a, MandarinOptions b) { return static_cast<MandarinOptions>( static_cast<std::underlying_type<MandarinOptions>::type>(a) & static_cast<std::underlying_type<MandarinOptions>::type>(b) ); }
constexpr inline MandarinOptions operator^ (MandarinOptions a, MandarinOptions b) { return static_cast<MandarinOptions>( static_cast<std::underlying_type<MandarinOptions>::type>(a) ^ static_cast<std::underlying_type<MandarinOptions>::type>(b) ); }

Q_DECLARE_METATYPE(EntryPhoneticOptions);
Q_DECLARE_METATYPE(EntryColourPhoneticType);
Q_DECLARE_METATYPE(CantoneseOptions);
Q_DECLARE_METATYPE(MandarinOptions);

#endif // ENTRYPHONETICOPTIONS_H
