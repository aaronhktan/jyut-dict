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
    SIMPLIFIED  = 0x01,
    TRADITIONAL = 0x02,
    CHINESE     = SIMPLIFIED | TRADITIONAL,

    JYUTPING    = 0x04,
    PINYIN      = 0x05,

    ENGLISH     = 0x06,
};

Q_DECLARE_METATYPE(SearchParameters)

#endif // SEARCHPARAMETERS_H
