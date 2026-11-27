#ifndef SEARCHPARAMETERS_H
#define SEARCHPARAMETERS_H

#include <QDataStream>
#include <QTranslator>
#include <QtGlobal>

#include <QMetaType>

// An enum that defines all possible search parameters

enum class SearchParameters : int {
    SIMPLIFIED = 0,
    TRADITIONAL = 1,

    JYUTPING = 2,
    FUZZY_JYUTPING = 3,
    PINYIN = 4,
    FUZZY_PINYIN = 5,

    ENGLISH = 6,

    AUTO_DETECT = 1000,
    NONE = 1001,
};

Q_DECLARE_METATYPE(SearchParameters)

#endif // SEARCHPARAMETERS_H
