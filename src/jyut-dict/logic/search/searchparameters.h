#ifndef SEARCHPARAMETERS_H
#define SEARCHPARAMETERS_H

// An enum that defines all possible search parameters

enum class SearchParameters {
    SIMPLIFIED  = 0x01,
    TRADITIONAL = 0x02,
    CHINESE     = SIMPLIFIED | TRADITIONAL,

    JYUTPING    = 0x04,
    PINYIN      = 0x05,

    ENGLISH     = 0x06,
};

#endif // SEARCHPARAMETERS_H
