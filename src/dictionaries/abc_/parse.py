from dragonmapper import transcriptions
from hanziconv import HanziConv
import pinyin_jyutping_sentence
from wordfreq import zipf_frequency

from database import database, objects

from collections import defaultdict
from enum import Enum
import logging
import re
import sqlite3
import string
import sys
import unicodedata

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
    simp = HanziConv.toSimplified(trad) if trad else ""
    jyut = ""
    pin = example[0].pron
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
    # Also replace combining characters with single-character equivalents
    content = unicodedata.normalize("NFKC", content)
    # Make the Pinyin lowercase, so that dragonmapper can parse it
    content = content.lower()
    # Insert spaces in front of punctuation
    for punctuation in string.punctuation:
        content = content.replace(punctuation, " " + punctuation)
    # ...so that we can split up the pinyin by whitespace
    content = content.split()

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
    # And remove the space in front of punctuatation
    for punctuation in string.punctuation:
        content = content.replace(" " + punctuation, punctuation)

    return content


def parse_char(content):
    # Traditional form is indicated enclosed in square brackets
    bracket_index = content.find("[")
    if bracket_index != -1:
        simp = content[:bracket_index]
        # fmt: off
        trad = content[bracket_index+1:content.find("]")]
        trad = list(trad)
        for index, char in enumerate(trad):
            if char == "-":
                try:
                    trad[index] = simp[index]
                except:
                    logging.error(f"couldn't get traditional from simplified {simp}, traditional {trad}")
        trad = "".join(trad)
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
        content = content[content.find("]")+2:]
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
    # Convert to numbered pinyin
    content = parse_pinyin(content)
    # Re-add stripped punctuation
    content = (
        content + stripped_punctuation if stripped_punctuation in string.punctuation else content
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
        lang, part_of_speech_extended = parse_definition(content)
        return Type.POSX, (lang, part_of_speech_extended)
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
                entry_pos = None
                entry_posx = None
                entry_ex_hz = None
                entry_ex_pin = None
                entry_ex_tr = None

                # First, parse all the lines that relate to the entirety of the entry
                # These lines do not have a number preceding them.
                entry_lines = list(filter(lambda x: not (x[0].isdigit()), current_entry_lines))
                parsed_entry_lines = list(map(parse_line, entry_lines, [current_entry] * len(entry_lines)))

                for parsed_type, parsed in parsed_entry_lines:
                    if parsed_type == Type.PINYIN:
                        current_entry.add_pinyin(parsed)
                    elif parsed_type == Type.HANZI:
                        current_entry.add_traditional(parsed[0])
                        current_entry.add_simplified(parsed[1])
                        current_entry.add_freq(zipf_frequency(parsed[0], "zh"))
                        current_entry.add_jyutping(pinyin_jyutping_sentence.jyutping(
                            parsed[0], tone_numbers=True, spaces=True
                        ))
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
                        definition = entry_posx + " " + definition if entry_posx else definition
                        label = entry_pos if entry_pos else ""

                        current_definition = objects.Definition(definition=definition, label=label)
                        current_entry.append_to_defs(current_definition)
                    elif content_type == Type.EXAMPLE_HANZI:
                        entry_ex_hz = content
                    elif content_type == Type.EXAMPLE_PINYIN:
                        entry_ex_pin = content
                    elif content_type == Type.EXAMPLE_TRANSLATION:
                        if content[0] != "en":
                            continue

                        entry_ex_tr = content[1]

                if entry_ex_pin or entry_ex_hz or entry_ex_tr:
                    current_definition.examples.append([])
                    current_definition.examples[-1].append(objects.Example(lang="cmn", pron=entry_ex_pin, content=entry_ex_hz))
                    current_definition.examples[-1].append(objects.Example(lang="eng", content=entry_ex_tr))


                # Then, parse each of the numbered lines
                numbered_lines = list(filter(lambda x: x[0].isdigit(), current_entry_lines))
                parsed_numbered_lines = list(map(parse_line, numbered_lines, [current_entry] * len(numbered_lines)))
                parsed_numbered_lines = list(filter(lambda x: x[0] == Type.SMUSHED, parsed_numbered_lines))
                parsed_numbered_lines = list(map(lambda x: x[1], parsed_numbered_lines))

                for pos_index in range(1, 8):
                    # In the ABC dictionary, there is a maximum of seven parts of speech (1-indexed)
                    pos_index_lines = list(filter(lambda x: x["pos_index"] == pos_index, parsed_numbered_lines))

                    # First, let's isolate lines that apply to all lines in this pos_index:
                    # do this by finding lines where the pos_index is some number, def_index == None, and ex_index == None
                    # e.g. "1ps   n." => applies to "11df   greeting", as well as "12df   salutation"
                    applies_to_all_in_this_pos_index = list(filter(lambda x: x["def_index"] == None and x["ex_index"] == None, pos_index_lines))

                    # Parse the part of speech that applies to all lines in this index
                    pos_index_pos_line = list(filter(lambda x: x["parsed"] and x["parsed"][0] == Type.POS, applies_to_all_in_this_pos_index))
                    pos_index_pos_line = list(map(lambda x: x["parsed"], pos_index_pos_line))

                    pos_index_pos = ""
                    if len(pos_index_pos_line) > 1:
                        logging.error(f"Found more than one part of speech for index {pos_index} in entry {entry.traditional}")
                    elif len(pos_index_pos_line) == 1:
                        pos_index_pos = pos_index_pos_line[0][1]

                    # Parse the definitions that apply to all lines in this index
                    pos_index_def_lines = list(filter(lambda x: x["parsed"] and x["parsed"][0] in (Type.POSX, Type.DEFINITION), applies_to_all_in_this_pos_index))
                    pos_index_def_lines = list(filter(lambda x: x["parsed"][1][0] == "en", pos_index_def_lines))
                    pos_index_def_lines = list(map(lambda x: x["parsed"], pos_index_def_lines))

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

                    label = (entry_pos if entry_pos else "") + (pos_index_pos if pos_index_pos else "")

                    if definition:
                        current_definition = objects.Definition(definition=definition, label=label)
                        current_entry.append_to_defs(current_definition)

                    # Then, parse the examples
                    pos_index_ex_lines = list(filter(lambda x: x["parsed"] and x["parsed"][0] in (Type.EXAMPLE_HANZI, Type.EXAMPLE_PINYIN, Type.EXAMPLE_TRANSLATION), applies_to_all_in_this_pos_index))
                    pos_index_ex_lines = list(map(lambda x: x["parsed"], pos_index_ex_lines))

                    ex_pin = ex_hz = ex_tr = ""
                    for content_type, content in pos_index_ex_lines:
                        if content_type == Type.EXAMPLE_HANZI:
                            ex_hz = content
                        elif content_type == Type.EXAMPLE_PINYIN:
                            ex_pin = content
                        elif content_type == Type.EXAMPLE_TRANSLATION:
                            if content[0] != "en":
                                continue

                            ex_tr = content[1]

                    if ex_pin or ex_hz or ex_tr:
                        current_definition.examples.append([])
                        current_definition.examples[-1].append(objects.Example(lang="cmn", pron=ex_pin, content=ex_hz))
                        current_definition.examples[-1].append(objects.Example(lang="eng", content=ex_tr))

                    for def_index in range(1, 10):
                        # Then, parse all the lines that apply to a smaller scope
                        # For each pos_index, there can be many definitions, but limit ourselves to 9 for now
                        def_index_lines = list(filter(lambda x: x["def_index"] == def_index, pos_index_lines))
                        applies_to_all_in_this_def_index_lines = list(filter(lambda x: x["ex_index"] == None, def_index_lines))

                        # Parse the definitions that apply to all lines in this index
                        def_index_def_lines = list(filter(lambda x: x["parsed"] and x["parsed"][0] in (Type.POSX, Type.DEFINITION), applies_to_all_in_this_def_index_lines))
                        def_index_def_lines = list(filter(lambda x: x["parsed"][1][0] == "en", def_index_def_lines))
                        def_index_def_lines = list(map(lambda x: x["parsed"], def_index_def_lines))

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

                        label = (entry_pos if entry_pos else "") + (pos_index_pos if pos_index_pos else "")

                        if definition:
                            current_definition = objects.Definition(definition=definition, label=label)
                            current_entry.append_to_defs(current_definition)

                        # Then, parse the examples
                        def_index_ex_lines = list(filter(lambda x: x["parsed"] and x["parsed"][0] in (Type.EXAMPLE_HANZI, Type.EXAMPLE_PINYIN, Type.EXAMPLE_TRANSLATION), applies_to_all_in_this_def_index_lines))
                        def_index_ex_lines = list(map(lambda x: x["parsed"], def_index_ex_lines))

                        ex_pin = ex_hz = ex_tr = ""
                        for content_type, content in def_index_ex_lines:
                            if content_type == Type.EXAMPLE_HANZI:
                                ex_hz = content
                            elif content_type == Type.EXAMPLE_PINYIN:
                                ex_pin = content
                            elif content_type == Type.EXAMPLE_TRANSLATION:
                                if content[0] != "en":
                                    continue

                                ex_tr = content[1]

                        if ex_pin or ex_hz or ex_tr:
                            current_definition.examples.append([])
                            current_definition.examples[-1].append(objects.Example(lang="cmn", pron=ex_pin, content=ex_hz))
                            current_definition.examples[-1].append(objects.Example(lang="eng", content=ex_tr))


                        for ex_index in range(1, 10):
                            # For each definition, there can be up to 10 examples
                            ex_index_lines = list(filter(lambda x: x["ex_index"] == ex_index, def_index_lines))

                            ex_index_ex_lines = list(filter(lambda x: x["parsed"] and x["parsed"][0] in (Type.EXAMPLE_HANZI, Type.EXAMPLE_PINYIN, Type.EXAMPLE_TRANSLATION), ex_index_lines))
                            ex_index_ex_lines = list(map(lambda x: x["parsed"], ex_index_ex_lines))

                            ex_pin = ex_hz = ex_tr = ""
                            for content_type, content in ex_index_ex_lines:
                                if content_type == Type.EXAMPLE_HANZI:
                                    ex_hz = content
                                elif content_type == Type.EXAMPLE_PINYIN:
                                    ex_pin = content
                                elif content_type == Type.EXAMPLE_TRANSLATION:
                                    if content[0] != "en":
                                        continue

                                    ex_tr = content[1]

                            if ex_pin or ex_hz or ex_tr:
                                current_definition.examples.append([])
                                current_definition.examples[-1].append(objects.Example(lang="cmn", pron=ex_pin, content=ex_hz))
                                current_definition.examples[-1].append(objects.Example(lang="eng", content=ex_tr))

                words[current_entry.traditional].append(current_entry)
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
