from dragonmapper import transcriptions
from hanziconv import HanziConv

# import jieba
# import pinyin_jyutping_sentence

from database import database, objects

from collections import defaultdict
from enum import Enum
import logging
import re
import sqlite3
import string
import sys

IGNORED_LINES = ("cidian.wenlindb\n", ".-arc\n", ".-publish\n")
IGNORED_TYPES = (
    "gr",
    "ref",
    "rem",
    "ser",
)
EXAMPLE_TYPE = re.compile(
    r"(?P<pos_index>\d?)(?P<def_index>\d?)(?P<ex_index>\d*)(?P<type>\w*)(@\w*)*"
)


class Type(Enum):
    NONE = 0
    IGNORED = 1
    ERROR = 2
    FINISHED_ENTRY = 3

    PINYIN = 11
    HANZI = 12

    POS = 21
    POSX = 22
    DEFINITION = 23

    EXAMPLE_PINYIN = 31
    EXAMPLE_HANZI = 32
    EXAMPLE_TRANSLATION = 33

    SMUSHED = 41


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
    # Reserved sentence IDs:
    #   - 0-999999999: Tatoeba
    #   - 1000000000-1999999999: words.hk
    #   - 2000000000-2999999999: CantoDict
    #   - 3000000000-3999999999: MoEDict
    #   - 4000000000-4999999999: Cross-Straits Dictionary
    #   - 5000000000-5999999999: ABC Chinese-English Dictionary
    example_id = 5000000000

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


def parse_pinyin(content):
    # Pinyin sometimes has superscript numbers preceding them
    # I assume it's to distinguish between words with the same Pinyin? Unsure
    content = content.strip("¹²³⁴⁵⁶⁷⁸⁹")
    # ABC indicates sound change by a dot underneath the vowel
    # But we don't support that, so remove it
    content = content.translate(str.maketrans("ạẹịọụ", "aeiou", "*"))
    # Make the Pinyin lowercase, so that dragonmapper can parse it
    content = content.lower()
    # Insert spaces in front of commas...
    content = content.replace(",", " ,")
    # ...so that we can split up the pinyin by whitespace
    content = content.split()

    # print(content)

    # Transcribing to zhuyin first identifies character boundaries
    new_content = []
    for x in content:
        try:
            new_content.append(
                transcriptions.to_pinyin(transcriptions.to_zhuyin(x), accented=False)
            )
        except:
            new_content.append(x)
    content = new_content

    # Rejoin the string
    content = " ".join(content)
    # And remove the space in front of the comma
    content = content.replace(" ,", ",")

    return content


def parse_char(content):
    # Traditional form is indicated enclosed in square brackets
    bracket_index = content.find("[")
    if bracket_index != -1:
        simp = content[:bracket_index]
        # fmt: off
        trad = content[bracket_index+1:content.find("]")]
        # fmt: on
    else:
        simp = trad = content

    return trad, simp


def parse_definition(content):
    # Assume translation is in English by default
    lang = "en"

    # If there are multiple translations for an example in different languages, they will
    # start with the language tag enclosed in square brackets, e.g. [en]
    if content.startswith("["):
        # fmt: off
        lang = content[content.find("[")+1:content.find("]")]
        content = content[content.find["]"]+2:]
        # fmt: on

    return lang, content


def parse_example_pinyin(content, entry_pinyin):
    # Replace ∼ in example pinyin with the entry's pinyin
    content = content.replace("∼", entry_pinyin)
    # Dragonmapper cannot handle punctuation
    # And cidian.u8 does not have any examples that have more than one punctuation mark at the end
    # So strip out the punctuation mark, and then re-add it after transcribing
    stripped_punctuation = content[-1]
    content = content[:-1] if content[-1] in string.punctuation else content
    # print(content)
    # Convert to numbered pinyin
    content = parse_pinyin(content)
    # Re-add stripped punctuation
    content = (
        content + stripped_punctuation if content[-1] in string.punctuation else content
    )

    return content


def parse_example_hanzi(content, simp):
    # Replace ∼ in example hanzi with entry's simplified form (all examples are in simplified)
    content = content.replace("∼", simp)
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
    if column_type == ".py":
        pin = parse_pinyin(content)
        return Type.PINYIN, pin
    elif column_type == "char":
        trad, simp = parse_char(content)
        return Type.HANZI, (trad, simp)
    elif column_type == "ps":
        part_of_speech = content
        return Type.POS, part_of_speech
    elif column_type == "psx":
        # not sure exactly what psx actually means
        part_of_speech_extended = content
        return Type.POSX, part_of_speech_extended
    elif column_type == "df":
        language, definition = parse_definition(content)
        return Type.DEFINITION, (language, definition)
    elif column_type == "ex":
        if not entry:
            return Type.ERROR, None
        example_pinyin = parse_example_pinyin(content, entry.pinyin)
        return Type.EXAMPLE_PINYIN, example_pinyin
    elif column_type == "hz":
        if not entry:
            return Type.ERROR, None
        example_hanzi = parse_example_hanzi(content, entry.simplified)
        return Type.EXAMPLE_HANZI, example_hanzi
    elif column_type == "tr":
        language, translation = parse_example_translation(content)
        return Type.EXAMPLE_TRANSLATION, (language, translation)
    elif column_type[0].isdigit():
        # In entries with multiple parts of speech and definitions per part of speech,
        # part-of-speech number, definition number, example number, and type are smushed together
        # e.g. "312hz": part-of-speech #3, definition #1 of pos #3, example #2 for definition #1, type = hanzi
        if EXAMPLE_TYPE.search(column_type):
            match = EXAMPLE_TYPE.match(column_type).groupdict()
            parsed = parse_content(match["type"], content, entry)
            return Type.SMUSHED, (
                int(match["pos_index"]) if match["pos_index"] else None,
                int(match["def_index"]) if match["def_index"] else None,
                int(match["ex_index"]) if match["ex_index"] else None,
                parsed,
            )
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

    current_pos = None
    part_of_speech_index = 0

    current_posx = None

    current_definition = None
    definition_index = 0

    current_example = None
    example_index = 0

    with open(filename) as file:
        for line in file.readlines():
            parsed_type, parsed = parse_line(line, current_entry)

            if parsed_type in (Type.NONE, Type.IGNORED, Type.ERROR):
                continue

            elif parsed_type == Type.FINISHED_ENTRY:
                if current_definition:
                    current_entry.append_to_defs(current_definition)
                if current_entry:
                    words[current_entry.traditional].append(current_entry)
                current_entry = objects.Entry()
                current_pos = current_posx = current_definition = current_example = None
                pos_index = definition_index = example_index = 0

            elif parsed_type == Type.PINYIN:
                current_entry.add_pinyin(parsed)
            elif parsed_type == Type.HANZI:
                current_entry.add_traditional(parsed[0])
                current_entry.add_simplified(parsed[1])
            elif parsed_type == Type.POS:
                current_pos = parsed
            elif parsed_type == Type.POSX:
                current_posx = parsed
                current_definition = objects.Definition(
                    definition=current_posx, label=current_pos
                )
            elif parsed_type == Type.DEFINITION:
                lang, definition = parsed
                # The PSX field should be prepended to all definitions that have a PSX field
                definition = current_posx + definition if current_posx else definition
                current_definition = objects.Definition(
                    definition=definition, label=current_pos
                )

            elif parsed_type == Type.SMUSHED:
                pos_index, def_index, ex_index, parsed_content = parsed

                if pos_index:
                    if pos_index == part_of_speech_index + 1:
                        if pos_index > 0:
                            # Done parsing the last part of speech; write down current definition and move on
                            current_entry.append_to_defs(current_definition)
                        current_definition = objects.Definition()
                part_of_speech_index = pos_index if pos_index else 0

                if def_index:
                    if def_index == definition_index + 1:
                        if def_index > 0:
                            # We are now parsing the next definition; write down the current definition and move on
                            current_entry.append_to_defs(current_definition)
                        current_definition = objects.Definition()
                    elif def_index > definition_index + 1:
                        logging.error(
                            "Uh oh, def_index went wrong: {current_entry}, expected {definition_index}, got {def_index}"
                        )
                definition_index = def_index if def_index else 0

                if ex_index:
                    if ex_index == example_index + 1:
                        current_example = objects.Example(lang="cmn")
                        current_definition.examples.append([current_example])
                    elif ex_index > example_index + 1:
                        logging.error(
                            "Uh oh, ex_index went wrong: {current_entry}, expected {example_index}, got {ex_index}"
                        )
                else:
                    current_example = objects.Example(lang="cmn")
                    current_definition.examples.append([current_example])
                example_index = ex_index if ex_index else 0

                parsed_type, parsed = parsed_content
                if parsed_type == Type.POS:
                    current_pos = parsed
                elif parsed_type == Type.POSX:
                    current_definition.definition = parsed
                elif parsed_type == Type.EXAMPLE_PINYIN:
                    current_example.pron = parsed
                elif parsed_type == Type.EXAMPLE_HANZI:
                    current_example.content = parsed
                elif parsed_type == Type.EXAMPLE_TRANSLATION:
                    lang = parsed[0]
                    if lang != "en":
                        # Only keep English translations for now
                        continue

                    current_definition.examples[-1].append(
                        objects.Example(lang="eng", content=parsed[1])
                    )


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
                "e.g. python3 -m abc_.parse abc.db abc/data/cidian.u8 "
                '"ABC Chinese-English Dictionary" ABC 2015-12-18 '
                '"This dictionary is an expansion of the ground breaking '
                "ABC Chinese-English Dictionary, the first strictly "
                "alphabetically ordered and Pinyin computerized dictionary. "
                "It contains over 196,000 entries, compared to the 71,486 "
                "entries of the earlier work, making it the most "
                'comprehensive one-volume dictionary of Chinese." '
                '"Copyright © 1996-2019 University of Hawai‘i Press, All Rights Reserved" '
                '"https://wenlin.com/abc" "" "words,sentences"'
            )
        )
        sys.exit(1)

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
