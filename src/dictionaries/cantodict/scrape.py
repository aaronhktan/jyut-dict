import csv
import random
import requests
import sys
import time


def read_csv(filename):
    with open(filename) as csvfile:
        reader = csv.reader(csvfile, delimiter='\t')
        for row in reader:
            yield row


index = 0

# Scrape words at indices from [second arg, third arg]
for item, url in read_csv(sys.argv[1]):
    index += 1
    if (index < int(sys.argv[3])):
        continue
    if (index > int(sys.argv[4])):
        break

    if url == "?":
        print(f"Skipped item {index} - {item}, no valid URL")
        continue

    while True:
        try:
            r = requests.get(url)
        except:
            print(f"Exception occured getting {index} - {item}, trying again")
            time.sleep(3)
            continue
        break

    # Save the webpage
    for variant in item.split("/"):
        with open(f"{sys.argv[2]}/scraped/{variant.strip()}.html", "w") as f:
            f.write(r.text)
        print(f"Scraped #{index} - {variant.strip()}")

    # Be respectful, only scrape a page once every 5-10 seconds (inclusive)
    time.sleep(random.randint(5, 10))