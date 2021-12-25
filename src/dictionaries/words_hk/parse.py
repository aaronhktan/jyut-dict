from bs4 import BeautifulSoup
from hanziconv import HanziConv
import hanzidentifier
import jieba
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict, namedtuple
import logging
import os
import sqlite3
import sys

# NOTE: This parser will will only parse words.hk pages saved before the 2021 redesign!

# Structure of entries: a WORD contains multiple MEANINGS
# A MEANING contains multiple DEFINITIONS in different languages, as well
# as EXAMPLES
DefinitionTuple = namedtuple("DefinitionTuple", ["lang", "content"])
MeaningTuple = namedtuple(
    "Meaning", ["label", "definitions", "examplephrases", "examplesentences"]
)
WordTuple = namedtuple("Word", ["word", "pronunciation", "meanings"])

# Useful test pages:
#   - 脂粉客 for malformed entry (or old entry)
#   - 了 for multiple meanings, multiple examples
#   - 一戙都冇 for multiple header words
#   - 是 for item with label and POS
#   - 印 for item with different POS for different meanings
#   - 使勁 for broken sentence
#   - 鬼靈精怪 for multiple different pronunciations
#   - 掌 and 撻 for different definitions (腳掌, 撻沙) with the same English translation (sole)


def insert_example(c, definition_id, starting_example_id, example):
    # The example should be a list of ExampleTuples, such that
    # the first item is the 'source', and all subsequent ones are the
    # translations
    examples_inserted = 0

    trad = example[0].content
    simp = HanziConv.toSimplified(trad)
    jyut = example[0].pron
    pin = ""
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
        sentence = translation.content
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
    # Because the sentence id is presumed to be unique by Tatoeba, we will give it
    # a namespace of 999999999 potential sentences. Thus, words.hk sentences will start
    # at rowid 1000000000.
    example_id = 1000000000

    for key in words:
        for entry in words[key]:
            trad = entry.word
            simp = HanziConv.toSimplified(trad)
            jyut = entry.pronunciation
            pin = (
                " ".join(
                    lazy_pinyin(trad, style=Style.TONE3, neutral_tone_with_five=True)
                )
                .lower()
                .replace("v", "u:")
            )
            freq = zipf_frequency(trad, "zh")

            entry_id = database.get_entry_id(c, trad, simp, pin, jyut, freq)

            if entry_id == -1:
                entry_id = database.insert_entry(c, trad, simp, pin, jyut, freq, None)
                if entry_id == -1:
                    logging.error(f"Could not insert word {trad}, uh oh!")
                    continue

            # Insert each meaning for the entry
            for meaning in entry.meanings:
                definitions = []
                for definition in meaning.definitions:
                    # Insert a zero-width space between Chinese words so that
                    # fts5 properly indexes them
                    is_chinese = hanzidentifier.is_simplified(
                        definition.content
                    ) or hanzidentifier.is_traditional(definition.content)
                    if is_chinese:
                        definitions.append("​".join(jieba.cut(definition.content)))
                    else:
                        definitions.append(definition.content)
                definition = "\r\n".join(definitions)

                definition_id = database.insert_definition(
                    c, definition, meaning.label, entry_id, 1, None
                )
                if definition_id == -1:
                    logging.error(
                        f"Could not insert definition {definition} for word {trad}, uh oh!"
                    )
                    continue

                # Insert examples for each meaning
                for sentence in meaning.examplesentences:
                    examples_inserted = insert_example(
                        c, definition_id, example_id, sentence
                    )
                    example_id += examples_inserted

                for phrase in meaning.examplephrases:
                    examples_inserted = insert_example(
                        c, definition_id, example_id, phrase
                    )
                    example_id += examples_inserted


def write(db_name, source, words):
    print("Writing to database file")

    db = sqlite3.connect(db_name)
    c = db.cursor()

    database.write_database_version(c)

    database.drop_tables(c)
    database.create_tables(c)

    # Add source information to table
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

    insert_words(c, words)

    database.generate_indices(c)

    db.commit()
    db.close()


def parse_definitions(def_elem):
    try:
        # Extract the language of this definition, and then remove it from the
        # tree
        lang_tag = def_elem.find("span", class_="zi-item-lang", recursive=False)
        lang = lang_tag.text.strip("()")
        lang_tag.decompose()
    except BaseException:
        lang = "yue"  # Assume Cantonese definition if no lang tag found

    # Get the textual content of the definition
    content = def_elem.text.strip()

    return DefinitionTuple(lang, content)


def parse_examples(ex_elem, classname):
    ex = []
    ex_divs = ex_elem.find_all("div", class_=None, recursive=False)
    for ex_div in ex_divs:
        lang_tag = ex_div.find("span", class_="zi-item-lang")
        lang = lang_tag.text.strip("()")
        lang_tag.decompose()

        pronunciation_tag = ex_div.find("span", class_=classname)
        if pronunciation_tag:
            pronunciation = pronunciation_tag.text.strip("()")
            pronunciation_tag.decompose()
        else:
            pronunciation = ""

        content = ex_div.text.strip()
        ex.append(objects.ExampleTuple(lang, pronunciation, content))
    return ex


def log_word(word):
    # Logging for debug purposes
    logging.debug(" ".join([word.word, word.pronunciation]))
    for index, meaning in enumerate(word.meanings):
        logging.debug(f"Meaning {index}:")
        for definition in meaning.definitions:
            logging.debug(
                " ".join(["\tDefinition:", definition.lang, definition.content])
            )
        for i, phrases in enumerate(meaning.examplephrases):
            logging.debug(f"\t\tPhrase {i}:")
            for phrase in phrases:
                logging.debug(
                    " ".join(
                        ["\t\t\tPhrase: ", phrase.lang, phrase.content, phrase.pron]
                    )
                )
        for i, sentences in enumerate(meaning.examplesentences):
            logging.debug(f"\t\tSentence {i}:")
            for sentence in sentences:
                logging.debug(
                    " ".join(
                        [
                            "\t\t\tSentence:",
                            sentence.lang,
                            sentence.content,
                            sentence.pron,
                        ]
                    )
                )


def parse_file(file_name, words):
    with open(file_name, "r") as file:
        soup = BeautifulSoup(file, "html.parser")
        drafts = soup.find_all("div", class_="draft-version")

        # Each separate entry for a particular word is contained in a div with
        # a class called "draft-version"
        for draft in drafts:
            word, _ = os.path.splitext(os.path.basename(file_name))

            # In words with multiple written forms or pronunciations,
            # try to parse the correct one
            word_pronunciation = ""
            written_forms = draft.find("tr", class_="zi-written-forms")
            word_pronunciations = written_forms.find_all("li", class_=None)
            if word_pronunciations:
                for item in word_pronunciations:
                    corresponding_word = "".join(
                        item.find_all(text=True, recursive=False)
                    ).strip()
                    if corresponding_word == word:
                        word_pronunciation = item.find(
                            "span", class_="zi-pronunciation"
                        ).text
                        break

            # If not able to find a corresponding pronunciation or there is only one,
            # take the first one that exists in this entry
            if not word_pronunciation:
                word_pronunciation = draft.find("span", class_="zi-pronunciation").text

            # The POS tag is in a class labelled zidin-pos
            # Each draft has definitions for only one POS label, so if an entry like 印 is
            # both a noun and a verb, words.hk separates it into two drafts.
            pos_elem = draft.find("tr", class_="zidin-pos")
            if pos_elem:
                pos = pos_elem.find("span", class_=None).get_text()
            else:
                pos = ""

            meanings = []
            # This will find the table row containing meaning if there are multiple meanings
            # If there are multiple meanings, they will be in an ordered list,
            # so extract every item in the list.
            try:
                list_items = draft.find("tr", class_="zidin-explanation").find_all("li")
                # This will find the table row containing meanings if there is
                # a single meaning
                if not list_items:
                    list_items = [
                        draft.find("tr", class_="zidin-explanation").find_all("td")[1]
                    ]
            except BaseException:
                # If there is no zidin-explanation class, it might be an old page
                # These do not contain good formatting, so just stick it into
                # the list and call it a day
                try:
                    text = draft.find("li", class_=None).text
                    definition = DefinitionTuple(
                        "yue", text
                    )  # Assume definition is in Cantonese
                    meanings.append(MeaningTuple("", [definition], [], []))
                    words[word].append(WordTuple(word, word_pronunciation, meanings))
                    continue
                except BaseException:
                    # Malformed page, give up
                    logging.warning(f"Failed to parse for item {word}")
                    continue

            logging.info(f"Parsing item {word}")
            for list_item in list_items:
                meaning = MeaningTuple(pos, [], [], [])

                # Each definition for one meaning is contained in a classless
                # div
                def_divs = list_item.find_all("div", class_=None, recursive=False)
                for def_div in def_divs:
                    meaning.definitions.append(parse_definitions(def_div))

                # Each example phrase for the definition is contained in a div
                # with class zi-details-phrase-item
                exphr_elems = list_item.find_all(
                    "div", class_="zi-details-phrase-item", recursive=False
                )
                for exphr_elem in exphr_elems:
                    meaning.examplephrases.append(
                        parse_examples(exphr_elem, "zi-item-phrase-pronunciation")
                    )

                # Each example sentence for the definition is contained in a
                # div with class zi-details-example-item
                exsen_elems = list_item.find_all(
                    "div", class_="zi-details-example-item", recursive=False
                )
                for exsen_elem in exsen_elems:
                    meaning.examplesentences.append(
                        parse_examples(exsen_elem, "zi-item-example-pronunciation")
                    )

                meanings.append(meaning)

            word_tuple = WordTuple(word, word_pronunciation, meanings)
            log_word(word_tuple)

            words[word].append(word_tuple)


def parse_folder(folder_name, words):
    for index, entry in enumerate(os.scandir(folder_name)):
        if not index % 100:
            print(f"Parsed word #{index}")
        if entry.is_file() and entry.path.endswith(".html"):
            parse_file(entry.path, words)


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<HTML folder> <source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 script.py words_hk.db scraped/data/ 粵典–words.hk WHK 2020-07-14 "
                '"《粵典》係一個大型嘅粵語辭典計劃。我哋會用Crowd-sourcing嘅方法，整一本大型、可持續發展嘅粵語辭典。" '
                '"https://words.hk/base/hoifong/" "https://words.hk/" "" ""'
            )
        )
        sys.exit(1)

    cc_cedict.load()

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
    # logging.basicConfig(level='DEBUG') # Uncomment to enable debug logging
    parsed_words = defaultdict(list)
    parse_folder(sys.argv[2], parsed_words)
    write(sys.argv[1], source, parsed_words)
