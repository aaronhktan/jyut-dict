#ifndef QVARIANTUTILS_H
#define QVARIANTUTILS_H

#include <QDataStream>
#include <QMetaType>

#include <type_traits>

// These are required to convert an enum class to a QVariant.
// QVariant serializes and deserializes with a QDataStream, and
// the default message is the "QVariant::load: unable to load/save type" error.
// By overriding the operators, the errors are suppressed and data is correctly
// serialized/deserialized.
// See https://stackoverflow.com/questions/42868924/templated-qdatastream-operator-for-enumerations

template<typename T, typename U = std::enable_if_t<std::is_enum<T>::value>>
QDataStream &operator<<(QDataStream &stream, T enumValue)
{
    stream << static_cast<std::underlying_type_t<T>>(enumValue);
    return stream;
}

template<typename T,
         typename = typename std::enable_if<std::is_enum<T>::value>::type>
QDataStream &operator>>(QDataStream &stream, T &e)
{
    std::underlying_type_t<T> v;
    stream >> v;
    e = static_cast<T>(v);
    return stream;
}

#endif // QVARIANTUTILS_H
