#ifndef ENTRYPHONETICOPTIONS_H
#define ENTRYPHONETICOPTIONS_H

#include "logic/utils/qvariantutils.h"

#include <QMetaType>

// An enum that tells how to show the phonetic guide of an entry

enum EntryPhoneticOptions {
    ONLY_JYUTPING,
    ONLY_PINYIN,
    PREFER_JYUTPING,
    PREFER_PINYIN,
};

enum CantoneseOptions {
    RAW_JYUTPING,
};

enum MandarinOptions {
    RAW_PINYIN,
    PRETTY_PINYIN,
};

Q_DECLARE_METATYPE(EntryPhoneticOptions);
Q_DECLARE_METATYPE(CantoneseOptions);
Q_DECLARE_METATYPE(MandarinOptions);

#endif // ENTRYPHONETICOPTIONS_H
