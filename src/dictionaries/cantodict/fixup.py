from bs4 import BeautifulSoup
import requests

import random
import re
import sys
import time


FIXUP_REGEX_PATTERN = re.compile(
    r"WARNING:root:Hmm, looks like the parsed word (.*) doesn't match the filename (.*)"
)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(
            (
                "Usage: python3 -m cantodict.fixup "
                "<logging output filepath from parse.py> "
                "<scraped file output folder>"
            )
        )
        print(("e.g. python3 -m cantodict.fixup ./cantodict/logging.log ./cantodict/scraped_words/"))
        sys.exit(1)

    bad_words = []

    # Collect all the words that did not parse correctly
    with open(sys.argv[1]) as input_file:
        for line in input_file:
            if re.match(FIXUP_REGEX_PATTERN, line):
                bad_words.append(re.match(FIXUP_REGEX_PATTERN, line).group(2))

    # Attempt to fetch the entry page by getting the URL from the search results page
    # The link to the page is accessible via the icon with a book in it – so find that
    # element, and then download the webpage from that link.
    for word in bad_words:
        while True:
            try:
                r = requests.get(
                    f"http://www.cantonese.sheik.co.uk/dictionary/search/?searchtype=1&text={word.strip()}"
                )
            except:
                print(f"Exception occured searching for {word.strip()}, trying again")
                time.sleep(3)
                continue
            break

        soup = BeautifulSoup(r.text, "html.parser")

        link = ""
        for image in soup.find_all("img"):
            if image["src"] == "http://www.cantonese.sheik.co.uk/images/icons/book.gif":
                link = image.parent["href"]
                break

        if link:
            while True:
                try:
                    r = requests.get(link)
                except:
                    print(f"Exception occured getting {word}, trying again")
                    time.sleep(3)
                    continue
                break

            with open(f"{sys.argv[2]}{word.strip()}.html", "w") as f:
                f.write(r.text)

            print(f"Scraped {word.strip()}")
        else:
            logging.error(f"Unable to find link for word {word.strip()}!")

        # Be respectful, only scrape a page once every 5-10 seconds (inclusive)
        time.sleep(random.randint(5, 10))
