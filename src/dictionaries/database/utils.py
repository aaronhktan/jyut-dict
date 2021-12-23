import logging
import re

PINYIN_CORRESPONDENCE_1 = {
    "ā": "a1",
    "á": "a2",
    "ǎ": "a3",
    "à": "a4",
    "a": "a5",
    "ō": "o1",
    "ó": "o2",
    "ǒ": "o3",
    "ò": "o4",
    "o": "o5",
    "ē": "e1",
    "é": "e2",
    "ě": "e3",
    "è": "e4",
    "e": "e5",
    "ī": "i1",
    "í": "i2",
    "ǐ": "i3",
    "ì": "i4",
    "i": "i5",
    "ū": "u1",
    "ú": "u2",
    "ǔ": "u3",
    "ù": "u4",
    "u": "u5",
    "ǖ": "u:1",
    "ǘ": "u:2",
    "ǚ": "u:3",
    "ǜ": "u:4",
    "ü": "u:5",
}
PINYIN_CORRESPONDENCE_2 = {
    "āi": "ai1",
    "ái": "ai2",
    "ǎi": "ai3",
    "ài": "ai4",
    "ai": "ai5",
    "ēi": "ei1",
    "éi": "ei2",
    "ěi": "ei3",
    "èi": "ei4",
    "ei": "ei5",
    "uī": "ui1",
    "uí": "ui2",
    "uǐ": "ui3",
    "uì": "ui4",
    "ui": "ui5",
    "āo": "ao1",
    "áo": "ao2",
    "ǎo": "ao3",
    "ào": "ao4",
    "ao": "ao5",
    "ōu": "ou1",
    "óu": "ou2",
    "ǒu": "ou3",
    "òu": "ou4",
    "ou": "ou5",
    "iū": "iu1",
    "iú": "iu2",
    "iǔ": "iu3",
    "iù": "iu4",
    "iu": "iu5",
    "iē": "ie1",
    "ié": "ie2",
    "iě": "ie3",
    "iè": "ie4",
    "ie": "ie5",
    "üē": "u:e1",
    "üé": "u:e2",
    "üě": "u:e3",
    "üè": "u:e4",
    "üe": "u:e5",
    "ēr": "er1",
    "ér": "er2",
    "ěr": "er3",
    "èr": "er4",
    "er": "er5",
    "ān": "an1",
    "án": "an2",
    "ǎn": "an3",
    "àn": "an4",
    "an": "an5",
    "ēn": "en1",
    "én": "en2",
    "ěn": "en3",
    "èn": "en4",
    "en": "en5",
    "īn": "in1",
    "ín": "in2",
    "ǐn": "in3",
    "ìn": "in4",
    "in": "in5",
    "ūn": "un1",
    "ún": "un2",
    "ǔn": "un3",
    "ùn": "un4",
    "un": "un5",
    "ǖn": "u:n1",
    "ǘn": "u:n2",
    "ǚn": "u:n3",
    "ǜn": "u:n4",
    "ün": "u:n5",
    "ār": "a1 er5",
    "ár": "a2 er5",
    "ǎr": "a3 er5",
    "àr": "a4 er5",
    "ar": "a5 er5",
    "ōr": "o1 er5",
    "ór": "o2 er5",
    "ǒr": "o3 er5",
    "òr": "o4 er5",
    "or": "o5 er5",
    "īr": "i1 er5",
    "ír": "i2 er5",
    "ǐr": "i3 er5",
    "ìr": "i4 er5",
    "ir": "i5 er5",
    "ūr": "u1 er5",
    "úr": "u2 er5",
    "ǔr": "u3 er5",
    "ùr": "u4 er5",
    "ur": "u5 er5",
    "ǖr": "u:1 er5",
    "ǘr": "u:2 er5",
    "ǚr": "u:3 er5",
    "ǜr": "u:4 er5",
    "ür": "u:5 er5",
}
PINYIN_CORRESPONDENCE_3 = {
    "āng": "ang1",
    "áng": "ang2",
    "ǎng": "ang3",
    "àng": "ang4",
    "ang": "ang5",
    "ēng": "eng1",
    "éng": "eng2",
    "ěng": "eng3",
    "èng": "eng4",
    "eng": "eng5",
    "īng": "ing1",
    "íng": "ing2",
    "ǐng": "ing3",
    "ìng": "ing4",
    "ing": "ing5",
    "ōng": "ong1",
    "óng": "ong2",
    "ǒng": "ong3",
    "òng": "ong4",
    "ong": "ong5",
    "āir": "ai1 er5",
    "áir": "ai2 er5",
    "ǎir": "ai3 er5",
    "àir": "ai4 er5",
    "air": "ai5 er5",
    "ēir": "ei1 er5",
    "éir": "ei2 er5",
    "ěir": "ei3 er5",
    "èir": "ei4 er5",
    "eir": "ei5 er5",
    "uīr": "ui1 er5",
    "uír": "ui2 er5",
    "uǐr": "ui3 er5",
    "uìr": "ui4 er5",
    "uir": "ui5 er5",
    "āor": "ao1 er5",
    "áor": "ao2 er5",
    "ǎor": "ao3 er5",
    "àor": "ao4 er5",
    "aor": "ao5 er5",
    "ōur": "ou1 er5",
    "óur": "ou2 er5",
    "ǒur": "ou3 er5",
    "òur": "ou4 er5",
    "our": "ou5 er5",
    "iūr": "iu1 er5",
    "iúr": "iu2 er5",
    "iǔr": "iu3 er5",
    "iùr": "iu4 er5",
    "iur": "iu5 er5",
    "iēr": "ie1 er5",
    "iér": "ie2 er5",
    "iěr": "ie3 er5",
    "ièr": "ie4 er5",
    "ier": "ie5 er5",
    "üēr": "u:e1 er5",
    "üér": "u:e2 er5",
    "üěr": "u:e3 er5",
    "üèr": "u:e4 er5",
    "üer": "u:e5 er5",
    "ānr": "an1 er5",
    "ánr": "an2 er5",
    "ǎnr": "an3 er5",
    "ànr": "an4 er5",
    "anr": "an5 er5",
    "ēnr": "en1 er5",
    "énr": "en2 er5",
    "ěnr": "en3 er5",
    "ènr": "en4 er5",
    "enr": "en5 er5",
    "īnr": "in1 er5",
    "ínr": "in2 er5",
    "ǐnr": "in3 er5",
    "ìnr": "in4 er5",
    "inr": "in5 er5",
    "ūnr": "un1 er5",
    "únr": "un2 er5",
    "ǔnr": "un3 er5",
    "ùnr": "un4 er5",
    "unr": "un5 er5",
    "ǖn": "u:n1 er5",
    "ǘn": "u:n2 er5",
    "ǚn": "u:n3 er5",
    "ǜn": "u:n4 er5",
    "ün": "u:n5 er5",
}
PINYIN_CORRESPONDENCE_4 = {
    "āngr": "ang1 er5",
    "ángr": "ang2 er5",
    "ǎngr": "ang3 er5",
    "àngr": "ang4 er5",
    "angr": "ang5 er5",
    "ēngr": "eng1 er5",
    "éngr": "eng2 er5",
    "ěngr": "eng3 er5",
    "èngr": "eng4 er5",
    "engr": "eng5 er5",
    "īngr": "ing1 er5",
    "íngr": "ing2 er5",
    "ǐngr": "ing3 er5",
    "ìngr": "ing4 er5",
    "ingr": "ing5 er5",
    "ōngr": "ong1 er5",
    "óngr": "ong2 er5",
    "ǒngr": "ong3 er5",
    "òngr": "ong4 er5",
    "ongr": "ong5 er5",
}


# Convert pinyin to tone numbers instead of the tone marks
def pinyin_to_tone_numbers(pinyin, word):
    ret = pinyin

    if pinyin[-4:] in PINYIN_CORRESPONDENCE_4:
        ret = pinyin[:-4] + PINYIN_CORRESPONDENCE_4[pinyin[-4:]]
    elif pinyin[-3:] in PINYIN_CORRESPONDENCE_3:
        ret = pinyin[:-3] + PINYIN_CORRESPONDENCE_3[pinyin[-3:]]
    elif pinyin[-2:] in PINYIN_CORRESPONDENCE_2:
        ret = pinyin[:-2] + PINYIN_CORRESPONDENCE_2[pinyin[-2:]]
    elif pinyin[-1:] in PINYIN_CORRESPONDENCE_1:
        ret = pinyin[:-1] + PINYIN_CORRESPONDENCE_1[pinyin[-1:]]
    else:
        logging.warning(
            f"Pinyin {pinyin} for word {word} could not be converted to tone numbers, uh oh!"
        )

    return ret


# Since the pinyin returned by lazy_pinyin doesn't always match the pinyin
# given in the heteronym, attempt to replace pinyin corresponding to the
# characters in this heteronym with the pinyin provided by the source file.
#
# e.g. example_text = "重新"; example_pinyin = "zhong4 xin1" (returned by lazy_pinyin)
# trad = "重", phrase_pinyin = "chong2" (provided by source file)
# means that we should convert "zhong4 xin1" to "chong2 xin1"
def change_pinyin_to_match_phrase(example, example_pinyin, phrase, phrase_pinyin):
    phrase_indices = [i.start() for i in re.finditer(phrase, example)]
    example_pinyin_list = example_pinyin.split()
    phrase_pinyin_list = phrase_pinyin.split()

    for i in phrase_indices:
        # I can't do a simple replacement with list slicing, because
        # sometimes the example contains punctuation that the pinyin does not have
        # (e.g. "三十年河東，三十年河西" -> "san1 shi2 nian2 he2 dong1 san1 shi2 nian2 he2 xi1")
        # so we must loop through the example, ignoring punctuation
        example_index = 0
        phrase_pinyin_index = 0
        while phrase_pinyin_index < len(phrase_pinyin_list):
            if example_pinyin_list[i + example_index] == "，":
                example_index += 1
                continue
            example_pinyin_list[i + example_index] = phrase_pinyin_list[
                phrase_pinyin_index
            ]
            example_index += 1
            phrase_pinyin_index += 1

    return " ".join(example_pinyin_list)
