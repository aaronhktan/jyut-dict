from bs4 import BeautifulSoup, element

from database import database, objects

import os
import re
import sys


def parse_file(file_name, words):
    with open(file_name, "r") as file:
        soup = BeautifulSoup(file, "html.parser")

        # Extract the traditional and simplified forms
        forms = [x.strip() for x in soup.find("td", class_="chinesebig").get_text().split(" / ")]
        if len(forms) > 1:
            trad = forms[0]
            simp = forms[1]
        else:
            trad = simp = forms[0]

        word = os.path.splitext(os.path.basename(file_name))[0]
        if trad != word:
            print(
                f"Hmm, looks like the parsed word {trad} doesn't match the filename {word}"
            )

        # Extract the pronunciations
        jyut_element = soup.find("span", class_="cardjyutping")
        jyut = jyut_element.get_text() if jyut_element else ""

        pin_element = soup.find("span", class_="cardpinyin")
        pin = pin_element.get_text() if pin_element else ""

        # Extract the meaning(s)
        meaning_element = soup.find("td", class_="wordmeaning")
        # The layout of compound word pages is different from single-character pages
        real_meaning_element = meaning_element.find("div", class_=None)
        if real_meaning_element:
            meaning_element = real_meaning_element

        # Remove children
        children = meaning_element.find_all("div")
        children += meaning_element.find_all("span")
        for child in children:
            child.decompose()

        meanings = []
        if meaning_element:
            for string in re.compile(r"\[\d+\]\s+|\n").split(meaning_element.get_text()):
                string = string.strip()
                if string and "Default PoS:" not in string:
                    meanings.append(string)

        entry = objects.Entry(trad, simp, pin, jyut, defs=meanings)

        print(entry)


def parse_folder(folder_name, words):
    for index, entry in enumerate(os.scandir(folder_name)):
        if not index % 100:
            print(f"Parsed word #{index}")
        if entry.is_file() and entry.path.endswith(".html"):
            parse_file(entry.path, words)


words = {}
parse_folder(sys.argv[1], {})
