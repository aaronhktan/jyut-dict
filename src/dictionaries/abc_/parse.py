from dragonmapper import transcriptions
# import jieba
# import pinyin_jyutping_sentence

from database import database, objects

import logging
import re
import sys

IGNORED_LINES = ("cidian.wenlindb\n", ".-arc\n", ".-publish\n")
EXAMPLE_TYPE = re.compile(r"(\d*)(\w*)(@\w*)*")

def parse_pinyin(content):
    # Pinyin sometimes has superscript numbers preceding them
    # I assume it's to distinguish between words with the same Pinyin? Unsure
    content = content.strip("¹²³⁴⁵⁶⁷⁸⁹")
    # ABC indicates sound change by a dot underneath the vowel
    # But we don't support that, so remove it
    content = content.translate(str.maketrans('ạẹịọụ', 'aeiou', '*'))
    # Make the Pinyin lowercase, so that dragonmapper can parse it
    content = content.lower()
    # Transcribing to zhuyin first identifies character boundaries
    return transcriptions.to_pinyin(transcriptions.to_zhuyin(content), accented=False)

def parse_char(content):
    # Traditional form is indicated enclosed in square brackets
    bracket_index = content.find("[")
    if bracket_index != -1:
        simp = content[:bracket_index]
        trad = content[bracket_index+1:content.find("]")]
    else:
        simp = trad = content

    return trad, simp


def parse_file(filename, words):
    with open(filename) as file:
        for line in file.readlines():
            if not line or line in IGNORED_LINES:
                continue

            if line == "\n":
                # Write down parsed entry, start new entry
                continue

            line = line.split("   ")
            if len(line) >= 2:
                try:
                    column_type, content = line[0], line[1]
                except Exception as e:
                    logging.error(f"Couldn't parse line: '{line}'")
                    continue
            else:
                continue

            content = content.strip()

            if column_type == ".py":
                pin = parse_pinyin(content)
                print(f"pin: {pin}")
            elif column_type == "char":
                trad, simp = parse_char(content)
                print(f"trad: {trad}, simp: {simp}")
            elif column_type == "ps":
                part_of_speech = content
                print(f"pos: {part_of_speech}")
            elif column_type[0].isdigit():
                # Example number and column type are smushed together in examples
                # e.g. "1ex": example_number = 1, column_type = example
                match = re.match(EXAMPLE_TYPE, column_type)
                example_number, example_type = match.group(1), match.group(2)
                print(f"ex_num: {example_number}, ex_type: {example_type}")



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
                'ABC Chinese-English Dictionary, the first strictly '
                'alphabetically ordered and Pinyin computerized dictionary. '
                'It contains over 196,000 entries, compared to the 71,486 '
                'entries of the earlier work, making it the most '
                'comprehensive one-volume dictionary of Chinese." '
                '"Copyright © 1996-2019 University of Hawai‘i Press, All Rights Reserved" '
                '"https://wenlin.com/abc" "" "words,sentences"'
            )
        )
        sys.exit(1)

    entries = {}
    source = objects.SourceTuple(
        sys.argv[3],
        sys.argv[4],
        sys.argv[5],
        sys.argv[6],
        sys.argv[7],
        sys.argv[8],
        sys.argv[9],
        sys.argv[10]
    )
    parse_file(sys.argv[2], entries)
    # write(entries, sys.argv[1])