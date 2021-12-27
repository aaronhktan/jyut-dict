from hanziconv import HanziConv
import jieba
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict, namedtuple
import copy
import csv
from itertools import chain
import logging
import re
import sqlite3
import sys
import traceback

# Useful test pages:
#   - 93076 (脂粉客) for malformed entry (or old entry)
#   - 55282 (了) for multiple meanings, multiple examples
#   - 95881 (一戙都冇) for multiple header words
#   - 84279 (是) for item with label and POS
#   - 51495 (印) for item with different POS for different meanings
#   - 84152 (使勁) for broken sentence
#   - 73697 (鬼靈精怪) for variant with different pronunciation
#   - 65669 (掌) and 111487 (撻) for different definitions (腳掌, 撻沙) with the same English translation (sole)
#   - 5026 (滑坡): contains explanation in mandarin
#   - 44110 (單打): multiple synonyms
#   - 98534 (套): empty example
#   - 89764 (奔馳): does not split up definition from metadata with <explanation>
#   - 98809 (Benz): variants
#   - 71732 (複製): example with space in it
#   - 11369 (中國): multiple explanations that are not separated by <explanation>, contains explanation in japanese
#   - 114718 (staff): non-standard Jyutping
#   - 97459 (緡): multiple pronunciations
#   - 62089 (投): no pronunciation

PART_OF_SPEECH_REGEX = re.compile(r"\(pos:(.*?)\)")
LABEL_REGEX = re.compile(r"\(label:(.*?)\)")
NEAR_SYNONYM_REGEX = re.compile(r"\(sim:(.*?)\)")
ANTONYM_REGEX = re.compile(r"\(ant:(.*?)\)")


def read_csv(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            yield row


def insert_example(c, definition_id, starting_example_id, example):
    # The example should be a list of Example objects, such that
    # the first item is the 'source', and all subsequent items are the
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
            trad = entry.traditional
            simp = entry.simplified
            jyut = entry.jyutping
            pin = entry.pinyin
            freq = entry.freq

            entry_id = database.get_entry_id(c, trad, simp, pin, jyut, freq)

            if entry_id == -1:
                entry_id = database.insert_entry(c, trad, simp, pin, jyut, freq, None)
                if entry_id == -1:
                    logging.warning(f"Could not insert word {trad}, uh oh!")
                    continue

            # Insert each meaning for the entry
            for definition in entry.definitions:
                definition_id = database.insert_definition(
                    c, definition.definition, definition.label, entry_id, 1, None
                )
                if definition_id == -1:
                    logging.warning(
                        f"Could not insert definition {definition} for word {trad}, uh oh!"
                    )
                    continue

                # Insert examples for each meaning
                for example in definition.examples:
                    examples_inserted = insert_example(
                        c, definition_id, example_id, example
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


def process_entry(line):
    entries = []

    # Parse the entry header
    header = line[1].strip('"')
    variants = header.split(",")

    for variant in variants:
        trad = variant.split(":")[0]
        simp = HanziConv.toSimplified(trad)
        pin = (
            " ".join(lazy_pinyin(trad, style=Style.TONE3, neutral_tone_with_five=True))
            .lower()
            .replace("v", "u:")
        )
        freq = zipf_frequency(trad, "zh")

        # Parse all the Jyutping
        pronunciations = variant.split(":")[1:]
        if not pronunciations:
            entries.append(
                objects.Entry(trad=trad, simp=simp, jyut="", pin=pin, freq=freq)
            )
        else:
            for jyut in pronunciations:
                # Non-standard Jyutping starts with an exclamation mark, e.g. !sdet1 or !sdaaf1
                # Remove the exclamation mark
                if jyut.startswith("!"):
                    jyut = jyut[1:]

                entries.append(
                    objects.Entry(trad=trad, simp=simp, jyut=jyut, pin=pin, freq=freq)
                )

    # Parse the entry content: explanations, examples
    content = line[2]
    if content.startswith("未有內容"):
        for entry in entries:
            entry.append_to_defs(objects.Definition(definition="x"))
        return entries

    definitions = []
    entry_labels = []
    near_synonyms = []
    antonyms = []

    # Explanations are separated by both '<explanation>' tags and '----' tags
    explanations = content.split("<explanation>")
    explanations = map(lambda x: x.split("----"), explanations)
    explanations = list(chain.from_iterable(explanations))

    for explanation_index, explanation in enumerate(explanations):
        if not explanation.strip():
            continue

        parse_explanation = True

        if explanation_index == 0:
            # The first item contains metadata about the entry
            parse_explanation = False

            for x in re.findall(PART_OF_SPEECH_REGEX, explanation):
                entry_labels.append(x)
            for x in re.findall(LABEL_REGEX, explanation):
                entry_labels.append(x)
            for x in re.findall(NEAR_SYNONYM_REGEX, explanation):
                near_synonyms.append(x)
            for x in re.findall(ANTONYM_REGEX, explanation):
                antonyms.append(x)

            # However, for some items, such as id 89764, the first item also contains the explanation
            if explanation.find("yue:") != -1:
                # fmt: off
                explanation = explanation[explanation.find("yue:")+1:]
                # fmt: on
                parse_explanation = True

        if parse_explanation:
            definition = objects.Definition(label="、".join(entry_labels), examples=[])

            # Subsequent items contain explanations
            for index, item in enumerate(explanation.split("<eg>")):
                if index == 0:
                    # The first item contains the explanation
                    # Translations in different languages are separated by newlines
                    explanation_translations = item.strip().split("\n")
                    # Strip out links
                    explanation_translations = map(
                        lambda x: x.replace("#", ""), explanation_translations
                    )
                    # fmt: off
                    # Segment the Chinese explanations so they show up in the FTS index 
                    explanation_translations = map(
                        lambda x: (x[:x.find(":")], x[x.find(":")+1:]),
                        explanation_translations
                    )
                    explanation_translations = map(
                        lambda x: (
                            "​".join(jieba.cut(x[1]))
                            if x[0] in ("yue", "zho")
                            else x[1]
                        ),
                        explanation_translations
                    )
                    # fmt: on
                    explanation = "\n".join(explanation_translations)
                    definition.definition = explanation
                else:
                    # Subsequent items contain examples for this explanation
                    definition.examples.append([])
                    example_translations = item.strip().split("\n")
                    # fmt: off
                    # Strip out links
                    example_translations = map(
                        lambda x: x.replace("#", ""), example_translations
                    )
                    # fmt: on
                    for translation in example_translations:
                        if not translation or translation == "----":
                            # Ignore lines that are not translations
                            continue

                        # fmt: off
                        lang = translation[:translation.find(":")]
                        if lang in ("yue", "zho"):
                            # Example content ends before the first space with an opening parenthesis after it 
                            # (which indicates the start of a romanization)
                            # but some example don't have romanization, so filter for that
                            if translation.find(" (") >= 0:
                                content = translation[translation.find(":")+1:translation.find(" (")]
                            else:
                                content = translation[translation.find(":")+1:]
                            pron = (
                                translation[translation.find(" (")+1:].strip("()")
                                if len(translation.split()) >= 2
                                else ""
                            )
                            if lang == "yue":
                                definition.examples[-1].insert(
                                    0,
                                    objects.Example(lang=lang, pron=pron, content=content)
                                )
                            else:
                                definition.examples[-1].append(
                                    objects.Example(lang=lang, content=content)
                                )
                        else:
                            content = translation[translation.find(":")+1:]
                            if not content:
                                content = "x"
                            definition.examples[-1].append(
                                objects.Example(lang=lang, content=content)
                            )
                        # fmt: on

            definitions.append(definition)

    # Add synonyms, antonyms to list of definitions
    if near_synonyms:
        definitions.append(
            objects.Definition(
                definition="、".join(near_synonyms), label="近義詞", examples=[]
            )
        )
    if antonyms:
        definitions.append(
            objects.Definition(definition="、".join(antonyms), label="反義詞", examples=[])
        )

    # Assign definitions to each entry
    for entry in entries:
        entry.add_defs(copy.deepcopy(definitions))

    # Add variants to the definitions of an entry; these are unique for each entry
    if len(variants) > 1:
        variants = set(map(lambda x: x.split(":")[0], variants))
        for entry in entries:
            # Do not add variants whose Chinese characters match the current entry's characters into the current entry's "see also" section
            filtered_variants = filter(lambda x: x != entry.traditional, variants)
            entry.append_to_defs(
                objects.Definition(
                    definition="、".join(filtered_variants),
                    label="參看",
                    examples=[],
                )
            )

    return entries


def parse_file(filename, words):
    index = 0
    for line in read_csv(filename):
        # Ignore first and second lines
        if (len(line) > 1 and not line[0] and not line[1]) or (not line[0]):
            index += 1
            continue

        if not index % 500:
            print(f"Parsed entry #{index}")

        entries = process_entry(line)
        for current_entry in entries:
            words[current_entry.traditional].append(current_entry)

        index += 1


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<all.csv file> <source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m words_hk.parse words_hk.db words_hk/data/all.csv 粵典–words.hk WHK 2021-12-23 "
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
    parse_file(sys.argv[2], parsed_words)
    write(sys.argv[1], source, parsed_words)
