#ifndef ENTRYPHONETICOPTIONS_H
#define ENTRYPHONETICOPTIONS_H

// An enum that tells how to show the phonetic guide of an entry

enum class EntryPhoneticOptions {
    ONLY_JYUTPING,
    ONLY_PINYIN,
    PREFER_JYUTPING,
    PREFER_PINYIN,
};

enum class CantoneseOptions {
    RAW_JYUTPING,
};

enum class MandarinOptions {
    RAW_PINYIN,
    PRETTY_PINYIN,
};

#endif // ENTRYPHONETICOPTIONS_H
