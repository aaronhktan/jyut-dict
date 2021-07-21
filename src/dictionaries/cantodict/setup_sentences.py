from bs4 import BeautifulSoup

import random
import requests
import sys
import time
import urllib

# For now, ignore incomplete compound words
urls = [
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=1", 22),
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=2", 54),
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=3", 73),
    ("https://www.cantonese.sheik.co.uk/scripts/examplelist.htm?level=4", 8),
]

sentences_file = open(sys.argv[1], "a")

for base_url, num_pages in urls:
    classname = "chinesemed"

    for page in range(num_pages):
        scrape_url = base_url + "&page=" + str(page)
        print("Scraping url: " + scrape_url)

        r = requests.get(scrape_url)
        document = BeautifulSoup(r.text, "html.parser")

        sentence_elems = document.find_all("td", class_=classname)

        for elem in sentence_elems:
            sentence = urllib.parse.quote(elem.get_text().encode('utf8'))
	    sentence_url = elem.parent.parent.find("a").get("href")
	    character_file.write(f"{sentence}\t{sentence_url}\n")

        # Be respectful, only scrape a page once every 5-10 seconds (inclusive)
        time.sleep(random.randint(5, 10))

sentences_file.close()
