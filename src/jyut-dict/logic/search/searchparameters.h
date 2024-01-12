#ifndef SEARCHPARAMETERS_H
#define SEARCHPARAMETERS_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QDataStream>
#else
#include "logic/utils/qvariantutils.h"
#endif

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
