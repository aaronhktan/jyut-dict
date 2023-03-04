from bs4 import BeautifulSoup, element
import opencc
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

import copy
import logging
import os
import re
import sqlite3
import sys

# Useful test pages:
#   - 鼻哥 for multiple definitions on a single line
#   - 一 for multiple definitions
#   - 惡 for multiple pronunciations
#   - 傾掂 for POS + multiple definitions on same line
#   - 庶 for weirdness involving newlines and <br> tags
#   - 福地 for lack of space between [1] and [Taoism/Daoism]
#   - 蚯蚓 for brackets in the middle of a definition
#   - 柏 for listing of pronunciations with commas instead of |
#   - 天秤 for listing of pronunciations with / instead of |
#   - 等 special-case Jyutping notation (-- [jp:] dang2; ting3)
#   - 嘛 has nonstandard labels for Jyutping and Pinyin (instead of 粵 and 國|普, it names jyutping or pinyin)
#   - 的 has a nonstandard way of indicating different definitions (of which all but the last definition are handled fine)
#   - 茄哩啡 has a nonstandard Jyutping notation that only it uses (-- Jyutping: /ke1 le1 fei1/ --)
#   - 操 has a nonstandard Jyutping notation that only it uses (Jyutping cou1)
#   - 一手遮天 for idiom, 香港電台 for brandname, 香港 for place name
#   - 卡拉OK has latin script in the entry
#   - AA制 starts with latin script
#   - 蛇 gwe has an extra space at the end of the entry name
#   - 犄角 has zero-width spaces in its Pinyin
#   - 購 has multiple pronunciations in Jyutping
#   - 垃圾 has Taiwan/PRC differences in pronunciation (Taiwan first)
#   - 垃圾桶 has Taiwan/PRC differences in pronunciation (PRC first)
#   - 緝捕 has Taiwan/PRC partial difference, PRC first
#   - 擊潰 has Taiwan/PRC partial difference, Taiwan first
#   - 瀕危 has Taiwan/PRC partial difference, PRC first (but at end of word, which is not currently supported)
#   - 查究 has Taiwan/PRC partial difference, Taiwan first (but at end of word, which is not currently supported)
#   - 地下 has a weird format for variant pronunciations (both indicating literary pronunciation without * and weird Jyutping/Pinyin)

# Pages with known issues:
#   - 蚺蛇 has different Jyutping for each definition, without any labelling (we discard the different pronunciations)
#   - 大使 has multiple definitions on the same line, without numbering (we put them all in one definition)

illegal_strings = (
    "Default PoS:",
    "Additional PoS:",
    "Show all nouns that can use this classifier",
)

pos_labels = {
    "v": "verb",
    "n": "noun",
    "adj": "adjective",
    "adv": "adverb",
    "prep": "preposition",
    "conj": "conjunction",
    "lit": "literal",
    "lit.": "literal",
    "syn": "synonym",
    "syn.": "synonym",
    "var.": "variant",
}

TITLE_REGEX_PATTERN = re.compile(r"<title>(.*)</title>")

# Since Jyut Dictionary does not support displaying literary/colloquial pronunciations
# in the format toi4*2, remove the literary pronunciation using this regex pattern
LITERARY_CANTONESE_READING_REGEX_PATTERN = re.compile(r"\d\*")
LITERARY_CANTONESE_READING_REGEX_PATTERN_VARIANT = re.compile(r"([1-6])([1-6])")
# CantoDict does the same with Pinyin
LITERARY_PINYIN_READING_REGEX_PATTERN = re.compile(r"\d\*")

# Get the different PRC / Taiwan pronunciations
TAIWAN_PRC_PRONUNCIATION = re.compile(
    r"^(?P<tw>.*?) ?\((?:Taiwan|TW)\) ?(?:,|\/|;)? ?(?P<prc>.*?) ?\(PRC\)$",
    re.IGNORECASE,
)
PRC_TAIWAN_PRONUNCIATION = re.compile(
    r"^(?P<prc>.*?) ?\(PRC\) ?(?:,|\/|;)? ?(?P<tw>.*?) ?\((?:Taiwan|TW)\)$",
    re.IGNORECASE,
)
MANDARIN_PRONUNCIATION_VARIANT_REGEX_PATTERNS = (
    TAIWAN_PRC_PRONUNCIATION,
    PRC_TAIWAN_PRONUNCIATION,
)
# Partial match: only some of the words' pronunciations are different
# Currently can only parse those whose partial differences are at the beginning of the word,
# since regex cannot know how long the equivalent pronunciations are
TAIWAN_PRC_PARTIAL = re.compile(
    r"^(?P<tw>.*?)\s*\((?:TW|Taiwan)\) ?(?:,|\/|;)? ?(?P<prc>.*?) ?\((?:PRC|CN)\)(?!$)",
    re.IGNORECASE,
)
PRC_TAIWAN_PARTIAL = re.compile(
    r"^(?P<prc>.*?) ?\((?:PRC|CN)\) ?(?:,|\/|;)? ?(?P<tw>.*?)\s*\((?:TW|Taiwan)\)(?!$)",
    re.IGNORECASE,
)
MANDARIN_PRONUNCIATION_PARTIAL_VARIANT_REGEX_PATTERNS = (
    TAIWAN_PRC_PARTIAL,
    PRC_TAIWAN_PARTIAL,
)

DEFINITION_SPLITTING_REGEX_PATTERN = re.compile(r"[\(|\[]\d+[\)\]]\s?|\n")

# This one searches for something in the format [粵] ok3 | [國] e4
SPECIFIC_PRONUNCIATION_REGEX_PATTERN = re.compile(
    r"\[粵\]\s?(.+\d+)\s?[\||/|,]?\s?\[[國|普]\]\s?(.+\d+)"
)
# This one searches for something in the format [loeng5 & liang3]
AMPERSAND_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\[(.*\d)\s*&\s*(.*\d)\]")
# This one searches for something in the format (jyutping) toi4, (pinyin) tai2
PARENTHESES_PRONUNCIATION_REGEX_PATTERN = re.compile(
    r"\(jyutping\)\s*(.*\d)[,|;]?\s*\(pinyin\)\s*(.*\d)"
)
# This one searches for something in the format [粵拼: dei6 haa62 | 漢語: di4 xia5]
PIPE_PRONUNCIATION_REGEX_PATTERN = re.compile(r"粵拼:\s*(.*\d)\s(?:\|\s*漢語:\s*(.*))\]")
# This one searches for something in the format 粵拼: ne1 -- 拼音: ne
DASHES_PRONUNCIATION_REGEX_PATTERN = re.compile(
    r"粵拼:\s*(.*\d)\s*(?:\-\-\s*拼音:\s*(.*))?"
)
JYUTPING_PINYIN_REGEX_PATTERNS = (
    SPECIFIC_PRONUNCIATION_REGEX_PATTERN,
    AMPERSAND_PRONUNCIATION_REGEX_PATTERN,
    PARENTHESES_PRONUNCIATION_REGEX_PATTERN,
    PIPE_PRONUNCIATION_REGEX_PATTERN,
    DASHES_PRONUNCIATION_REGEX_PATTERN,
)

# This one searches for something in the format -- [jp:] dang2; ting3
DASHES_2_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\-\-\s*\[jp:\]\s*(.*\d).*")
# This one searches for something in the format -- Jyutping: /ke1 le1 fei1/ --
SLASH_PRONUNCIATION_REGEX_PATTERN = re.compile(r"\-\-\s*Jyutping:\s*/(.*\d)/\s*\-\-")
# This one searches for something in the format Jyutping cou1
JYUTPING_PRONUNCIATION_REGEX_PATTERN = re.compile(r"Jyutping\s*(.*\d)")
JYUTPING_ONLY_REGEX_PATTERNS = (
    DASHES_2_PRONUNCIATION_REGEX_PATTERN,
    SLASH_PRONUNCIATION_REGEX_PATTERN,
    JYUTPING_PRONUNCIATION_REGEX_PATTERN,
)

LABEL_REGEX_PATTERN = re.compile(r"^\[(.*?)\]:?\s*")
SENTENCE_ID_PATTERN = re.compile(
    r"http://www\.cantonese\.sheik\.co\.uk/dictionary/examples/(\d*)/"
)

traditional_to_simplified_converter = opencc.OpenCC("hk2s.json")
simplified_to_traditional_converter = opencc.OpenCC("s2hk.json")


def write(db_name, source, entries, sentences, translations):
    db = sqlite3.connect(db_name)
    c = db.cursor()

    # Set version of database
    database.write_database_version(c)

    # Delete old tables and indices, then create new one
    database.drop_tables(c)
    database.create_tables(c)

    # Add sources to table
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

    for entry in entries:
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

        for label, definition in entry.definitions:
            definition_id = database.insert_definition(
                c, definition, label, entry_id, 1, None
            )
            if definition_id == -1:
                logging.error(
                    f"Could not insert definition {definition} for word {entry.traditional}, uh oh!"
                )
                continue

    for sentence in sentences:
        database.insert_chinese_sentence(
            c,
            sentence.traditional,
            sentence.simplified,
            sentence.pinyin,
            sentence.jyutping,
            sentence.language,
            sentence.id,
        )
        # In CantoDict, a sentence ID and its corresponding translation ID are separated by 500000000
        # (See parse_sentence_file())
        database.insert_sentence_link(
            c,
            sentence.id,
            sentence.id + 500000000,
            1,
            True,
        )

    for translation in translations:
        database.insert_nonchinese_sentence(
            c,
            translation.sentence,
            translation.language,
            translation.id,
        )

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_word_file(file_name, words):
    with open(file_name, "r") as file:
        # In pages with latin script, the title messes with BeautifulSoup's HTML parsing
        # So remove the title and replace it with "CantoDict"
        file_text = file.read()
        file_text = TITLE_REGEX_PATTERN.sub("<title>CantoDict</title>", file_text)

        soup = BeautifulSoup(file_text, "html.parser")

        # Extract the traditional and simplified forms
        try:
            forms = [
                x.strip()
                for x in soup.find("td", class_="chinesebig").get_text().split(" / ")
            ]
            if len(forms) > 1:
                trad = forms[0].strip()
                simp = forms[1].strip()
            else:
                trad = forms[0].strip()
                # Cantodict sometimes reports that there is no simplified variant, which is sometimes incorrect
                simp = traditional_to_simplified_converter.convert(trad)
        except:
            # If a character has latin script in it, it may not have a class called "chinesebig"
            try:
                forms = [
                    x.strip()
                    for x in soup.select("span.word.script")[0].get_text().split(" / ")
                ]
                if len(forms) > 1:
                    trad = forms[0].strip()
                    simp = forms[1].strip()
                else:
                    trad = forms[0].strip()
                    # Cantodict sometimes reports that there is no simplified variant, which is sometimes incorrect
                    simp = traditional_to_simplified_converter(trad)
            except:
                logging.error(
                    f"Couldn't find traditional and simplified forms in file {file_name}"
                )
                return

        word = os.path.splitext(os.path.basename(file_name))[0].strip()
        if trad != word:
            if trad == simplified_to_traditional_converter.convert(
                word
            ) or word == traditional_to_simplified_converter.convert(trad):
                logging.debug(
                    f"File name {word} appears to be a simplified variant "
                    f"of {trad}. Ignoring..."
                )
                return
            else:
                if len(trad) == 1:
                    logging.error(
                        f"Hmm, looks like the parsed word {trad} doesn't "
                        f"match the file name {word}. If they are simplified "
                        "or traditional variants of each other, this error "
                        "can be safely ignored."
                    )
                else:
                    logging.error(
                        f"Hmm, looks like the parsed word {trad} doesn't "
                        f"match the file name {word}."
                    )
                return

        freq = zipf_frequency(trad, "zh")

        # Extract the pronunciations
        # CantoDict indicates differences in literary/colloquial pronunciation with *, but we don't support that
        # So remove the stars
        jyut_element = soup.find("span", class_="cardjyutping")
        jyut = jyut_element.get_text() if jyut_element else ""
        jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN.sub("", jyut)
        jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN_VARIANT.sub("\g<2>", jyut)
        jyut = jyut.replace("  ", " ") # Remove double-spaces
        jyut = jyut.strip()

        pin_element = soup.find("span", class_="cardpinyin")
        pin = pin_element.get_text() if pin_element else ""
        # CantoDict also indicates tone sandhi in pinyin with *, but we don't support that either
        pin = LITERARY_PINYIN_READING_REGEX_PATTERN.sub("", pin)
        if not pin:
            pin = " ".join(
                lazy_pinyin(simp, style=Style.TONE3, neutral_tone_with_five=True)
            ).lower()
        # Replace 'v' in Pinyin with the u: that CEDICT uses
        pin = pin.strip().replace("v", "u:")
        # Remove the zero-width spaces that sometimes show up
        pin = pin.replace("​", "")
        pin = pin.replace("  ", " ") # Remove double-spaces

        # CantoDict may have multiple pronunciations for an entry
        # Check for multiple pronunciations in Jyutping
        variant_jyutping = []
        if len(trad) == 1 and len(jyut.split(" ")) > len(trad):
            variant_jyutping = jyut.split(" ")
        elif ";" in jyut:
            variant_jyutping = [x.strip() for x in jyut.split("; ")]

        # Check for differences in PRC and Taiwan pronunciation
        variant_pinyin = []
        for pattern in MANDARIN_PRONUNCIATION_VARIANT_REGEX_PATTERNS:
            match = pattern.match(pin)
            if match:
                # Make sure the matched groups match the length of the characters
                if len(match.group("prc").split(" ")) == len(trad) and len(
                    match.group("tw").split(" ")
                ) == len(trad):
                    variant_pinyin.append((match.group("prc"), match.group("tw")))
        if not variant_pinyin:
            for pattern in MANDARIN_PRONUNCIATION_PARTIAL_VARIANT_REGEX_PATTERNS:
                match = pattern.match(pin)
                if match:
                    prc_pin = pattern.sub(match.group("prc"), pin)
                    tw_pin = pattern.sub(match.group("tw"), pin)
                    # Make sure the found pronunciations match the length of the characters
                    if len(prc_pin.split(" ")) == len(trad) and len(
                        tw_pin.split(" ")
                    ) == len(trad):
                        variant_pinyin.append((prc_pin, tw_pin))
        # Also check for multiple pronunciations of single-character words
        if len(trad) == 1 and len(pin.split(" ")) > len(trad):
            [variant_pinyin.append((x, None)) for x in pin.split(" ")]

        # Some entries give different meanings for different pronunciation;
        # assume they don't but mark True if yes
        variants_handled = False

        # Extract the meaning element
        meaning_element = soup.find("td", class_="wordmeaning")

        # Check for special labels in compound words (brandname, idiom, placename, etc.)
        special_label = ""
        special_pos_elem = meaning_element.find("img", class_="flagicon")
        if special_pos_elem:
            special_label = special_pos_elem["alt"]

        # The layout of compound word pages is different from single-character pages
        real_meaning_element = meaning_element.find("div", class_=None)
        if real_meaning_element:
            meaning_element = real_meaning_element

        # Remove children (these usually contain useless fluff that interfere with definition parsing)
        children = meaning_element.find_all("div")
        children += meaning_element.find_all("span")
        for child in children:
            child.decompose()

        # Parse the meanings from the meaning element
        meanings = []
        # CantoDict puts some weird stuff in the meanings div, and the only way to separate
        # them out is to replace the <br> tags with "\n"
        for br in soup.find_all("br"):
            br.replace_with("\n")

        strings = DEFINITION_SPLITTING_REGEX_PATTERN.split(meaning_element.get_text())
        for string in strings:
            string = string.strip()
            if not string or any([x in string for x in illegal_strings]):
                continue

            continue_parsing = True
            for pattern in JYUTPING_PINYIN_REGEX_PATTERNS:
                result = pattern.search(string)
                if result:
                    if meanings:
                        entry = objects.Entry(
                            trad,
                            simp,
                            pin,
                            jyut,
                            freq=freq,
                            defs=meanings,
                        )
                        words.append(entry)

                    # Then, extract the new pinyin and jyutping
                    # and reset the meanings tuple
                    jyut = result.group(1)
                    jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN.sub("", jyut)
                    jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN_VARIANT.sub(
                        "\g<2>", jyut
                    )
                    pin = result.group(2) if result.group(2) else ""
                    meanings = []
                    continue_parsing = False
                    variants_handled = True
                    break

            if not continue_parsing:
                continue

            for pattern in JYUTPING_ONLY_REGEX_PATTERNS:
                result = pattern.search(string)
                if result:
                    if meanings:
                        entry = objects.Entry(
                            trad,
                            simp,
                            pin,
                            jyut,
                            freq=freq,
                            defs=meanings,
                        )
                        words.append(entry)

                    # Then, extract the new jyutping (but keep the old pinyin!)
                    # and reset the meanings tuple
                    jyut = result.group(1)
                    jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN.sub("", jyut)
                    jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN_VARIANT.sub(
                        "\g<2>", jyut
                    )
                    meanings = []
                    variants_handled = True
                    continue_parsing = False
                    break

            if not continue_parsing:
                continue

            # Try to isolate one or more labels (usually a POS or [華]: indicating Mandarin-only usage or [粵]: indicating Cantonese-only usage)
            labels = []
            definition = string
            result = LABEL_REGEX_PATTERN.search(string)
            if not result:
                # Filter out bad non-standard strings that are completely enclosed in square braces
                if string[0] == "[" and string[-1] == "]":
                    continue

            while result:
                labels.extend(result.group(1).strip().split(","))
                string = LABEL_REGEX_PATTERN.sub("", string)
                result = LABEL_REGEX_PATTERN.search(string)

            # At this point, all the labels enclosed in square braces (possibly followed by whitespace)
            # should be stripped out of the beginning of the string.
            # Therefore, we can now assume the contents of the string are the definition
            definition = string
            if not definition:
                continue

            # Override black trying to add a trailing comma here
            # fmt: off
            labels = map(
                lambda x: pos_labels[x.lower()] if x.lower() in pos_labels else x,
                labels
            )
            # fmt: on
            label = ", ".join(labels)
            if not label and special_label:
                label = special_label

            meanings.append((label, definition))

        if meanings:
            entry = objects.Entry(trad, simp, pin, jyut, freq=freq, defs=meanings)

            if not variant_jyutping and not variant_pinyin:
                words.append(entry)

            elif not variants_handled:
                for jyutping in variant_jyutping:
                    variant_entry = copy.deepcopy(entry)
                    variant_entry.add_jyutping(jyutping)
                    words.append(variant_entry)

                for prc, tw in variant_pinyin:
                    prc_variant = copy.deepcopy(entry)
                    prc_variant.add_pinyin(prc)
                    words.append(prc_variant)

                    if tw:
                        tw_variant = copy.deepcopy(entry)
                        tw_variant.add_pinyin(tw)
                        words.append(tw_variant)


def parse_sentence_file(file_name, sentences, translations):
    with open(file_name, "r") as file:
        soup = BeautifulSoup(file, "html.parser")

        # Get the sentence ID
        # We add 2000000000 to the ID, because 0-999999999 are reserved for Tatoebaa
        # and 1000000000-1999999999 are reserved for words.hk
        link_element = soup.find("div", class_="wd_code_links")
        if link_element:
            result = SENTENCE_ID_PATTERN.search(link_element.get_text())
            if result:
                sentence_id = int(result.group(1)) + 2000000000
        else:
            logging.error(f"Couldn't find the sentence ID in file {file_name}")
            return

        # Get the sentence
        try:
            trad = soup.find("span", class_="sentence").get_text()
            simp = traditional_to_simplified_converter.convert(trad)
        except:
            logging.error(f"Couldn't find sentence in file {file_name}")
            return

        # Find romanizations
        jyut_element = soup.find("span", class_="cardjyutping")
        jyut = jyut_element.get_text() if jyut_element else ""
        jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN.sub("", jyut)
        jyut = LITERARY_CANTONESE_READING_REGEX_PATTERN_VARIANT.sub("\g<2>", jyut)
        jyut = jyut.strip()

        pin_element = soup.find("span", class_="cardpinyin")
        pin = pin_element.get_text() if pin_element else ""
        pin = pin.strip().replace("v", "u:")

        # Find the language this sentence is in
        lang = "zho"
        meaning_element = soup.find("td", class_="wordmeaning")
        cantonese_element = meaning_element.find("span", class_="cantonesebox")
        if cantonese_element:
            lang = "yue"
        mandarin_element = meaning_element.find("span", class_="mandarinbox")
        if mandarin_element:
            lang = "cmn"

        sentence = objects.ChineseSentence(
            sentence_id,
            trad,
            simp,
            pin,
            jyut,
            lang,
        )
        sentences.append(sentence)

        # Find the translation of this sentence
        translation_element = soup.find("div", class_="audioplayer")

        # Remove children (these usually contain useless fluff that interfere with definition parsing)
        children = translation_element.find_all("a")
        children += translation_element.find_all("center")
        for child in children:
            child.decompose()

        if translation_element:
            translation = translation_element.get_text().strip()
            sentence_translation = objects.NonChineseSentence(
                500000000 + sentence_id, translation, "eng"
            )
            translations.append(sentence_translation)


def parse_words_folder(folder_name, words):
    for index, entry in enumerate(os.scandir(folder_name)):
        if not index % 100:
            print(f"Parsed word #{index}")
        if entry.is_file() and entry.path.endswith(".html"):
            parse_word_file(entry.path, words)


def parse_sentences_folder(folder_name, sentences, translations):
    for index, entry in enumerate(os.scandir(folder_name)):
        if not index % 100:
            print(f"Parsed sentence #{index}")
        if entry.is_file() and entry.path.endswith(".html"):
            parse_sentence_file(entry.path, sentences, translations)


if __name__ == "__main__":
    if len(sys.argv) != 13:
        print(
            (
                "Usage: python3 -m cantodict.parse <database filename> "
                "<scraped characters + compound words HTML folder> "
                "<scraped sentences HTML folder> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other> "
                "<logging output filepath>"
            )
        )
        print(
            (
                "e.g. python3 -m cantodict.parse cantodict.db ./cantodict/scraped_words/ "
                "./cantodict/scraped_sentences/ CantoDict CD 2021-07-18 "
                '"CantoDict is a collaborative Chinese Dictionary project started in November 2003. '
                'Entries are added and mistakes corrected by a team of kind volunteers from around the world." '
                '"https://www.cantonese.sheik.co.uk/copyright.htm" "https://www.cantonese.sheik.co.uk/" "" "" '
                "./cantodict/logging.log"
            )
        )
        sys.exit(1)

    source = objects.SourceTuple(
        sys.argv[4],
        sys.argv[5],
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10],
        sys.argv[11],
    )

    cc_cedict.load()

    logging.basicConfig(filename=sys.argv[12], level=logging.INFO)

    words = []
    sentences = []
    translations = []
    parse_words_folder(sys.argv[2], words)
    parse_sentences_folder(sys.argv[3], sentences, translations)
    write(sys.argv[1], source, words, sentences, translations)
