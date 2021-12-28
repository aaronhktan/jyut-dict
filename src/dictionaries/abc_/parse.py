from dragonmapper import transcriptions

# import jieba
# import pinyin_jyutping_sentence

from database import database, objects

from collections import defaultdict
import enum
import logging
import re
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




def parse_pinyin(content):
    # Pinyin sometimes has superscript numbers preceding them
    # I assume it's to distinguish between words with the same Pinyin? Unsure
    content = content.strip("¹²³⁴⁵⁶⁷⁸⁹")
    # ABC indicates sound change by a dot underneath the vowel
    # But we don't support that, so remove it
    content = content.translate(str.maketrans("ạẹịọụ", "aeiou", "*"))
    # Make the Pinyin lowercase, so that dragonmapper can parse it
    content = content.lower()
    # Transcribing to zhuyin first identifies character boundaries
    return transcriptions.to_pinyin(transcriptions.to_zhuyin(content), accented=False)


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
        content = content[content.find["]"]+2:]
        # fmt: on

    return lang, content


def parse_content(column_type, content):
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
        example_pinyin = parse_example_pinyin(content, pin)
        return Type.EXAMPLE_PINYIN, example_pinyin
    elif column_type == "hz":
        example_hanzi = parse_example_hanzi(content, simp)
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
            parsed = parse_content(match["type"], content)
            return Type.SMUSHED, (
                match["pos_index"],
                match["def_index"],
                match["ex_index"],
                parsed,
            )


def parse_line(line):
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

    return parse_content(column_type, content)


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
            parsed_type, parsed = parse_line(line)

            if parsed_type in (Type.NONE, Type.IGNORED, Type.ERROR):
                continue

            elif parsed_type == Type.FINISHED_ENTRY:
                if current_entry:
                    words[current_entry.traditional].append(current_entry)
                current_entry = objects.Entry()
                current_pos = current_definition = current_example = None
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
                current_definition = objects.Definition(definition=current_posx, label=current_pos)
            elif parsed_type == Type.DEFINITION:
                # The PSX field should be prepended to all definitions that have a PSX field
                definition = current_posx + parsed if current_posx else parsed
                current_definition = objects.Definition(definition=definition, label=current_pos)

            elif parsed_type == Type.SMUSHED:
                pos_index, def_index, ex_index, parsed_content = parsed

                if pos_index == part_of_speech_index + 1:
                    if pos_index > 0:
                        # Done parsing the last part of speech; write down current definition and move on
                        current_entry.append_to_defs(current_definition)
                    current_definition = objects.Definition()
                part_of_speech_index = pos_index if pos_index else 0

                if def_index == definition_index + 1:
                    if def_index > 0:
                        # We are now parsing the next definition; write down the current definition and move on
                        current_entry.append_to_defs(current_definition)
                    current_definition = objects.Definition()
                elif def_index > definition_index + 1:
                    logging.error("Uh oh, def_index went wrong: {current_entry}, expected {definition_index}, got {def_index}")
                definition_index = def_index if def_index else 0

                if ex_index == example_index + 1:
                    current_definition.examples.append([current_example])
                    current_example = objects.Example(lang="cmn")
                elif ex_index > example_index + 1:
                    logging.error("Uh oh, ex_index went wrong: {current_entry}, expected {example_index}, got {ex_index}")
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

                    current_definitions.examples[-1].append(objects.Example(lang="eng", content=parsed[1]))



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
    write(entries, sys.argv[1])
