from dragonmapper import transcriptions
import opencc
import pinyin_jyutping_sentence
from pypinyin import lazy_pinyin, Style
from pypinyin.contrib.tone_convert import to_normal, to_tone3
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

import copy
import json
import logging
import re
import sys
import sqlite3
import unicodedata

# Useful test words:
# - 太: secondary mandarin pronunciation in parentheses
# - 孫子: mandarin neutral tone
# - 磚坯: multiple mandarin pronunciations
# - 浣熊: Taiwan and Mainland China pronunciations
# - 哎喲: Mandarin yo1 as syllable
# - 三Q: Mandarin kiu1 as syllable
# - M: Mandarin aim2 as syllable
# - K: Mandarin kei1 as syllable
# - 哦: Mandarin o2 as syllable
# - 噷: Mandarin hm5 as syllable
# - 呵: Mandarin o1 as syllable
# - 哼: Mandarin 哼 as syllable
# - 學生: Multiple pronunciations in Cantonese and Mandarin
# - 佢哋: Basic Cantonese test
# - 綠: ǜ in Pinyin
# - 澳洲: example with numbers in Pinyin
# - 2019冠狀病毒病: example with numbers in Pinyin
# - 芒: duplicate definition
# - 奇異筆: Basic Cantonese and Mandarin test
# - 回去: Mandarin neutral tone
# - 閪: Basic Cantonese test

# Useful test examples:
# - 呢度好閪热呀！ [Cantonese, simp.]nei¹ dou⁶ hou² hai¹ jit⁶ aa³! [Jyutping]
# - 驚閪咩？／惊閪咩？ [Cantonese] ― geng¹ hai¹ me¹? [Jyutping] ― What the fuck I am afraid of?
# - 有閪用！ [Cantonese] ― jau⁵ hai¹ jung⁶! [Jyutping] ― There is no effing use of it!
# - 撚狗行动 [Cantonese, simp.]From: 2019, https://hk.news.appledaily.com/local/realtime/article/20190621/59740269\nlan² gau² hang⁴ dung⁶ [Jyutping]
# - 佢识得话人唔识得话自己。 [Guangzhou Cantonese, simp.]keoi⁵ sik¹ dak¹ waa⁶ jan⁴ m⁴ sik¹ dak¹ waa⁶ zi⁶ gei². [Jyutping]
# - 冬天捐咗入被窦度就唔愿起身。 [Cantonese, simp.]From: 開放詞典 [Kaifang Cidian]\ndung¹ tin¹ gyun¹ zo² jap⁶ pei⁵ dau³ dou⁶ zau⁶ m⁴ jyun⁶ hei² san¹. [Jyutping]
# - 我而家喺香港。 [Cantonese, trad. and simp.]ngo⁵ ji⁴ gaa¹ hai² hoeng¹ gong². [Jyutping]
# - 情義已失去，恩愛都失去 [Literary Cantonese, trad.]
# - 扮示威者 [Cantonese, trad. and simp.]From: 2019, 便衣警扮示威者偷拍人群, in Apple Daily https://hk.news.appledaily.com/local/daily/article/20190627/20714653
# - C：边队band先？\nA：系\U0003154c。冇讲到系边队band。 [Cantonese, simp.]From: 1998, 收音機1 (Radio 1), Hong Kong Cantonese Corpus (HKCanCor)\nC: Bin¹ deoi⁶ ben¹ sin¹?\nA: Hai⁶ lo¹. mou⁵ gong² dou³ hai⁶ bin¹ deoi⁶ ben¹. [Jyutping]
# - 扮示威者 [Cantonese, trad. and simp.]From: 2019, 便衣警扮示威者偷拍人群, in Apple Daily https://hk.news.appledaily.com/local/daily/article/20190627/2071465
# - 佢哋係學生。／佢哋系学生。 [Guangzhou Cantonese] ― keoi⁵ dei⁶ hai⁶ hok⁶ saang¹. [Jyutping] ― They are students.
# - 达到高效、稳定、安全 [MSC, simp.]dádào gāoxiào, wěndìng, ānquán [Pinyin]
# - 阿Q被抬上了一辆没有蓬的车，几个短衣人物也和他同坐在一处。 [MSC, simp.]From: Lu Xun, 1922. The True Story of Ah Q (《阿Q正傳》)\nĀqiū bèi tái shàng le yī liàng méiyǒu péng de chē, jǐ ge duǎnyī rénwù yě hé tā tóng zuò zài yī chù. [Pinyin]
# - 我被机构投资者割韭菜了。 [MSC, simp.]From:\n10 February 2021, ““割韭菜”“韭菜”用英语怎么说？
# - 你把事情搞糟了，卻倒轉來怪我。 [MSC, trad.]\n你把事情搞糟了，却倒转来怪我。 [MSC, simp.]Nǐ bǎ shìqíng gǎo zāo le, què dàozhuǎn lái guài wǒ. [Pinyin]
# - 江干 ― jiānggān
# - 腰圍／腰围 ― yāowéi ― waistline
# - zhǐshì wēixiǎo de guāng, zhào bù liàng yī mǐ de dìfāng [Pinyin]
# - 那是一深夜 [Beijing Mandarin, trad. and simp.]From: 2000, 梁左 and 梁欢, 《闲人马大姐》, episode 7, 18:30
# - 她最近跑到上海去了。 [MSC, trad. and simp.]Tā zuìjìn pǎo dào Shànghǎi qù le. [Pinyin]
# - 只是微小的光 照不亮一米的地方 [MSC, trad. and simp.]From: 2020, 曹楊／曹杨 (Young), 微光 (Glimmer)
# - 這桌椅板凳都得從新打，太麻煩。 [Beijing Mandarin, trad.]\n这桌椅板凳都得从新打，太麻烦。 [Beijing Mandarin, simp.]Zhè zhuōyǐbǎndèng dōu děi cóngxīn dǎ, tài máfan. [Pinyin]
# - 现在的孩子们啊嘴都刁着呢，啊。 [Beijing Mandarin, simp.]From: 2004, 《家有儿女》, episode 68\nXiànzài de háizimen a zuǐ dōu diāo zhe ne, ā. [Pinyin]
# - 那是一深夜 [Beijing Mandarin, trad. and simp.]From: 2000, 梁左 and 梁欢, 《闲人马大姐》, episode 7, 18:30
# - 奴才替天下百姓感激皇太后恩典！ [MSC, trad. and simp.]From: 2010, The Firmament of the Pleiades Núcái tì tiānxià bǎixìng gǎnjī huángtàihòu ēndiǎn! [Pinyin]
# - 有一個人，是從神那裡差來的，名叫約翰。 [MSC, trad.]
# - 有一个人，是从神那里差来的，名叫约翰。 [MSC, simp.]Yǒu yī ge rén, shì cóng shén nàlǐ chā lái de, míng jiào Yuēhàn. [Pinyin]
# - 据英国《每日邮报》5月9日报导，不久前，意大利海关在从中国运过来的集装箱里发现一只小橘猫。 [MSC, simp.]From:\n2019, “中国小猫被困在集装箱 “漂流”40多天到意大利”, in sina:\nJù Yīngguó “Měirì Yóubào” 5 yuè 9 rì bàodǎo, bùjiǔqián, Yìdàlì hǎiguān zài cóng zhōngguó yùnguòlái de jízhuāngxiānglǐ fāxiàn yī zhī xiǎo júmāo. [Pinyin]
# - 甲：为啥你穿校服？\n乙：去上课呗。 [MSC, simp.]Jiǎ: Wèishá nǐ chuān xiàofú?\nYǐ: Qù shàngkè bei. [Pinyin]
# - 而现在\n乡愁是一湾浅浅的海峡\n我在这头\n大陆在那头 [MSC, simp.]From: 1972, 余光中 (Yu Kwang-chung), 鄉愁\nér xiànzài\nxiāngchóu shì yī wān qiǎn qiǎn de hǎixiá\nwǒ zài zhè tóu\nDàlù zài nà tóu [Pinyin]
# - 这孙子成绩好是因为丫老给老师舔屁眼。 [Beijing Mandarin, simp.]Zhè sūnzi chéngjì hǎo shì yīnwèi yā lǎo gěi lǎoshī tiǎn pìyǎn. [Pinyin]

CANTONESE_REGEX_0 = re.compile(
    r"(.*)\s\[(?:.* )?Cantonese.*?\]\s―\s(.*?)\s\[Jyutping\]\s―\s(.*)"
)
CANTONESE_REGEX_1 = re.compile(
    r"((?:.|\n)*) \[(?:.* )?Cantonese.*?\](?:From:\s.*?\n)?((?:.|\n)*) \[Jyutping\]"
)
CANTONESE_REGEX_2 = re.compile(r"(.*?) \[Cantonese, trad. and simp.\](From:\s.*?\\n)?")
CANTONESE_REGEX_3 = re.compile(r"(.*)\s\[Cantonese.*?\]")

JYUTPING_COLLOQUIAL_PRONUNCIATION = re.compile(r".⁻(.)")
JYUTPING_REGEX_0 = re.compile(r"(.*)\[Jyutping\]")

MANDARIN_REGEX_0 = re.compile(
    r"(.*?) \[MSC, trad\.\]\n(?:.*?) \[MSC, simp.\](.*) \[Pinyin\](?:\n)?((?:.|\n)*)?"
)
MANDARIN_REGEX_1 = re.compile(
    r"((?:.|\n)*) \[MSC, simp\.\](?:From:(?:.*|\n*)\n)?((?:.|\n)*) \[Pinyin\](?:\n)?((?:.|\n)*)?"
)
MANDARIN_REGEX_2 = re.compile(
    r"((?:.|\n)*) \[MSC, trad\.\](?:From:(?:.*|\n*)\n)?((?:.|\n)*) \[Pinyin\](?:\n)?((?:.|\n)*)?"
)
MANDARIN_REGEX_3 = re.compile(
    r"(.*?) \[MSC, trad\. and simp\.\](?:From:.*\n?.*\n?)?(.*?) \[Pinyin\](?:\n)?((?:.|\n)*)?"
)
MANDARIN_REGEX_4 = re.compile(r"(.*?) \[MSC, trad\. and simp\.\](.*)")
MANDARIN_REGEX_5 = re.compile(
    r"(.*?) \[Beijing Mandarin, trad\.\].* \[Beijing Mandarin, simp.\](?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_6 = re.compile(
    r"(.*?) \[Beijing Mandarin, simp\.\](?:From: .*?\n)?(?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_7 = re.compile(
    r"(.*?) \[Beijing Mandarin, trad\.\](?:From: .*?\n)?(?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_8 = re.compile(
    r"(.*?) \[Beijing Mandarin, trad\. and simp.\](?:From: .*?\n)?(?:(.*)(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_9 = re.compile(
    r"(.*?) \[Taiwanese Mandarin, simp\.\](From: .*?\n)?(?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_10 = re.compile(
    r"(.*?) \[Taiwanese Mandarin, trad\.\](From: .*?\n)?(?:(.*)*?(?: \[Pinyin\]))?"
)
MANDARIN_REGEX_11 = re.compile(
    r"(.*?) \[Taiwanese Mandarin, trad\. and simp\.\](?:From:(?:.*|\n*)(?:\n|” ))(.*?) \[Pinyin\]"
)
MANDARIN_REGEX_12 = re.compile(
    r"((?:.|\n)*)? \[Taiwanese Mandarin\] ― ((?:.|\n)*)? \[Pinyin\] ― ((?:.|\n)*)?"
)
MANDARIN_REGEX_13 = re.compile(r"(.*)\s―\s(.*)\s―\s(.*)")
MANDARIN_REGEX_14 = re.compile(r"(.*?)\s―\s(.*)")

PINYIN_TONELESS_SYLLABLE_PRONUNCIATION = re.compile(
    r"(?:.*)→ (.*) \(toneless final syllable variant\)"
)
PINYIN_EXTRA_ANNOTATION_REGEX = re.compile(r"(.*)(?: \(.*\))")
PINYIN_REGEX_0 = re.compile(r"(.*)\[Pinyin]")
PINYIN_ERHUA_REGEX = re.compile(r".\d")

KNOWN_WEIRD_BOPOMOFO = {
    "˙ㄏㄫ": "hng5",
    "˙ㄏㄇ": "hm5",
    "˙ㄇ": "m5",
    "˙ㄎㄧㄡ": "kiu5",
    "˙ㄧㄛ": "yo5",
    "ㄎㄟ": "kei1",
    "ㄎㄟˊ": "kei2",
    "ㄎㄟˇ": "kei3",
    "˙ㄎˋ": "kei4",
    "˙ㄎㄟ": "kei5",
    "ㄛ": "o1",
    "ㄛˊ": "o2",
    "ㄛˇ": "o3",
    "ㄛˋ": "o4",
    "˙ㄛ": "o5",
}

SUPERSCRIPT_EQUIVALENT = str.maketrans("¹²³⁴⁵⁶⁷⁸⁹⁰", "1234567890", "")
HALF_TO_FULL = dict((i, i + 0xFEE0) for i in range(0x21, 0x7F))
PUNCTUATION_TABLE = {}
PUNCTUATION_SET = set()
for i in range(sys.maxunicode):
    if unicodedata.category(chr(i)).startswith("P"):
        PUNCTUATION_TABLE[i] = " " + chr(i) + " "
        PUNCTUATION_SET.add(chr(i))

LANGUAGE_INDICATOR = (
    "Cantonese",
    "MSC",
    "Mandarin",
)

IGNORED_TEXT = (
    # We should eventually parse Literary and Classical Chinese, but ignore for now
    "Literary Chinese",
    "Classical Chinese",
    "Written Vernacular Chinese",
    # Variations of Cantonese
    "Literary Cantonese",
    # Variations of Mandarin
    "dialectal Mandarin",
    "Central Plains Mandarin",
    "Guilin Mandarin",
    "Lanyin Mandarin",
    "Malaysian Mandarin",
    "Nanjing Mandarin",
    "Northeastern Mandarin",
    "Philippine Mandarin",
    "Singaporean Mandarin",
    "Tianjin Mandarin",
    "Yangzhou Mandarin",
    # Other Sinitic languages or writing systems
    "Gan, simp.",
    "Fangyan",
    "Hainanese",
    "Hakka",
    "Hokkien",
    "Min Bei",
    "Min Dong",
    "Pe̍h-ōe-jī",
    "Shanghainese",
    "Sichuanese",
    "Sino-Korean",
    "Sino-Vietnamese",
    "Suzhounese",
    "Taishanese",
    "Teochow",
    "Teochew",
    # Wiktionary stuff
    "alt. forms:",
    "Citations:man",
    "See also:",
    "zh-co",
    "zh-x",
)

traditional_to_simplified_converter = opencc.OpenCC("hk2s.json")
simplified_to_traditional_converter = opencc.OpenCC("s2hk.json")


def insert_example(c, definition_id, starting_example_id, example):
    examples_inserted = 0

    trad = example[0].content
    simp = traditional_to_simplified_converter.convert(trad)
    jyut = example[0].pron if example[0].lang == "yue" else ""
    pin = (
        example[0].pron if example[0].lang == "cmn" or example[0].lang == "zho" else ""
    )
    lang = example[0].lang

    example_id = database.insert_chinese_sentence(
        c, trad, simp, pin, jyut, lang, starting_example_id
    )

    # Check if example insertion was successful
    if example_id == -1:
        if trad == "X" or trad == "x":
            # Ignore examples that are just 'x'
            return 0
        else:
            # If insertion failed, it's probably because the example already exists
            # Get its rowid, so we can link it to this definition
            example_id = database.get_chinese_sentence_id(
                c, trad, simp, pin, jyut, lang
            )
            if example_id == -1:  # Something went wrong if example_id is still -1
                return 0
    else:
        examples_inserted += 1

    database.insert_definition_chinese_sentence_link(c, definition_id, example_id)

    for translation in example[1:]:
        sentence = translation.content if translation.content else "X"
        lang = translation.lang

        # Check if translation already exists before trying to insert
        # Insert a translation only if the translation doesn't already exist in the database
        translation_id = database.get_nonchinese_sentence_id(c, sentence, lang)

        if translation_id == -1:
            translation_id = starting_example_id + examples_inserted
            database.insert_nonchinese_sentence(c, sentence, lang, translation_id)
            examples_inserted += 1

        # Then, link the translation to the example only if the link doesn't already exist
        link_id = database.get_sentence_link(c, example_id, translation_id)

        if link_id == -1:
            database.insert_sentence_link(c, example_id, translation_id, 1, True)

    return examples_inserted


def insert_words(c, words):
    # Reserved sentence IDs:
    #   - 0-999999999: Tatoeba
    #   - 1000000000-1999999999: words.hk
    #   - 2000000000-2999999999: CantoDict
    #   - 3000000000-3999999999: MoEDict
    #   - 4000000000-4999999999: Cross-Straits Dictionary
    #   - 5000000000-5999999999: ABC Chinese-English Dictionary
    #   - 6000000000-6999999999: ABC Cantonese-English Dictionary
    #   - 7000000000-7999999999: Wiktionary
    example_id = 7000000000

    for entry in words:
        entry_id = database.get_entry_id(
            c,
            entry.traditional,
            entry.simplified,
            entry.pinyin,
            entry.jyutping,
            entry.freq,
        )

        if entry_id == -1:
            entry_id = database.insert_entry(
                c,
                entry.traditional,
                entry.simplified,
                entry.pinyin,
                entry.jyutping,
                entry.freq,
            )
            if entry_id == -1:
                logging.error(f"Could not insert word {entry.traditional}, uh oh!")
                continue

        # print(entry, entry.definitions)
        for definition in entry.definitions:
            definition_id = database.insert_definition(
                c, definition.definition, definition.label, entry_id, 1, None
            )
            if definition_id == -1:
                logging.error(
                    f"Could not insert definition {definition} for word {entry.traditional} "
                    "- check if the definition is a duplicate!"
                )
                continue

            for example in definition.examples:
                examples_inserted = insert_example(
                    c, definition_id, example_id, example
                )
                example_id += examples_inserted


def write(db_name, source, entries):
    db = sqlite3.connect(db_name)
    c = db.cursor()

    database.write_database_version(c)

    database.drop_tables(c)
    database.create_tables(c)

    database.insert_source(
        c,
        source.name,
        source.shortname,
        source.version,
        source.description,
        source.legal,
        source.link,
        source.update_url,
        source.other,
        None,
    )

    insert_words(c, entries)

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_cantonese_romanization(romanization):
    if romanization == "":
        return romanization

    processed = romanization.translate(SUPERSCRIPT_EQUIVALENT)
    processed = JYUTPING_COLLOQUIAL_PRONUNCIATION.sub(r"\1", processed)
    return processed


def process_mandarin_romanization(romanization):
    if romanization == "":
        return romanization

    romanization = romanization.replace("'", "")

    example_romanization_list = (
        romanization.translate(PUNCTUATION_TABLE).strip().split(" ")
    )
    processed_example_romanization_list = []
    for grouping in example_romanization_list:
        if grouping == "":
            continue
        elif (
            any(punct in grouping for punct in PUNCTUATION_SET) or grouping.isnumeric()
        ):
            processed_example_romanization_list.append(grouping.translate(HALF_TO_FULL))
        else:
            try:
                syllables = transcriptions.to_pinyin(
                    transcriptions.to_zhuyin(grouping.lower())
                ).split(" ")
            except ValueError:
                logging.debug(
                    f'Parsing romanization failed for syllable(s) "{grouping}", romanization is "{romanization}"'
                )

                if grouping in KNOWN_WEIRD_BOPOMOFO:
                    grouping = KNOWN_WEIRD_BOPOMOFO[grouping]
                elif grouping[0] == "˙":
                    # Remove the neutral tone because Dragonmapper cannot handle it
                    # Dragonmapper will add the first tone since there is no tone indicated by Bopomofo symbols
                    grouping = transcriptions.to_pinyin(grouping[1:].lower())
                    # Remove the first tone added by Dragonmapper using pypinyin
                    grouping = to_normal(grouping)
                    grouping += "5"
                else:
                    grouping = grouping.translate(HALF_TO_FULL)

                processed_example_romanization_list.append(grouping)
                continue

            for syllable in syllables:
                converted_syllable = to_tone3(
                    syllable, v_to_u=True, neutral_tone_with_five=True
                )
                converted_syllable.replace("ü", "u:")

                # Erhua is not well supported in Jyut Dictionary, so convert "r" to "er"
                if PINYIN_ERHUA_REGEX.match(syllable):
                    converted_syllable = "e" + converted_syllable

                processed_example_romanization_list.append(converted_syllable)

    return " ".join(processed_example_romanization_list)


def parse_file(filename, words):
    for line in open(filename):
        if not (len(words) % 500):
            logging.info(f"Word #{len(words)} processed")

        data = json.loads(line)

        trad = data["word"]
        simp = traditional_to_simplified_converter.convert(trad)

        jyutping_list = []
        pinyin_list = []
        bopomofo_to_pinyin_list = []
        mainland_taiwain_pinyin_list = []
        if "sounds" in data:
            for pron in data["sounds"]:
                if "tags" not in pron:
                    continue

                if pron["tags"] == ["Mandarin", "Pinyin", "standard"] or pron[
                    "tags"
                ] == [
                    "Mandarin",
                    "Pinyin",
                    "standard",
                    "toneless-final-syllable-variant",
                ]:
                    pin = pron["zh-pron"]
                    pin_match = PINYIN_EXTRA_ANNOTATION_REGEX.match(pin)
                    if pin_match:
                        pin = pin_match.group(1)
                    pinyin_list.append(process_mandarin_romanization(pin))
                elif pron["tags"] == ["Mandarin", "bopomofo", "standard"] or pron[
                    "tags"
                ] == [
                    "Mandarin",
                    "bopomofo",
                    "standard",
                    "toneless-final-syllable-variant",
                ]:
                    bopomofo = pron["zh-pron"]
                    bopomofo_to_pinyin_list.append(
                        process_mandarin_romanization(bopomofo)
                    )
                elif (
                    pron["tags"]
                    == [
                        "Mainland-China",
                        "Mandarin",
                        "Standard-Chinese",
                        "bopomofo",
                    ]
                    or pron["tags"]
                    == [
                        "Mainland-China",
                        "Mandarin",
                        "Standard-Chinese",
                        "bopomofo",
                        "toneless-final-syllable-variant",
                    ]
                    or pron["tags"]
                    == [
                        "Mandarin",
                        "Standard-Chinese",
                        "Taiwan",
                        "bopomofo",
                    ]
                    or pron["tags"]
                    == [
                        "Mandarin",
                        "Standard-Chinese",
                        "Taiwan",
                        "bopomofo",
                        "toneless-final-syllable-variant",
                    ]
                ):
                    bopomofo = pron["zh-pron"]
                    mainland_taiwain_pinyin_list.append(
                        process_mandarin_romanization(bopomofo)
                    )
                elif pron["tags"] == ["Cantonese", "Guangzhou", "Jyutping"]:
                    jyutping_list.append(parse_cantonese_romanization(pron["zh-pron"]))

        if len(mainland_taiwain_pinyin_list) > len(pinyin_list):
            # There is a variance in pronunciation between Mainland China and Taiwan
            pinyin_list = mainland_taiwain_pinyin_list
        elif not pinyin_list or len(bopomofo_to_pinyin_list) != len(pinyin_list):
            # Either there is no Pinyin (due to Wiktionary parsing error),
            # or the Pinyin list is somehow longer than the Bopomofo list (which
            # should never happen). In these cases, the Bopomofo is usually more
            # reliable.
            pinyin_list = bopomofo_to_pinyin_list

        if not jyutping_list:
            jyutping_list = [
                pinyin_jyutping_sentence.jyutping(trad, tone_numbers=True, spaces=True)
            ]
        if not pinyin_list:
            pinyin_list = [
                " ".join(
                    lazy_pinyin(simp, style=Style.TONE3, neutral_tone_with_five=True)
                )
                .lower()
                .replace("v", "u:")
            ]

        freq = zipf_frequency(trad, "zh")

        entry = objects.Entry(
            trad=trad,
            simp=simp,
            jyut=jyutping_list[0],
            pin=pinyin_list[0],
            freq=freq,
            defs=set(),
        )

        words.append(entry)

        pos = data["pos"] if "pos" in data else ""

        for sense in data["senses"]:
            if "glosses" not in sense:
                continue
            gloss = sense["glosses"][0].split("\n")[0]

            synonym_set = set()
            if "synonyms" in sense:
                for synonym in sense["synonyms"]:
                    if "word" in synonym and "／" not in synonym["word"]:
                        synonym_set.add(synonym["word"].replace(" (", ""))

            antonym_set = set()
            if "antonyms" in sense:
                for antonym in sense["antonyms"]:
                    if "word" in antonym and "／" not in antonym["word"]:
                        antonym_set.add(antonym["word"].replace(" (", ""))

            definition_text = (
                gloss
                + ("\n(syn.) " + ", ".join(list(synonym_set)) if synonym_set else "")
                + ("\n(ant.) " + ", ".join(list(antonym_set)) if antonym_set else "")
            )

            definition = objects.Definition(
                definition=definition_text,
                label=", ".join([pos] + sense["tags"]) if "tags" in sense else pos,
                examples=[],
            )

            if not entry.append_to_defs(definition):
                # This definition is not unique. Skip adding it.
                # This occurs because glosses are often repeated in Wiktionary entries
                # for some reason.
                continue

            if "examples" not in sense:
                continue

            for example in sense["examples"]:
                found_example = False
                if (
                    "roman" in example
                    and "english" in example
                    and transcriptions.is_pinyin_compatible(example["roman"])
                ):
                    found_example = True
                    example_text = example["text"].split("／")[0]
                    example_romanization = process_mandarin_romanization(
                        example["roman"]
                    )
                    example_translation = (
                        example["english"] if "english" in example else ""
                    )
                    lang = "cmn"
                elif "text" in example:
                    if all(
                        language not in example["text"]
                        for language in LANGUAGE_INDICATOR
                    ):
                        if "ref" not in example:
                            continue
                        elif all(
                            language not in example["ref"]
                            for language in LANGUAGE_INDICATOR
                        ):
                            continue
                    if any(ignored in example["text"] for ignored in IGNORED_TEXT):
                        continue
                    elif "ref" in example and any(
                        ignored in example["ref"] for ignored in IGNORED_TEXT
                    ):
                        continue

                    # Generally, the Chinese sentence is in example["text"]
                    # But sometimes, it is in example["ref"]
                    if not found_example:
                        match = CANTONESE_REGEX_0.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1).split("／")[0]
                            example_romanization = parse_cantonese_romanization(
                                match.group(2)
                            )
                            example_translation = match.group(3)
                            lang = "yue"

                    if not found_example:
                        match = CANTONESE_REGEX_1.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = parse_cantonese_romanization(
                                match.group(2)
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "yue"
                        elif "ref" in example:
                            match = CANTONESE_REGEX_1.match(example["ref"])
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = parse_cantonese_romanization(
                                    match.group(2)
                                )
                                example_translation = (
                                    example["text"] if "text" in example else ""
                                )
                                lang = "yue"

                    if not found_example:
                        match = CANTONESE_REGEX_2.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = ""
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "yue"
                        elif "ref" in example:
                            match = CANTONESE_REGEX_2.match(example["ref"])
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                jyutping_match = JYUTPING_REGEX_0.match(example["text"])
                                example_romanization = (
                                    jyutping_match.group(1) if jyutping_match else ""
                                )
                                example_romanization = example_romanization.translate(
                                    SUPERSCRIPT_EQUIVALENT
                                )
                                example_translation = (
                                    example["english"] if "english" in example else ""
                                )
                                lang = "yue"

                    if not found_example:
                        match = CANTONESE_REGEX_3.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = ""
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "yue"

                    if not found_example:
                        match = MANDARIN_REGEX_0.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"]
                                if "english" in example
                                else match.group(3)
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_1.match(example["text"])
                        if match:
                            found_example = True
                            example_text = simplified_to_traditional_converter.convert(
                                match.group(1)
                            )
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"]
                                if "english" in example
                                else match.group(3)
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_2.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"]
                                if "english" in example
                                else match.group(3)
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_3.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"]
                                if "english" in example
                                else match.group(3)
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_3.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = match.group(2)
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["english"]
                                    if "english" in example
                                    else match.group(3)
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_4.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_4.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = example["ref"]
                                pinyin_match = PINYIN_REGEX_0.match(
                                    example_romanization
                                )
                                example_romanization = (
                                    pinyin_match.group(1) if pinyin_match else ""
                                )
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["english"] if "english" in example else ""
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_5.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_6.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_7.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_8.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_8.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = (
                                    match.group(2) if match.group(2) else ""
                                )
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["english"] if "english" in example else ""
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_9.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_10.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_11.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = (
                                match.group(2) if match.group(2) else ""
                            )
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                example["english"] if "english" in example else ""
                            )
                            lang = "cmn"
                        else:
                            match = (
                                MANDARIN_REGEX_11.match(example["ref"])
                                if "ref" in example
                                else None
                            )
                            if match:
                                found_example = True
                                example_text = match.group(1)
                                example_romanization = (
                                    match.group(2) if match.group(2) else ""
                                )
                                example_romanization = process_mandarin_romanization(
                                    example_romanization
                                )
                                example_translation = (
                                    example["text"] if "text" in example else ""
                                )
                                lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_12.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = match.group(3)
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_13.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = match.group(3)
                            lang = "cmn"

                    if not found_example:
                        match = MANDARIN_REGEX_14.match(example["text"])
                        if match:
                            found_example = True
                            example_text = match.group(1)
                            example_romanization = match.group(2)
                            example_romanization = process_mandarin_romanization(
                                example_romanization
                            )
                            example_translation = (
                                ""  # This regex does not have pronunciation
                            )
                            lang = "cmn"

                if found_example:
                    definition.examples.append([])
                    definition.examples[-1].append(
                        objects.ExampleTuple(lang, example_romanization, example_text)
                    )
                    definition.examples[-1].append(
                        objects.Example(lang="eng", content=example_translation)
                    )
                else:
                    logging.warning("no match found for example", example)

        for jyutping in jyutping_list[1:]:
            new_entry = copy.deepcopy(entry)
            new_entry.add_jyutping(jyutping)
            words.append(new_entry)

        for pinyin in pinyin_list[1:]:
            new_entry = copy.deepcopy(entry)
            new_entry.add_pinyin(pinyin)
            words.append(new_entry)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 -m wiktionary.parse <database filename> "
                "<dict-wk.json filepath> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m wiktionary.parse wiktionary/developer/wiktionary.db wiktionary/data/dict-wk.json "
                '"Wiktionary" WT 2023-02-16 '
                '"Wiktionary is a collaborative project to produce a free-content multilingual dictionary. '
                'It aims to describe all words of all languages using definitions and descriptions in English." '
                '"Text is available under the Creative Commons Attribution-ShareAlike License; additional terms may apply." '
                '"https://en.wiktionary.org/wiki/Wiktionary:Main_Page" "" "words,sentences"'
            )
        )
        sys.exit(1)

    source = objects.SourceTuple(
        sys.argv[3],
        sys.argv[4],
        sys.argv[5],
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10],
    )

    cc_cedict.load()

    logging.getLogger().setLevel(logging.INFO)

    words = []
    parse_file(sys.argv[2], words)
    write(sys.argv[1], source, words)
