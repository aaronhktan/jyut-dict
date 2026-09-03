#ifndef SEARCHPARAMETERS_H
#define SEARCHPARAMETERS_H

#include <QtGlobal>
#include <QDataStream>

#include <QMetaType>

// An enum that defines all possible search parameters

enum class SearchParameters : int {
    SIMPLIFIED = 1,
    TRADITIONAL,
    CHINESE,

    JYUTPING,
    PINYIN,

    ENGLISH,

    AUTO_DETECT = 1000,
};

Q_DECLARE_METATYPE(SearchParameters)

#endif // SEARCHPARAMETERS_H
