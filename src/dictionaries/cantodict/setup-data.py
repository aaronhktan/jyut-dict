from bs4 import BeautifulSoup

import random
import requests
import sys
import time

# Limitation of this script:
# For words that start with a Roman character (e.g. MK仔), CantoDict does not provide a link from the wordlist.
# This script will get a link to the first non-Roman character (e.g. 仔), which is incorrect.
# The parse.py script will filter out pages that don't match the filename - so if you want to get compound words
# that start with a Roman character, you have to download them yourself!

# For now, ignore incomplete compound words
urls = [
    ("http://www.cantonese.sheik.co.uk/masterlist.htm?action=unsure", 1),
    ("http://www.cantonese.sheik.co.uk/masterlist.htm?level=1", 15),
    ("http://www.cantonese.sheik.co.uk/masterlist.htm?level=2", 26),
    ("http://www.cantonese.sheik.co.uk/masterlist.htm?level=3", 53),
    ("http://www.cantonese.sheik.co.uk/masterlist.htm?level=4", 14),
    ("http://www.cantonese.sheik.co.uk/scripts/wordlist.htm?level=1", 25),
    ("http://www.cantonese.sheik.co.uk/scripts/wordlist.htm?level=2", 192),
    ("http://www.cantonese.sheik.co.uk/scripts/wordlist.htm?level=3", 2203),
    ("http://www.cantonese.sheik.co.uk/scripts/wordlist.htm?level=4", 512),
    ("http://www.cantonese.sheik.co.uk/scripts/wordlist.htm?level=5", 81),
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=1", 22),
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=2", 54),
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=3", 73),
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=4", 8),
]

character_file = open(sys.argv[1], "a")
word_file = open(sys.argv[2], "a")
sentence_file = open(sys.argv[3], "a")

for base_url, num_pages in urls:
    # Character pages, compound word pages, and sentence pages use different class names
    if "wordlist" in base_url:
        classname = "wl_uni"
    elif "masterlist" in base_url:
        classname = "chinese"
    else:
        classname = "chinesemed"

    for page in range(num_pages):
        scrape_url = base_url + "&page=" + str(page)
        print("Scraping url: " + scrape_url)

        r = requests.get(scrape_url)
        document = BeautifulSoup(r.text, "html.parser")

        if "examplelist" in base_url:
            word_elems = document.find_all("span", class_=classname)

            for elem in word_elems:
                sentence = elem.get_text().replace("/", "%2F") # Take out illegal "/" character
                sentence = sentence.replace("\t", "%09") # Take out illegal "\t" character
                sentence_url = elem.parent.parent.find("a").get("href")
                sentence_file.write(f"{sentence}\t{sentence_url}\n")
        else:
            word_elems = document.find_all("td", class_=classname)

            for elem in word_elems:
                word = elem.get_text()
                if elem.find("a"):
                    word_url = elem.find("a").get("href")
                else:
                    word_url = "?"

                if "wordlist" in base_url:
                    word_file.write(f"{word}\t{word_url}\n")
                elif "masterlist" in base_url:
                    character_file.write(f"{word}\t{word_url}\n")

        # Be respectful, only scrape a page once every 5-10 seconds (inclusive)
        time.sleep(random.randint(5, 10))

character_file.close()
word_file.close()
sentence_file.close()
