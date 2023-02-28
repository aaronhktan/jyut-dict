import opencc
from pypinyin import lazy_pinyin, Style
from pypinyin_dict.phrase_pinyin_data import cc_cedict
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict
import copy
from enum import Enum
import logging
import re
import sqlite3
import string
import sys
import unicodedata

# Useful test entries:
# - 三少: has two parts of speech


IGNORED_LINES = ("cidian.wenlindb\n", ".-arc\n", ".-publish\n")
IGNORED_TYPES = (
    "gr",
    "ref",
    "rem",
    "ser",
)
EXAMPLE_TYPE = re.compile(
    r"(?P<pos_index>\d{1})(?P<def_index>\d{1})?(?P<ex_index>\d{1})?(?P<type>\w*)(@\w*)*"
)
VARIANTS = re.compile(r"((?:./)+.)")
ABRIDGED_DATED_VARIANT = re.compile(r"@{.*?}")
LITERARY_CANTONESE_READING_REGEX_PATTERN = re.compile(r"\d\/")

converter = opencc.OpenCC("hk2s.json")


class Type(Enum):
    NONE = 0
    IGNORED = 1
    ERROR = 2
    FINISHED_ENTRY = 3

    JYUTPING = 11
    HANZI = 12

    POS = 21
    POSX = 22
    DEFINITION = 23

    EXAMPLE_JYUTPING = 31
    EXAMPLE_HANZI = 32
    EXAMPLE_TRANSLATION = 33

    SMUSHED = 41


def insert_example(c, definition_id, starting_example_id, example):
    # The example should be a list of Example objects, such that
    # the first item is the 'source', and all subsequent items are the
    # translations
    examples_inserted = 0

    trad = example[0].content
    simp = converter.convert(trad) if trad else ""
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
    # Reserved sentence IDs:
    #   - 0-999999999: Tatoeba
    #   - 1000000000-1999999999: words.hk
    #   - 2000000000-2999999999: CantoDict
    #   - 3000000000-3999999999: MoEDict
    #   - 4000000000-4999999999: Cross-Straits Dictionary
    #   - 5000000000-5999999999: ABC Chinese-English Dictionary
    #   - 6000000000-6999999999: ABC Cantonese-English Dictionary
    example_id = 6000000000

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
                    # Try to find definition if we got an error
                    definition_id = database.get_definition_id(
                        c, definition.definition, definition.label, entry_id, 1
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


def parse_jyutping(content):
    # Remove literary pronunciations, which ABY indicates with "/"
    content = LITERARY_CANTONESE_READING_REGEX_PATTERN.sub("", content)
    # Replace combining characters with single-character equivalents
    content = unicodedata.normalize("NFKC", content)
    # Make the Jyutping lowercase
    content = content.lower()

    return content


def parse_char(content):
    content_variants = []

    # Remove dated variants, if any (denoted by @{<variant>})
    content = ABRIDGED_DATED_VARIANT.sub("", content)
    # Remove "@@" from characters
    content = content.replace("@@", "")

    # Characters may have one or more variants
    traditional = content

    traditional_variants = []
    match = VARIANTS.search(traditional)
    if match:
        variants = match.group(1).split("/")
        for variant in variants:
            traditional_variants.append(VARIANTS.sub(variant, traditional))
    else:
        traditional_variants.append(traditional)

    for traditional_variant in traditional_variants:
        simplified_variant = converter.convert(traditional_variant)
        content_variants.append((traditional_variant, simplified_variant))

    return content_variants


def parse_definition(content):
    # Assume translation is in English by default
    lang = "en"

    # If there are multiple translations for an example in different languages, they will
    # start with the language tag enclosed in square brackets, e.g. [en]
    if content.startswith("["):
        # fmt: off
        lang = content[content.find("[")+1:content.find("]")]
        content = content[content.find("]")+2:]
        # fmt: on

    # I'm not sure why, but there is sometimes an "@@" in the definition content.
    # Since it's not useful, remove it.
    content = content.replace("@@", "")

    return lang, content


def parse_example_jyutping(content, entry_jyutping):
    # Remove literary pronunciations, which ABY indicates with "/"
    content = LITERARY_CANTONESE_READING_REGEX_PATTERN.sub("", content)
    # Replace combining characters with single-character equivalents
    content = unicodedata.normalize("NFKC", content)
    # Make the Jyutping lowercase
    content = content.lower()

    return content


def parse_example_hanzi(content, trad):
    # Currently a no-op, examples do not use ~ like the ABC Chinese-English dictionary
    return content


def parse_example_translation(content):
    # Assume translation is in English by default
    lang = "en"

    # If there are multiple translations for an example in different languages, they will
    # start with the language tag enclosed in square brackets, e.g. [en]
    if content.startswith("["):
        # fmt: off
        lang = content[content.find("[")+1:content.find("]")]
        content = content[content.find("]")+2:]
        # fmt: on

    return lang, content


def parse_content(column_type, content, entry):
    if column_type.startswith(".hw"):
        jyut = parse_jyutping(content)
        return Type.JYUTPING, jyut
    elif column_type.startswith("char"):
        variants = parse_char(content)
        return Type.HANZI, variants
    elif column_type.startswith("psx"):
        # not sure exactly what psx actually means
        lang, part_of_speech_extended = parse_definition(content)
        return Type.POSX, (lang, part_of_speech_extended)
    elif column_type.startswith("ps"):
        part_of_speech = content.replace("@@", "")
        return Type.POS, part_of_speech
    elif column_type.startswith("df"):
        language, definition = parse_definition(content)
        return Type.DEFINITION, (language, definition)
    elif column_type.startswith("ex"):
        if not entry:
            return Type.ERROR, None
        example_jyutping = parse_example_jyutping(content, entry.jyutping)
        return Type.EXAMPLE_JYUTPING, example_jyutping
    elif column_type.startswith("hz"):
        if not entry:
            return Type.ERROR, None
        example_hanzi = parse_example_hanzi(content, entry.traditional)
        return Type.EXAMPLE_HANZI, example_hanzi
    elif column_type.startswith("tr"):
        language, translation = parse_example_translation(content)
        return Type.EXAMPLE_TRANSLATION, (language, translation)
    elif column_type[0].isdigit():
        # In entries with multiple parts of speech and definitions per part of speech,
        # part-of-speech number, definition number, example number, and type are smushed together
        # e.g. "312hz": part-of-speech #3, definition #1 of pos #3, example #2 for definition #1, type = hanzi
        if EXAMPLE_TYPE.search(column_type):
            match = EXAMPLE_TYPE.match(column_type).groupdict()
            parsed = parse_content(match["type"], content, entry)
            return Type.SMUSHED, {
                "pos_index": int(match["pos_index"]) if match["pos_index"] else None,
                "def_index": int(match["def_index"]) if match["def_index"] else None,
                "ex_index": int(match["ex_index"]) if match["ex_index"] else None,
                "parsed": parsed,
            }
    else:
        return Type.ERROR, None


def parse_line(line, entry):
    if not line or line in IGNORED_LINES:
        return Type.NONE, None

    if line == "\n":
        return Type.FINISHED_ENTRY, None

    line = line.split("   ")
    if len(line) >= 2:
        try:
            column_type, content = line[0], line[1]
        except Exception as e:
            logging.error(f"Couldn't parse line: '{line}'")
            return Type.ERROR, None
    else:
        return Type.IGNORED, None

    content = content.strip()

    return parse_content(column_type, content, entry)


def parse_file(filename, words):
    current_entry = None

    current_entry_lines = []
    counter = 0

    with open(filename) as file:
        for line in file.readlines():
            parsed_type, parsed = parse_line(line, current_entry)

            if parsed_type in (Type.NONE, Type.IGNORED, Type.ERROR):
                continue

            elif parsed_type != Type.FINISHED_ENTRY:
                current_entry_lines.append(line)

            else:
                counter += 1
                if not counter % 500:
                    print(f"Processed {counter} entries!")

                if not current_entry_lines:
                    continue

                current_entry = objects.Entry()
                variants = []
                entry_pos = None
                entry_posx = None
                entry_ex_hz = None
                entry_ex_jyut = None
                entry_ex_tr = None

                # First, parse all the lines that relate to the entirety of the entry
                # These lines do not have a number preceding them.
                entry_lines = list(
                    filter(lambda x: not (x[0].isdigit()), current_entry_lines)
                )
                parsed_entry_lines = list(
                    map(parse_line, entry_lines, [current_entry] * len(entry_lines))
                )

                for parsed_type, parsed in parsed_entry_lines:
                    if parsed_type == Type.JYUTPING:
                        current_entry.add_jyutping(parsed)
                    elif parsed_type == Type.HANZI:
                        current_entry.add_traditional(parsed[0][0])
                        current_entry.add_simplified(parsed[0][1])
                        current_entry.add_freq(zipf_frequency(parsed[0][0], "zh"))
                        current_entry.add_pinyin(
                            " ".join(
                                lazy_pinyin(
                                    parsed[0][1],
                                    style=Style.TONE3,
                                    neutral_tone_with_five=True,
                                )
                            )
                            .lower()
                            .replace("v", "u:")
                        )
                        variants = parsed[1:]
                    elif parsed_type == Type.POS:
                        entry_pos = parsed
                    elif parsed_type == Type.POSX:
                        lang, posx = parsed

                        if lang not in (None, "en"):
                            continue

                        entry_posx = "(" + posx + ")"
                    elif parsed_type == Type.DEFINITION:
                        lang, definition = parsed

                        if lang not in (None, "en"):
                            continue

                        # Add the psx label to the definition if there is one
                        definition = (
                            entry_posx + " " + definition if entry_posx else definition
                        )
                        label = entry_pos if entry_pos else ""

                        current_definition = objects.Definition(
                            definition=definition, label=label
                        )
                        current_entry.append_to_defs(current_definition)

                parsed_entry_lines = list(
                    map(parse_line, entry_lines, [current_entry] * len(entry_lines))
                )

                for parsed_type, parsed in parsed_entry_lines:
                    if parsed_type == Type.EXAMPLE_HANZI:
                        entry_ex_hz = parsed
                    elif parsed_type == Type.EXAMPLE_JYUTPING:
                        entry_ex_jyut = parsed
                    elif parsed_type == Type.EXAMPLE_TRANSLATION:
                        if parsed[0] != "en":
                            continue

                        entry_ex_tr = parsed[1]

                if entry_ex_jyut or entry_ex_hz or entry_ex_tr:
                    current_definition.examples.append([])
                    current_definition.examples[-1].append(
                        objects.Example(
                            lang="yue", pron=entry_ex_jyut, content=entry_ex_hz
                        )
                    )
                    current_definition.examples[-1].append(
                        objects.Example(lang="eng", content=entry_ex_tr)
                    )

                # Then, parse each of the numbered lines
                numbered_lines = list(
                    filter(lambda x: x[0].isdigit(), current_entry_lines)
                )
                parsed_numbered_lines = list(
                    map(
                        parse_line,
                        numbered_lines,
                        [current_entry] * len(numbered_lines),
                    )
                )
                parsed_numbered_lines = list(
                    filter(lambda x: x[0] == Type.SMUSHED, parsed_numbered_lines)
                )
                parsed_numbered_lines = list(map(lambda x: x[1], parsed_numbered_lines))

                for pos_index in range(1, 7):
                    # In the ABC Cantonese dictionary, there is a maximum of six parts of speech (1-indexed)
                    pos_index_lines = list(
                        filter(
                            lambda x: x["pos_index"] == pos_index, parsed_numbered_lines
                        )
                    )

                    # First, let's isolate lines that apply to all lines in this pos_index:
                    # do this by finding lines where the pos_index is some number, def_index == None, and ex_index == None
                    # e.g. "1ps   n." => applies to "11df   greeting", as well as "12df   salutation"
                    applies_to_all_in_this_pos_index = list(
                        filter(
                            lambda x: x["def_index"] == None and x["ex_index"] == None,
                            pos_index_lines,
                        )
                    )

                    # Parse the part of speech that applies to all lines in this index
                    pos_index_pos_line = list(
                        filter(
                            lambda x: x["parsed"] and x["parsed"][0] == Type.POS,
                            applies_to_all_in_this_pos_index,
                        )
                    )
                    pos_index_pos_line = list(
                        map(lambda x: x["parsed"], pos_index_pos_line)
                    )

                    pos_index_pos = ""
                    if len(pos_index_pos_line) > 1:
                        logging.error(
                            f"Found more than one part of speech for index {pos_index} in entry {current_entry.traditional}"
                        )
                        logging.info(pos_index_pos_line)
                        pos_index_pos = " / ".join([x[1] for x in pos_index_pos_line])
                    elif len(pos_index_pos_line) == 1:
                        pos_index_pos = pos_index_pos_line[0][1]

                    # Parse the definitions that apply to all lines in this index
                    pos_index_def_lines = list(
                        filter(
                            lambda x: x["parsed"]
                            and x["parsed"][0] in (Type.POSX, Type.DEFINITION),
                            applies_to_all_in_this_pos_index,
                        )
                    )
                    pos_index_def_lines = list(
                        filter(lambda x: x["parsed"][1][0] == "en", pos_index_def_lines)
                    )
                    pos_index_def_lines = list(
                        map(lambda x: x["parsed"], pos_index_def_lines)
                    )

                    posx = definition = ""
                    definition_list = []
                    for content_type, content in pos_index_def_lines:
                        if content_type == Type.POSX:
                            posx = "(" + content[1] + ")"
                            definition_list.append(posx)
                        elif content_type == Type.DEFINITION:
                            definition = content[1]
                            definition_list.append(definition)

                    definition = " ".join(definition_list)

                    label = (entry_pos if entry_pos else "") + (
                        pos_index_pos if pos_index_pos else ""
                    )

                    if definition:
                        current_definition = objects.Definition(
                            definition=definition, label=label
                        )
                        current_entry.append_to_defs(current_definition)

                    # Then, parse the examples
                    pos_index_ex_lines = list(
                        filter(
                            lambda x: x["parsed"]
                            and x["parsed"][0]
                            in (
                                Type.EXAMPLE_HANZI,
                                Type.EXAMPLE_JYUTPING,
                                Type.EXAMPLE_TRANSLATION,
                            ),
                            applies_to_all_in_this_pos_index,
                        )
                    )
                    pos_index_ex_lines = list(
                        map(lambda x: x["parsed"], pos_index_ex_lines)
                    )

                    ex_jyut = ex_hz = ex_tr = ""
                    for content_type, content in pos_index_ex_lines:
                        if content_type == Type.EXAMPLE_HANZI:
                            ex_hz = content
                        elif content_type == Type.EXAMPLE_JYUTPING:
                            ex_jyut = content
                        elif content_type == Type.EXAMPLE_TRANSLATION:
                            if content[0] != "en":
                                continue

                            ex_tr = content[1]

                    if ex_jyut or ex_hz or ex_tr:
                        current_definition.examples.append([])
                        current_definition.examples[-1].append(
                            objects.Example(lang="yue", pron=ex_jyut, content=ex_hz)
                        )
                        current_definition.examples[-1].append(
                            objects.Example(lang="eng", content=ex_tr)
                        )

                    for def_index in range(1, 10):
                        # Then, parse all the lines that apply to a smaller scope
                        # For each pos_index, there can be many definitions, but limit ourselves to 9 for now
                        def_index_lines = list(
                            filter(
                                lambda x: x["def_index"] == def_index, pos_index_lines
                            )
                        )
                        applies_to_all_in_this_def_index_lines = list(
                            filter(lambda x: x["ex_index"] == None, def_index_lines)
                        )

                        # Parse the definitions that apply to all lines in this index
                        def_index_def_lines = list(
                            filter(
                                lambda x: x["parsed"]
                                and x["parsed"][0] in (Type.POSX, Type.DEFINITION),
                                applies_to_all_in_this_def_index_lines,
                            )
                        )
                        def_index_def_lines = list(
                            filter(
                                lambda x: x["parsed"][1][0] == "en", def_index_def_lines
                            )
                        )
                        def_index_def_lines = list(
                            map(lambda x: x["parsed"], def_index_def_lines)
                        )

                        posx = definition = ""
                        definition_list = []
                        for content_type, content in def_index_def_lines:
                            if content_type == Type.POSX:
                                posx = "(" + content[1] + ")"
                                definition_list.append(posx)
                            elif content_type == Type.DEFINITION:
                                definition = content[1]
                                definition_list.append(definition)

                        definition = " ".join(definition_list)

                        label = (entry_pos if entry_pos else "") + (
                            pos_index_pos if pos_index_pos else ""
                        )

                        if definition:
                            current_definition = objects.Definition(
                                definition=definition, label=label
                            )
                            current_entry.append_to_defs(current_definition)

                        # Then, parse the examples
                        def_index_ex_lines = list(
                            filter(
                                lambda x: x["parsed"]
                                and x["parsed"][0]
                                in (
                                    Type.EXAMPLE_HANZI,
                                    Type.EXAMPLE_JYUTPING,
                                    Type.EXAMPLE_TRANSLATION,
                                ),
                                applies_to_all_in_this_def_index_lines,
                            )
                        )
                        def_index_ex_lines = list(
                            map(lambda x: x["parsed"], def_index_ex_lines)
                        )

                        ex_jyut = ex_hz = ex_tr = ""
                        for content_type, content in def_index_ex_lines:
                            if content_type == Type.EXAMPLE_HANZI:
                                ex_hz = content
                            elif content_type == Type.EXAMPLE_JYUTPING:
                                ex_jyut = content
                            elif content_type == Type.EXAMPLE_TRANSLATION:
                                if content[0] != "en":
                                    continue

                                ex_tr = content[1]

                        if ex_jyut or ex_hz or ex_tr:
                            current_definition.examples.append([])
                            current_definition.examples[-1].append(
                                objects.Example(lang="yue", pron=ex_jyut, content=ex_hz)
                            )
                            current_definition.examples[-1].append(
                                objects.Example(lang="eng", content=ex_tr)
                            )

                        for ex_index in range(1, 10):
                            # For each definition, there can be up to 10 examples
                            ex_index_lines = list(
                                filter(
                                    lambda x: x["ex_index"] == ex_index, def_index_lines
                                )
                            )

                            ex_index_ex_lines = list(
                                filter(
                                    lambda x: x["parsed"]
                                    and x["parsed"][0]
                                    in (
                                        Type.EXAMPLE_HANZI,
                                        Type.EXAMPLE_JYUTPING,
                                        Type.EXAMPLE_TRANSLATION,
                                    ),
                                    ex_index_lines,
                                )
                            )
                            ex_index_ex_lines = list(
                                map(lambda x: x["parsed"], ex_index_ex_lines)
                            )

                            ex_jyut = ex_hz = ex_tr = ""
                            for content_type, content in ex_index_ex_lines:
                                if content_type == Type.EXAMPLE_HANZI:
                                    ex_hz = content
                                elif content_type == Type.EXAMPLE_JYUTPING:
                                    ex_jyut = content
                                elif content_type == Type.EXAMPLE_TRANSLATION:
                                    if content[0] != "en":
                                        continue

                                    ex_tr = content[1]

                            if ex_jyut or ex_hz or ex_tr:
                                current_definition.examples.append([])
                                current_definition.examples[-1].append(
                                    objects.Example(
                                        lang="yue", pron=ex_jyut, content=ex_hz
                                    )
                                )
                                current_definition.examples[-1].append(
                                    objects.Example(lang="eng", content=ex_tr)
                                )

                words[current_entry.traditional].append(current_entry)

                for variant in variants:
                    traditional, simplified = variant
                    variant_entry = copy.deepcopy(current_entry)
                    variant_entry.add_simplified(simplified)
                    variant_entry.add_traditional(traditional)
                    variant_entry.add_pinyin(
                        " ".join(
                            lazy_pinyin(
                                simplified,
                                style=Style.TONE3,
                                neutral_tone_with_five=True,
                            )
                        )
                        .lower()
                        .replace("v", "u:")
                    )
                    variant_entry.add_freq(zipf_frequency(traditional, "zh"))
                    words[variant_entry.traditional].append(variant_entry)

                current_entry_lines = []


if __name__ == "__main__":
    if len(sys.argv) != 11:
        print(
            (
                "Usage: python3 script.py <database filename> "
                "<cidian.u8 file> "
                "<source name> <source short name> "
                "<source version> <source description> <source legal> "
                "<source link> <source update url> <source other>"
            )
        )
        print(
            (
                "e.g. python3 -m aby.parse aby/developer/aby.db aby/data/jyut.u8 "
                '"ABC Cantonese-English Dictionary" ABY 2018-02-10 '
                '"The first and most authoritative reference of its kind to be '
                "published in the last forty years, ABC Cantonese-English "
                "Comprehensive Dictionary comprises about 15,000 lexical "
                "entries that are unique to the colloquial Cantonese language "
                "as it is spoken and written in Hong Kong today. Author "
                "Robert S. Bauer, a renowned lexicographer and authority on "
                "Cantonese, has utilized language documentation resources to the "
                "fullest extent by gathering material firsthand from dictionaries, "
                "glossaries, and grammars; newspapers and magazines; government "
                "records; cartoons and comic books; film and television; websites; "
                "and native speakers striding the sidewalks of Hong Kong to "
                'capture concretely contemporary Cantonese." '
                '"Copyright © 2017–2021 Wenlin Institute, Inc. SPC, All Rights Reserved" '
                '"https://wenlin.co/wow/Project:Jyut" "" "words,sentences"'
            )
        )
        sys.exit(1)

    cc_cedict.load()

    entries = defaultdict(list)
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
    parse_file(sys.argv[2], entries)
    write(sys.argv[1], source, entries)
