#ifndef QVARIANTUTILS_H
#define QVARIANTUTILS_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/search/searchparameters.h"
#endif

#include <QDataStream>
#include <QMetaType>

#include <type_traits>

// These are required to convert an enum class to a QVariant.
// QVariant serializes and deserializes with a QDataStream, and
// the default message is the "QVariant::load: unable to load/save type" error.
// By overriding the operators, the errors are suppressed and data is correctly
// serialized/deserialized.
// See https://stackoverflow.com/questions/42868924/templated-qdatastream-operator-for-enumerations

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
QDataStream &operator<<(QDataStream &stream, EntryCharactersOptions enumValue)
{
    stream << static_cast<std::underlying_type_t<EntryCharactersOptions>>(enumValue);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EntryCharactersOptions &e)
{
    std::underlying_type_t<EntryCharactersOptions> v;
    stream >> v;
    e = static_cast<EntryCharactersOptions>(v);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, EntryPhoneticOptions enumValue)
{
    stream << static_cast<std::underlying_type_t<EntryPhoneticOptions>>(enumValue);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EntryPhoneticOptions &e)
{
    std::underlying_type_t<EntryPhoneticOptions> v;
    stream >> v;
    e = static_cast<EntryPhoneticOptions>(v);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, EntryColourPhoneticType enumValue)
{
    stream << static_cast<std::underlying_type_t<EntryColourPhoneticType>>(enumValue);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, EntryColourPhoneticType &e)
{
    std::underlying_type_t<EntryColourPhoneticType> v;
    stream >> v;
    e = static_cast<EntryColourPhoneticType>(v);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, CantoneseOptions enumValue)
{
    stream << static_cast<std::underlying_type_t<CantoneseOptions>>(enumValue);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, CantoneseOptions &e)
{
    std::underlying_type_t<CantoneseOptions> v;
    stream >> v;
    e = static_cast<CantoneseOptions>(v);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, MandarinOptions enumValue)
{
    stream << static_cast<std::underlying_type_t<MandarinOptions>>(enumValue);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, MandarinOptions &e)
{
    std::underlying_type_t<MandarinOptions> v;
    stream >> v;
    e = static_cast<MandarinOptions>(v);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, SearchParameters enumValue)
{
    stream << static_cast<std::underlying_type_t<SearchParameters>>(enumValue);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, SearchParameters &e)
{
    std::underlying_type_t<SearchParameters> v;
    stream >> v;
    e = static_cast<SearchParameters>(v);
    return stream;
}
#else
template<typename T, typename U = std::enable_if_t<std::is_enum<T>::value>>
QDataStream &operator<<(QDataStream &stream, T enumValue)
{
    stream << static_cast<std::underlying_type_t<SearchParameters>>(enumValue);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, SearchParameters &e)
{
    std::underlying_type_t<SearchParameters> v;
    stream >> v;
    e = static_cast<SearchParameters>(v);
    return stream;
}
#endif

#endif // QVARIANTUTILS_H
