package com.aaronhktan.cantonesedictionary.logic.entry

enum class EntryCharactersOptions {
    ONLY_TRADITIONAL, ONLY_SIMPLIFIED, PREFER_SIMPLIFIED, PREFER_TRADITIONAL
}

enum class EntryPhoneticOptions {
    ONLY_CANTONESE, ONLY_MANDARIN, PREFER_CANTONESE, PREFER_MANDARIN
}

enum class EntryColourPhoneticType {
    NONE, CANTONESE, MANDARIN
}

enum class CantoneseOptions {
    RAW_JYUTPING, PRETTY_YALE, CANTONESE_IPA
}

enum class MandarinOptions {
    RAW_PINYIN, PRETTY_PINYIN, NUMBERED_PINYIN, ZHUYIN, MANDARIN_IPA, RAW_PINYIN_WITH_V
}

class Entry {
}